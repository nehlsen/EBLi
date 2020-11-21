#include "OtaUpdaterPrivate.h"
#include <esp_https_ota.h>
#include <esp_ota_ops.h>
#include <ebli_events.h>
#include <ebli_log.h>
#include <cstring>

namespace EBLi::ota {

#define MAX_MESSAGE_LENGTH 128 // maximum size of a message to post to the event system

OtaUpdaterPrivate::OtaUpdaterPrivate():
    httpConfig(), otaConfig({.http_config = &httpConfig})
{}

bool OtaUpdaterPrivate::isActive() const
{
    return nullptr != otaHandle;
}

void OtaUpdaterPrivate::work()
{
    postEventMessage(EBLI_EVENT_OTA_STARTED, otaConfig.http_config->url);

    esp_app_desc_t updateInfo;
    lastError = esp_https_ota_get_img_desc(otaHandle, &updateInfo);
    if (updateInfo.magic_word != ESP_APP_DESC_MAGIC_WORD) {
        char msg[MAX_MESSAGE_LENGTH];
        sprintf(msg, "Magic-Word mismatch! (update:0x%x != mine:0x%x)", updateInfo.magic_word, ESP_APP_DESC_MAGIC_WORD);
        ESP_LOGE(LOG_TAG_OTA, "%s", msg);
        lastError = ESP_ERR_OTA_VALIDATE_FAILED;
        postEventMessage(EBLI_EVENT_OTA_FAILED, msg);
    }
    if (lastError == ESP_OK) {
        {
            char msg[MAX_MESSAGE_LENGTH];
            sprintf(msg, "Image: %s, version:%s (%s / %s)",
                    updateInfo.project_name,
                    updateInfo.version,
                    updateInfo.date,
                    updateInfo.time);
            postEventMessage(EBLI_EVENT_OTA_PROGRESS, msg);
            ESP_LOGI(LOG_TAG_OTA, "%s", msg);
        }

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

    if (lastError == ESP_OK) {
        postEventMessage(EBLI_EVENT_OTA_SUCCEED, "OTA Succeed");
    } else {
        char msg[MAX_MESSAGE_LENGTH];
        sprintf(msg, "OTA Failed: [%d]: %s", lastError, esp_err_to_name(lastError));
        postEventMessage(EBLI_EVENT_OTA_FAILED, msg);
    }
}

void OtaUpdaterPrivate::postEventMessage(int32_t event_id, const char *msg)
{
    char msgCopy[MAX_MESSAGE_LENGTH];
    sprintf(msgCopy, "%.*s", MAX_MESSAGE_LENGTH-1, msg);

    postEventMessage(event_id, msgCopy);
}

void OtaUpdaterPrivate::postEventMessage(int32_t event_id, char *msg)
{
    esp_event_post(EBLI_EVENTS, event_id, static_cast<void*>(msg), strlen(msg), EBLI_EVENTS_MAX_TICKS_TO_POST);
}

}
