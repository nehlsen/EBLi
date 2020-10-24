#include "Status.h"
#include <ebli_log.h>
#include <esp_event.h>
#include <LedBlinker.h>
#include <ebli_events.h>

#define LED_GPIO GPIO_NUM_2

namespace EBLi::Status {

static const char *LOG_TAG = "EBLi:Status";

static void on_ebli_event(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    auto status = static_cast<Status*>(event_handler_arg);
    ESP_ERROR_CHECK(nullptr == status ? ESP_FAIL : ESP_OK);

    switch (event_id) {
        case EBLI_EVENT_WIFI_CONNECTING:
            ESP_LOGI(LOG_TAG, "EBLI_EVENT_WIFI_CONNECTING");
            status->setWifiState(Status::WifiState::WIFI_CONNECTING);
            break;
        case EBLI_EVENT_WIFI_PROVISIONING:
            ESP_LOGI(LOG_TAG, "EBLI_EVENT_WIFI_PROVISIONING");
            status->setWifiState(Status::WifiState::WIFI_PROVISIONING);
            break;
        case EBLI_EVENT_WIFI_CONNECTED:
            ESP_LOGI(LOG_TAG, "EBLI_EVENT_WIFI_CONNECTED");
            status->setWifiState(Status::WifiState::WIFI_CONNECTED);
            break;
        case EBLI_EVENT_WIFI_DISCONNECTED:
            ESP_LOGI(LOG_TAG, "EBLI_EVENT_WIFI_DISCONNECTED");
            status->setWifiState(Status::WifiState::WIFI_DISCONNECTED);
            break;

        case EBLI_EVENT_MQTT_CONNECTED:
            ESP_LOGI(LOG_TAG, "EBLI_EVENT_MQTT_CONNECTED");
            status->setMqttState(Status::MqttState::MQTT_CONNECTED);
            break;
        case EBLI_EVENT_MQTT_DISCONNECTED:
            ESP_LOGI(LOG_TAG, "EBLI_EVENT_MQTT_DISCONNECTED");
            status->setMqttState(Status::MqttState::MQTT_DISCONNECTED);
            break;

        default:
            // ignore event
            break;
    }
}

Status::Status()
{
    m_ledBlinker = new Utility::LedBlinker(LED_GPIO);
    updateBlinker();

    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_handler_register(EBLI_EVENTS, ESP_EVENT_ANY_ID, on_ebli_event, this));
}

void Status::setWifiState(WifiState state)
{
    m_wifiState = state;
    updateBlinker();
}

void Status::setMqttState(MqttState state)
{
    m_mqttState = state;
    updateBlinker();
}

void Status::updateBlinker()
{
    switch (m_wifiState) {
        case WifiState::WIFI_DISCONNECTED:
            m_ledBlinker
                    ->setPulseTime(100)
                    ->setPulseCount(5)
                    ->setPauseTime(750);
            return;
        case WifiState::WIFI_CONNECTING:
            m_ledBlinker
                    ->setPulseTime(100, 150)
                    ->setPulseCount(2)
                    ->setPauseTime(750);
            return;
        case WifiState::WIFI_PROVISIONING:
            m_ledBlinker
                    ->setPulseTime(500)
                    ->setPulseCount(1)
                    ->setPauseTime(0);
            return;

        default:
            // ignored
            break;
    }

    switch (m_mqttState) {
        case MqttState::MQTT_DISCONNECTED:
            m_ledBlinker
                    ->setPulseTime(1000)
                    ->setPulseCount(1)
                    ->setPauseTime(0);
            return;
        case MqttState::MQTT_CONNECTED:
            m_ledBlinker
                    ->setPulseTime(100)
                    ->setPulseCount(1)
                    ->setPauseTime(2000);
            return;

        default:
            // ignored
            break;
    }
}

}
