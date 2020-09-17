#include "OtaUpdaterPrivate.h"
#include <esp_https_ota.h>
#include <esp_ota_ops.h>
#include <esp_log.h>

namespace EBLi {

static const char *UPDATER_LOG_TAG = "OtaUpdater";

OtaUpdaterPrivate::OtaUpdaterPrivate():
    httpConfig(), otaConfig({.http_config = &httpConfig})
{}

bool OtaUpdaterPrivate::isActive() const
{
    return nullptr != otaHandle;
}

void OtaUpdaterPrivate::work()
{
    esp_app_desc_t updateInfo;
    lastError = esp_https_ota_get_img_desc(otaHandle, &updateInfo);
    if (updateInfo.magic_word != ESP_APP_DESC_MAGIC_WORD) {
        ESP_LOGE(UPDATER_LOG_TAG, "Update info: magic word mismatch!");
        lastError = ESP_ERR_OTA_VALIDATE_FAILED;
    }
    if (lastError == ESP_OK) {
        ESP_LOGI(
                UPDATER_LOG_TAG,
                "Update info: name:%s, version:%s, compiled: %s / %s",
                updateInfo.project_name,
                updateInfo.version,
                updateInfo.date,
                updateInfo.time
        );

        while (true) { // download loop
            lastError = esp_https_ota_perform(otaHandle);
            if (lastError != ESP_ERR_HTTPS_OTA_IN_PROGRESS) {
                // something failed. e.g. download failed, failed to write to flash, ...
                // OR: ESP_OK: download complete
                break;
            }
        }
    }

    // cleanup broken update OR finish successful update
    esp_err_t ota_finish_err = esp_https_ota_finish(otaHandle); // free's otaHandle
    otaHandle = nullptr;

    if (lastError == ESP_OK) {
        // if esp_https_ota_perform() was successful, overwrite last-error with the one from finish
        lastError = ota_finish_err;
    }
}

}
