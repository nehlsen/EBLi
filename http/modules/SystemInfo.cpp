#include "SystemInfo.h"
#include <ebli_version.h>

#if defined(CONFIG_EBLI_CONFIG_MANAGER_ENABLE)
#include <ConfigManager.h>
#endif

#if defined(CONFIG_EBLI_OTA_ENABLE)
#include <esp_ota_ops.h>
#endif

namespace EBLi::http::module {

SystemInfo::SystemInfo():
    m_system_info_uri {
        .uri = BASE_URI "/system/info",
        .method = http_method::HTTP_GET,
        .handler = SystemInfo::getSystemInfoHttpHandler,
        .user_ctx = nullptr
    }
{}

std::vector<httpd_uri_t *> SystemInfo::getHandlers()
{
    return {&m_system_info_uri};
}

esp_err_t SystemInfo::getSystemInfoHttpHandler(httpd_req_t *request)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "idfVersion", IDF_VER);

    responseAddChipInfo(root);
    responseAddLastResetReason(root);
    responseAddRestartCounter(root);
    responseAddOtaInfo(root);
    responseAddEbliInfo(root);

    return sendJsonResponse(root, request);
}

void SystemInfo::responseAddChipInfo(cJSON *responseObject)
{
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    cJSON_AddNumberToObject(responseObject, "cores", chip_info.cores);
    cJSON_AddNumberToObject(responseObject, "chip_revision", chip_info.revision);
}

void SystemInfo::responseAddLastResetReason(cJSON *responseObject)
{
    switch (esp_reset_reason()) {
        case ESP_RST_UNKNOWN:
            cJSON_AddStringToObject(responseObject, "reset_reason", "Reset reason can not be determined");
            break;
        case ESP_RST_POWERON:
            cJSON_AddStringToObject(responseObject, "reset_reason", "Reset due to power-on event");
            break;
        case ESP_RST_EXT:
            cJSON_AddStringToObject(responseObject, "reset_reason", "Reset by external pin");
            break;
        case ESP_RST_SW:
            cJSON_AddStringToObject(responseObject, "reset_reason", "Software reset via esp_restart");
            break;
        case ESP_RST_PANIC:
            cJSON_AddStringToObject(responseObject, "reset_reason", "Software reset due to exception/panic");
            break;
        case ESP_RST_INT_WDT:
            cJSON_AddStringToObject(responseObject, "reset_reason", "Reset (software or hardware) due to interrupt watchdog");
            break;
        case ESP_RST_TASK_WDT:
            cJSON_AddStringToObject(responseObject, "reset_reason", "Reset due to task watchdog");
            break;
        case ESP_RST_WDT:
            cJSON_AddStringToObject(responseObject, "reset_reason", "Reset due to other watchdogs");
            break;
        case ESP_RST_DEEPSLEEP:
            cJSON_AddStringToObject(responseObject, "reset_reason", "Reset after exiting deep sleep mode");
            break;
        case ESP_RST_BROWNOUT:
            cJSON_AddStringToObject(responseObject, "reset_reason", "Brownout reset (software or hardware)");
            break;
        case ESP_RST_SDIO:
            cJSON_AddStringToObject(responseObject, "reset_reason", "Reset over SDIO");
            break;
    }
}

void SystemInfo::responseAddRestartCounter(cJSON *responseObject)
{
#if defined(CONFIG_EBLI_CONFIG_MANAGER_ENABLE)
    cJSON_AddNumberToObject(responseObject, "restarts", EBLi::ConfigManager::instance()->getRestartCounter());
#endif
}

void SystemInfo::responseAddOtaInfo(cJSON *responseObject)
{
#if defined(CONFIG_EBLI_OTA_ENABLE)
    const esp_app_desc_t *app_desc = esp_ota_get_app_description();
    cJSON_AddStringToObject(responseObject, "projectName", app_desc->project_name);
    cJSON_AddStringToObject(responseObject, "projectVersion", app_desc->version);
    cJSON_AddStringToObject(responseObject, "compileTime", app_desc->time);
    cJSON_AddStringToObject(responseObject, "compileDate", app_desc->date);
#endif
}

void SystemInfo::responseAddEbliInfo(cJSON *responseObject)
{
    cJSON_AddStringToObject(responseObject, "EBLi_version", EBLi_VERSION_HASH);
    cJSON_AddStringToObject(responseObject, "EBLi_components", EBLi_COMPONENTS_ENABLED);
}

}
