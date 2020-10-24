#include "Wifip.h"
#include <ebli_log.h>
#include <tcpip_adapter.h>
#include <esp_event.h>
#include <esp_wifi.h>
#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_ble.h>
#include <cstring>
#include <ebli_events.h>

namespace EBLi {

#define RESET_WIFI_AFTER_ERROR_COUNT 15
static uint8_t wifi_connect_error_count = 0;
static const char *PROVISIONING_SERVICE_PREFIX = "PROV_";

esp_err_t Wifip::init()
{
    tcpip_adapter_init();
    esp_err_t ret = provisionWifi();
    if (ret != ESP_OK) {
        return ret;
    }

    return ESP_OK;
}

esp_err_t Wifip::provisionWifi()
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.wifi_task_core_id = 1;
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip, nullptr));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, nullptr));

    wifi_prov_mgr_config_t config = {
            .scheme = wifi_prov_scheme_ble,
            .scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BT, // FIXME

            .app_event_handler = {
//                    .event_cb = prov_event_handler,
                    .event_cb = provisioning_event_handler,
                    .user_data = nullptr
            }
    };

    ESP_ERROR_CHECK(wifi_prov_mgr_init(config));
    bool provisioned = false;
    ESP_ERROR_CHECK(wifi_prov_mgr_is_provisioned(&provisioned));

    if (!provisioned) {
        startProvisioning();
    } else {
        ESP_LOGI(LOG_TAG_WIFI, "Already provisioned, starting Wi-Fi Station");
        wifi_prov_mgr_deinit();
        startWifiStation();
    }

    return ESP_OK;
}

void Wifip::startProvisioning()
{
    ESP_LOGI(LOG_TAG_WIFI, "Starting provisioning");

    char advertisedBleDeviceName[12];
    prov_service_name(advertisedBleDeviceName, sizeof(advertisedBleDeviceName));

    /* This step is only useful when scheme is wifi_prov_scheme_ble. This will
     * set a custom 128 bit UUID which will be included in the BLE advertisement
     * and will correspond to the primary GATT service that provides provisioning
     * endpoints as GATT characteristics. Each GATT characteristic will be
     * formed using the primary service UUID as base, with different auto assigned
     * 12th and 13th bytes (assume counting starts from 0th byte). The client side
     * applications must identify the endpoints by reading the User Characteristic
     * Description descriptor (0x2901) for each characteristic, which contains the
     * endpoint name of the characteristic */
    uint8_t custom_service_uuid[] = {
            /* LSB <---------------------------------------
             * ---------------------------------------> MSB */
            0x21, 0x43, 0x65, 0x87, 0x09, 0xba, 0xdc, 0xfe,
            0xef, 0xcd, 0xab, 0x90, 0x78, 0x56, 0x34, 0x12
    };
    wifi_prov_scheme_ble_set_service_uuid(custom_service_uuid);

    /* Start provisioning service */
    ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(WIFI_PROV_SECURITY_1, CONFIG_EBLI_WIFI_PROVISION_POP, advertisedBleDeviceName, nullptr));
}

void Wifip::prov_service_name(char *service_name, size_t max)
{
    uint8_t eth_mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    snprintf(service_name, max, "%s%02X%02X%02X", PROVISIONING_SERVICE_PREFIX, eth_mac[3], eth_mac[4], eth_mac[5]);
}

void Wifip::startWifiStation()
{
    ESP_LOGI(LOG_TAG_WIFI, "Starting Wifi Station...");
    esp_event_post(EBLI_EVENTS, EBLI_EVENT_WIFI_CONNECTING, nullptr, 0, EBLI_EVENTS_MAX_TICKS_TO_POST);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());
}

