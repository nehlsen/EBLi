#include <esp_log.h>
#include "ConfigManager.h"

namespace EBLi {

static const char *LOG_TAG = "EBLi:ConfigManager";

#define NVS_NAMESPACE "ebli_config"
// NOTE keys are limited to 15 characters
#define MAXIMUM_CONFIG_KEY_LENGTH 15
#define MAXIMUM_CONFIG_VALUE_LENGTH 64
#define RESTART_COUNTER_KEY "restart_counter"

ConfigManager *ConfigManager::s_instance = nullptr;

ConfigManager *ConfigManager::init()
{
    return instance();
}

ConfigManager *ConfigManager::instance()
{
    if (nullptr == s_instance) {
        s_instance = new ConfigManager;
    }

    return s_instance;
}

bool ConfigManager::open()
{
    if (isOpen()) {
        return true;
    }

    if (!openNvs()) {
        return false;
    }

    logNvsState();
    
    updateRestartCounter();
    m_isOpen = true;
    return true;
}

bool ConfigManager::isOpen() const
{
    return m_isOpen;
}

bool ConfigManager::openNvs()
{
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &m_nvsHandle);
    if (err != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return false;
    }

    return true;
}

void ConfigManager::logNvsState()
{
    nvs_stats_t nvs_stats;
    nvs_get_stats(nullptr, &nvs_stats);
    ESP_LOGI(LOG_TAG, "NVS: UsedEntries = (%d), FreeEntries = (%d), AllEntries = (%d)",
             nvs_stats.used_entries, nvs_stats.free_entries, nvs_stats.total_entries);
}

void ConfigManager::close()
{
    if (!isOpen()) {
        return;
    }

    nvs_close(m_nvsHandle);
    m_isOpen = false;
}

void ConfigManager::setAutoCommitEnabled(bool enabled)
{
    m_autoCommitEnabled = enabled;
}

bool ConfigManager::isAutoCommitEnabled() const
{
    return m_autoCommitEnabled;
}

bool ConfigManager::commit()
{
    if (!isOpen()) {
        return false;
    }

    esp_err_t err = nvs_commit(m_nvsHandle);
    
    if (err != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Error (%s) committing NVS Data!", esp_err_to_name(err));
        return false;
    }

    return true;
}

int32_t ConfigManager::getValue(const std::string &key, int32_t defaultValue) const
{
    if (!isOpen()) {
        return 0;
    }
    if (!checkKeyAndLog(key)) {
        return 0;
    }

    int32_t value = defaultValue;
    esp_err_t err = nvs_get_i32(m_nvsHandle, key.c_str(), &value);

    if (err != ESP_OK) {
        ESP_LOGW(LOG_TAG, "NVS read Error (%s), key:\"%s\"", esp_err_to_name(err), key.c_str());
    }
    
    return value;
}

bool ConfigManager::setValue(const std::string &key, int32_t value)
{
    if (!isOpen()) {
        return false;
    }
    if (!checkKeyAndLog(key)) {
        return false;
    }

    esp_err_t err = nvs_set_i32(m_nvsHandle, key.c_str(), value);

    if (err != ESP_OK) {
        ESP_LOGE(LOG_TAG, "NVS write Error (%s), key:\"%s\"", esp_err_to_name(err), key.c_str());
        return false;
    }

    return autoCommit();
}

std::string ConfigManager::getValue(const std::string &key, const std::string &defaultValue) const
{
    if (!isOpen()) {
        return "";
    }
    if (!checkKeyAndLog(key)) {
        return "";
    }
    if (defaultValue.length() > MAXIMUM_CONFIG_VALUE_LENGTH) {
        ESP_LOGW(LOG_TAG, "Default value for \"%s\" is too long", key.c_str());
        return "";
    }

    size_t required_size;
    esp_err_t err = nvs_get_str(m_nvsHandle, key.c_str(), nullptr, &required_size);
    if (err != ESP_OK) {
        ESP_LOGW(LOG_TAG, "NVS read Error (%s), key:\"%s\"", esp_err_to_name(err), key.c_str());
        return defaultValue;
    }
    if (required_size >= 64) {
        ESP_LOGW(LOG_TAG, "NVS read failed - max size exceeded, key:\"%s\"", key.c_str());
        return defaultValue;
    }

    char* value = (char*)malloc(required_size);
    err = nvs_get_str(m_nvsHandle, key.c_str(), value, &required_size);
    if (err != ESP_OK) {
        ESP_LOGW(LOG_TAG, "NVS read Error (%s), key:\"%s\"", esp_err_to_name(err), key.c_str());
        return defaultValue;
    }

    auto ret = std::string(value);
    free(value);
    return ret;
}

bool ConfigManager::setValue(const std::string &key, const std::string &value)
{
    if (!isOpen()) {
        return false;
    }
    if (!checkKeyAndLog(key)) {
        return false;
    }
    if (value.length() > MAXIMUM_CONFIG_VALUE_LENGTH) {
        ESP_LOGW(LOG_TAG, "Config value for \"%s\" is too long", key.c_str());
        return false;
    }

    esp_err_t err = nvs_set_str(m_nvsHandle, key.c_str(), value.c_str());
    if (err != ESP_OK) {
        ESP_LOGE(LOG_TAG, "NVS write Error (%s), key:\"%s\"", esp_err_to_name(err), key.c_str());
        return false;
    }

    return autoCommit();
}

int32_t ConfigManager::getRestartCounter()
{
    if (m_restartCounter == 0) {
        m_restartCounter = getValue(RESTART_COUNTER_KEY, 0);
    }
    
    return m_restartCounter;
}

bool ConfigManager::isKeyValid(const std::string &key)
{
    return key.length() <= MAXIMUM_CONFIG_KEY_LENGTH;
}

bool ConfigManager::checkKeyAndLog(const std::string &key)
{
    if (isKeyValid(key)) {
        return true;
    }

    ESP_LOGW(LOG_TAG, "Config key \"%s\" is too long", key.c_str());
    return false;
}

bool ConfigManager::autoCommit()
{
    if (!isAutoCommitEnabled()) {
        return true;
    }
    
    return commit();
}

void ConfigManager::updateRestartCounter()
{
    if (m_restartCounter > 0) {
        // already incremented...
        return;
    }

    int32_t newCount = getRestartCounter() + 1;
    if (setValue(RESTART_COUNTER_KEY, newCount)) {
        ESP_LOGI(LOG_TAG, "Restart counter updated to %d", newCount);
        m_restartCounter = newCount;
    }
}

}
