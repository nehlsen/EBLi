#include "OtaUpdater.h"
#include "OtaUpdaterPrivate.h"
#include <freertos/event_groups.h>
#include <esp_log.h>
#include <tcpip_adapter.h>
#include <esp_event.h>

#if defined(CONFIG_EBLI_HTTP_ENABLE)
#include <WebServer.h>
#include "OtaHttpModule.h"
#endif

namespace EBLi {

static const char *UPDATER_LOG_TAG = "EBLi:OtaUpdater";

#define UPDATER_RUNNING_BIT BIT1
static EventGroupHandle_t ota_updater_event_group = nullptr;

[[noreturn]] void ota_updater_task(void *pvParameter)
{
    ESP_LOGI(UPDATER_LOG_TAG, "Starting OTA Update Task...");

    auto updater = static_cast<OtaUpdaterPrivate*>(pvParameter);

    while (true) { // this will only loop if the update fails
        xEventGroupWaitBits(ota_updater_event_group, UPDATER_RUNNING_BIT, false, false, portMAX_DELAY);
        ESP_LOGI(UPDATER_LOG_TAG, "Starting Update...");
        updater->work();
        xEventGroupClearBits(ota_updater_event_group, UPDATER_RUNNING_BIT);
    }
}

static void on_got_ip(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (nullptr != ota_updater_event_group) {
        return;
    }
    auto updater = static_cast<OtaUpdaterPrivate*>(event_handler_arg);
    if (nullptr == updater) {
        return;
    }

    ota_updater_event_group = xEventGroupCreate();
    xTaskCreate(
            &ota_updater_task,
            "ota_updater_task",
            4000,
            updater,
            1,
            nullptr
    );
}

/**********************************************************************************************************************/

OtaUpdater::OtaUpdater():
    m_updater(new OtaUpdaterPrivate)
{
    esp_log_level_set("esp_https_ota", ESP_LOG_DEBUG); // log download progress

    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, on_got_ip, m_updater);

#if defined(CONFIG_EBLI_HTTP_ENABLE)
    auto srv = http::WebServer::instance();
    srv->addModule(new ota::OtaHttpModule(this));
#endif
}

void OtaUpdater::setUpdateUrl(const char *url)
{
    if (m_updater->isActive()) {
        return;
    }

    m_updater->httpConfig.url = url;
}

bool OtaUpdater::startUpdate()
{
    if (m_updater->isActive()) {
        return false;
    }

    m_updater->httpConfig.disable_auto_redirect = false;
    m_updater->httpConfig.max_redirection_count = 10;

    m_updater->lastError = esp_https_ota_begin(&m_updater->otaConfig, &m_updater->otaHandle);
    if (m_updater->lastError == ESP_OK) {
        xEventGroupSetBits(ota_updater_event_group, UPDATER_RUNNING_BIT);
        return true;
    }

    return false;
}

esp_err_t OtaUpdater::getLastError() const
{
    return m_updater->lastError;
}

}