void Wifip::provisioning_event_handler(void *user_data, wifi_prov_cb_event_t event, void *event_data)
{
    switch (event) {
        case WIFI_PROV_START:
            ESP_LOGI(LOG_TAG_WIFI, "Provisioning started");
            esp_event_post(EBLI_EVENTS, EBLI_EVENT_WIFI_PROVISIONING, nullptr, 0, EBLI_EVENTS_MAX_TICKS_TO_POST);
            break;
        case WIFI_PROV_CRED_RECV: {
            wifi_sta_config_t *wifi_sta_cfg = (wifi_sta_config_t *)event_data;
            /* If SSID length is exactly 32 bytes, null termination
             * will not be present, so explicitly obtain the length */
            size_t ssid_len = strnlen((const char *)wifi_sta_cfg->ssid, sizeof(wifi_sta_cfg->ssid));
            ESP_LOGI(LOG_TAG_WIFI,
                     "Received Wi-Fi credentials\n\tSSID     : %.*s\n\tPassword : %s",
                     ssid_len, (const char *) wifi_sta_cfg->ssid,
                     (const char *) wifi_sta_cfg->password
            );
            break;
        }
        case WIFI_PROV_CRED_FAIL: {
            wifi_prov_sta_fail_reason_t *reason = (wifi_prov_sta_fail_reason_t *)event_data;
            ESP_LOGE(LOG_TAG_WIFI,
                     "Provisioning failed!\n\tReason : %s\n\tPlease reset to factory and retry provisioning",
                     (*reason == WIFI_PROV_STA_AUTH_ERROR) ? "Wi-Fi AP password incorrect" : "Wi-Fi AP not found"
            );
            break;
        }
        case WIFI_PROV_CRED_SUCCESS:
            ESP_LOGI(LOG_TAG_WIFI, "Provisioning successful");
            break;
        case WIFI_PROV_END:
            /* De-initialize manager once provisioning is finished */
            wifi_prov_mgr_deinit();
            break;
        default:
            break;
    }
}

void Wifip::on_got_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    wifi_connect_error_count = 0;

    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(LOG_TAG_WIFI, "Got IP: IPv4 address: " IPSTR, IP2STR(&event->ip_info.ip));
    esp_event_post(EBLI_EVENTS, EBLI_EVENT_WIFI_CONNECTED, nullptr, 0, EBLI_EVENTS_MAX_TICKS_TO_POST);
}

void Wifip::on_wifi_disconnect(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    wifi_connect_error_count += 1;

    if (wifi_connect_error_count == RESET_WIFI_AFTER_ERROR_COUNT) {
        ESP_LOGW(LOG_TAG_WIFI, "Wi-Fi disconnected - limit reached (%d/%d)", wifi_connect_error_count, RESET_WIFI_AFTER_ERROR_COUNT);
        ESP_LOGW(LOG_TAG_WIFI, "resetting Wi-Fi Config...");

        wifi_config_t wifiCfg;
        esp_wifi_get_config(ESP_IF_WIFI_STA, &wifiCfg);
        std::memset(wifiCfg.sta.ssid, 0, sizeof(wifiCfg.sta.ssid));
        std::memset(wifiCfg.sta.password, 0, sizeof(wifiCfg.sta.password));

        if (esp_wifi_set_config(ESP_IF_WIFI_STA, &wifiCfg) != ESP_OK) {
            ESP_LOGE(LOG_TAG_WIFI, "failed to reset Wi-Fi config");
        } else {
            ESP_LOGW(LOG_TAG_WIFI, "config has been reset, next reboot will start provisioning mode...");
            countdownAndRestart(5);
        }
    }

    ESP_LOGI(LOG_TAG_WIFI, "Wi-Fi disconnected (%d/%d), trying to reconnect...", wifi_connect_error_count, RESET_WIFI_AFTER_ERROR_COUNT);
    esp_event_post(EBLI_EVENTS, EBLI_EVENT_WIFI_DISCONNECTED, nullptr, 0, EBLI_EVENTS_MAX_TICKS_TO_POST);
    ESP_ERROR_CHECK(esp_wifi_connect());
}

void Wifip::countdownAndRestart(uint8_t countdown)
{
    do {
        ESP_LOGW(LOG_TAG_WIFI, "reboot in %d...", countdown);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        countdown--;
    } while (countdown > 0);

    esp_restart();
}

}
