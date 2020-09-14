#include "FS.h"
#include <nvs_flash.h>
#include <esp_log.h>

namespace EBLi
{

static const char *LOG_TAG = "EBLi:FS";

bool FS::m_isInitialized = false;

esp_err_t FS::init()
{
    if (m_isInitialized) {
        ESP_LOGW(LOG_TAG, "already initialized...");
        return ESP_OK;
        // maybe? e.g. esp_event_loop_create_default() does the same...
//        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = initNvs();
    if (ret != ESP_OK) {
        return ret;
    }

    ret = initSpiffs();
    if (ret != ESP_OK) {
        return ret;
    }

    logFsInfo();
    m_isInitialized = true;

    return ESP_OK;
}

esp_err_t FS::initNvs()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(LOG_TAG, "NVS: No Free Pages or new NVS Version. Erasing and retrying...");
        ret = nvs_flash_erase();
        ret |= nvs_flash_init();
    }

    if (ret != ESP_OK) {
        ESP_LOGE(LOG_TAG, "NVS: Failed to initialize NVS");
    }
    ESP_LOGI(LOG_TAG, "NVS: Init complete");

    return ret;
}

esp_err_t FS::initSpiffs()
{
    esp_vfs_spiffs_conf_t conf = createSpiffsConfig();
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(LOG_TAG, "SPIFFS: Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(LOG_TAG, "SPIFFS: Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(LOG_TAG, "SPIFFS: Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

    return ret;
}

esp_vfs_spiffs_conf_t FS::createSpiffsConfig()
{
    return {
            .base_path = "",
            .partition_label = nullptr,
            .max_files = 5,
            .format_if_mount_failed = false
    };
}

void FS::logFsInfo(const char* partition_label)
{
    size_t total = 0, used = 0;
    esp_err_t ret = esp_spiffs_info(partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(LOG_TAG, "SPIFFS: Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(LOG_TAG, "SPIFFS: Partition size: total: %d, used: %d", total, used);
    }
}

}
