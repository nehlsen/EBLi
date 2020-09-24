#include "OtaUpdater.h"
#include "OtaUpdaterPrivate.h"
#include <freertos/event_groups.h>
#include <esp_log.h>

namespace EBLi {

static const char *UPDATER_LOG_TAG = "EBLi:OtaUpdater";

#define UPDATER_RUNNING_BIT BIT1
static EventGroupHandle_t ota_updater_event_group;

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

/**********************************************************************************************************************/

OtaUpdater::OtaUpdater():
    m_updater(new OtaUpdaterPrivate)
{
    esp_log_level_set("esp_https_ota", ESP_LOG_DEBUG); // log download progress

    // FIXME start on_got_ip

    ota_updater_event_group = xEventGroupCreate();
    xTaskCreate(
            &ota_updater_task,
            "ota_updater_task",
            4000,
            m_updater,
            1,
            nullptr
    );
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
