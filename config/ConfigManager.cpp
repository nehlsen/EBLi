#include "ConfigManager.h"
#include <ebli_log.h>
#include <algorithm>
#include <cJSON.h>
#include <ebli_events.h>

#if defined(CONFIG_EBLI_HTTP_ENABLE)
#include <WebServer.h>
#include "ConfigHttpModule.h"
#endif

namespace EBLi::config {

#define NVS_NAMESPACE "ebli_config"
// NOTE keys are limited to 15 characters
#define MAXIMUM_CONFIG_KEY_LENGTH 15
#define MAXIMUM_CONFIG_VALUE_LENGTH 64
#define RESTART_COUNTER_KEY "restart_counter"

ConfigManager *ConfigManager::s_instance = nullptr;

ConfigManager *ConfigManager::init()
{
    ConfigManager *singleton = instance();
    singleton->open();

#if defined(CONFIG_EBLI_HTTP_ENABLE)
    auto srv = http::WebServer::instance();
    srv->addModule(new ConfigHttpModule());
#endif

    return singleton;
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

    m_isOpen = true;

    logNvsState();
    updateRestartCounter();

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
        ESP_LOGE(LOG_TAG_CONFIG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return false;
    }

    return true;
}

void ConfigManager::logNvsState()
{
    if (!isOpen()) {
        return;
    }

    nvs_stats_t nvs_stats;
    nvs_get_stats(nullptr, &nvs_stats);
    ESP_LOGI(LOG_TAG_CONFIG, "NVS: UsedEntries = (%d), FreeEntries = (%d), AllEntries = (%d)",
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

ConfigProperty * ConfigManager::property(const std::string &shortKey, const std::string &longKey)
{
    auto property = getProperty(shortKey);
    if (nullptr == property) {
        property = createProperty(shortKey, longKey);
    }

    return property;
}

ConfigProperty *ConfigManager::createProperty(const std::string &shortKey, const std::string &longKey)
{
    auto property = getProperty(shortKey);
    if (nullptr != property) {
        return property;
    }

    ESP_LOGV(LOG_TAG_CONFIG, "createProperty('%s')", shortKey.c_str());

    property = new ConfigProperty(shortKey, longKey);
    m_properties.push_back(property);

    esp_event_post(EBLI_EVENTS, EBLI_EVENT_CONFIG_PROPERTY_REGISTERED, (void*)&property, sizeof(ConfigProperty*), portMAX_DELAY);

    return property;
}

ConfigProperty * ConfigManager::getProperty(const std::string &shortKey)
{
    auto it = std::find_if(m_properties.begin(), m_properties.end(), [shortKey](const ConfigProperty *property) {
        return property->getShortKey() == shortKey;
    });

    if (it != m_properties.end()) {
        ESP_LOGV(LOG_TAG_CONFIG, "getProperty('%s') found", shortKey.c_str());
        return *it;
    }

    ESP_LOGV(LOG_TAG_CONFIG, "getProperty('%s') not found", shortKey.c_str());

    return nullptr;
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
        ESP_LOGE(LOG_TAG_CONFIG, "Error (%s) committing NVS Data!", esp_err_to_name(err));
        return false;
    }

    return true;
}

int32_t ConfigManager::getValue(const std::string &key, int32_t defaultValue) const
{
    if (!isOpen()) {
        return defaultValue;
    }
    if (!checkKeyAndLog(key)) {
        return defaultValue;
    }

    int32_t value = defaultValue;
    esp_err_t err = nvs_get_i32(m_nvsHandle, key.c_str(), &value);
    if (err != ESP_OK) {
        ESP_LOGW(LOG_TAG_CONFIG, "NVS read Error (%s), key:\"%s\", fallback to %d", esp_err_to_name(err), key.c_str(), defaultValue);
        return defaultValue;
    }
    
    return value;
}

bool ConfigManager::setValue(const std::string &key, int32_t value)
{
    ESP_LOGD(LOG_TAG_CONFIG, "setValue('%s')", key.c_str());

    if (!isOpen()) {
        return false;
    }
    if (!checkKeyAndLog(key)) {
        return false;
    }

    esp_err_t err = nvs_set_i32(m_nvsHandle, key.c_str(), value);

    if (err != ESP_OK) {
        ESP_LOGE(LOG_TAG_CONFIG, "NVS write Error (%s), key:\"%s\"", esp_err_to_name(err), key.c_str());
        return false;
    }

    return autoCommit();
}

std::string ConfigManager::getValue(const std::string &key, const std::string &defaultValue) const
{
    if (!isOpen()) {
        return defaultValue;
    }
    if (!checkKeyAndLog(key)) {
        return defaultValue;
    }
    if (defaultValue.length() > MAXIMUM_CONFIG_VALUE_LENGTH) {
        ESP_LOGW(LOG_TAG_CONFIG, "Default value for \"%s\" is too long", key.c_str());
    }

    size_t required_size;
    esp_err_t err = nvs_get_str(m_nvsHandle, key.c_str(), nullptr, &required_size);
    if (err != ESP_OK) {
        ESP_LOGW(LOG_TAG_CONFIG, "NVS read Error (%s), key:\"%s\"", esp_err_to_name(err), key.c_str());
        return defaultValue;
    }
    if (required_size >= 64) {
        ESP_LOGW(LOG_TAG_CONFIG, "NVS read failed - max size exceeded, key:\"%s\"", key.c_str());
        return defaultValue;
    }

    char* value = (char*)malloc(required_size);
    err = nvs_get_str(m_nvsHandle, key.c_str(), value, &required_size);
    if (err != ESP_OK) {
        ESP_LOGW(LOG_TAG_CONFIG, "NVS read Error (%s), key:\"%s\"", esp_err_to_name(err), key.c_str());
        return defaultValue;
    }

    auto ret = std::string(value);
    free(value);
    return ret;
}

bool ConfigManager::setValue(const std::string &key, const std::string &value)
{
    ESP_LOGD(LOG_TAG_CONFIG, "setValue('%s')", key.c_str());

    if (!isOpen()) {
        return false;
    }
    if (!checkKeyAndLog(key)) {
        return false;
    }
    if (value.length() > MAXIMUM_CONFIG_VALUE_LENGTH) {
        ESP_LOGW(LOG_TAG_CONFIG, "Config value for \"%s\" is too long", key.c_str());
        return false;
    }

    esp_err_t err = nvs_set_str(m_nvsHandle, key.c_str(), value.c_str());
    if (err != ESP_OK) {
        ESP_LOGE(LOG_TAG_CONFIG, "NVS write Error (%s), key:\"%s\"", esp_err_to_name(err), key.c_str());
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

cJSON *ConfigManager::toJson()
{
    cJSON *configObject = cJSON_CreateObject();

    for (auto property : m_properties) {
        if (property->isVisibilityHidden()) {
            continue;
        }

        property->toJson(configObject);
    }

    return configObject;
}

bool ConfigManager::fromJson(cJSON *configObject)
{
    bool atLeastOneValueChanged = false;

    for (auto property : m_properties) {
        if (property->isAccessibilityReadOnly()) {
            continue;
        }

        cJSON *const propertyObject = cJSON_GetObjectItem(configObject, property->getLongKey().c_str());
        if (propertyObject) {
            atLeastOneValueChanged |= property->fromJson(propertyObject);
        }
    }

    return atLeastOneValueChanged;
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

    ESP_LOGW(LOG_TAG_CONFIG, "Config key \"%s\" is too long", key.c_str());
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
        ESP_LOGI(LOG_TAG_CONFIG, "Restart counter updated to %d", newCount);
        m_restartCounter = newCount;
    }
}

}
