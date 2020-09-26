#include "ConfigProperty.h"
#include "ConfigPropertyConstraint.h"
#include "ConfigManager.h"
#include <cassert>
#include <utility>
#include <cJSON.h>
#include <esp_log.h>
#include "ebli_events.h"

namespace EBLi
{

ConfigProperty::ConfigProperty(std::string shortKey, std::string longKey):
    m_shortKey(std::move(shortKey)), m_longKey(std::move(longKey))
{
    assert(m_shortKey.length() > 0);
    assert(m_shortKey.length() < 16);
}

std::string ConfigProperty::getShortKey() const
{
    return m_shortKey;
}

std::string ConfigProperty::getLongKey() const
{
    return m_longKey.length() < 1 ? m_shortKey : m_longKey;
}

ConfigProperty *ConfigProperty::setDefaultValue(const int &defaultValue)
{
    if (m_type != TYPE_UNKNOWN && m_type != TYPE_INT) {
        ESP_LOGW("ConfigProperty", "setDefaultValue: operation changes type hint!");
    }
    m_type = TYPE_INT;

    m_defaultValueInt = defaultValue;
    return this;
}

ConfigProperty *ConfigProperty::setDefaultValue(const std::string &defaultValue)
{
    if (m_type != TYPE_UNKNOWN && m_type != TYPE_STRING) {
        ESP_LOGW("ConfigProperty", "setDefaultValue: operation changes type hint!");
    }
    m_type = TYPE_STRING;

    m_defaultValueString = defaultValue;
    return this;
}

template<>
int ConfigProperty::getDefaultValue() const
{
    return m_defaultValueInt;
}

template<>
std::string ConfigProperty::getDefaultValue() const
{
    return m_defaultValueString;
}

template<typename T>
T ConfigProperty::getDefaultValue() const
{
    return T();
}

void ConfigProperty::setValue(const int &value)
{
    if (m_type != TYPE_UNKNOWN && m_type != TYPE_INT) {
        ESP_LOGW("ConfigProperty", "setValue: operation changes type hint!");
    }
    m_type = TYPE_INT;

    if (nullptr != m_constraint) {
        ConfigManager::instance()->setValue(getShortKey(), m_constraint->constrain(value));
    } else {
        ConfigManager::instance()->setValue(getShortKey(), value);
    }

    if (m_changeHandler) {
        m_changeHandler(this);
    }

    esp_event_post(EBLI_EVENTS, EBLI_EVENT_CONFIG_PROPERTY_CHANGED, (void*)this, sizeof(ConfigProperty), portMAX_DELAY);
}

void ConfigProperty::setValue(const std::string &value)
{
    if (m_type != TYPE_UNKNOWN && m_type != TYPE_STRING) {
        ESP_LOGW("ConfigProperty", "setDefaultValue: operation changes type hint!");
    }
    m_type = TYPE_STRING;

    if (nullptr != m_constraint) {
        ConfigManager::instance()->setValue(getShortKey(), m_constraint->constrain(value));
    } else {
        ConfigManager::instance()->setValue(getShortKey(), value);
    }

    if (m_changeHandler) {
        m_changeHandler(this);
    }

    esp_event_post(EBLI_EVENTS, EBLI_EVENT_CONFIG_PROPERTY_CHANGED, (void*)this, sizeof(ConfigProperty), portMAX_DELAY);
}

template<>
int ConfigProperty::getValue() const
{
    return ConfigManager::instance()->getValue(getShortKey(), getDefaultValue<int>());
}

template<>
std::string ConfigProperty::getValue() const
{
    return ConfigManager::instance()->getValue(getShortKey(), getDefaultValue<std::string>());
}

template<typename T>
T ConfigProperty::getValue() const
{
    return T();
}

ConfigProperty *ConfigProperty::setConstraint(ConfigPropertyConstraint *constraint)
{
    m_constraint = constraint;
    return this;
}

ConfigProperty *ConfigProperty::setChangeHandler(ChangeHandlerCallback cb)
{
    m_changeHandler = cb;
    return this;
}

void ConfigProperty::toJson(cJSON *configObject) const
{
    assert(configObject != nullptr);

    if (m_type == TYPE_STRING) {
        cJSON_AddStringToObject(configObject, getLongKey().c_str(), getValue<std::string>().c_str());
    } else if (m_type == TYPE_INT) {
        cJSON_AddNumberToObject(configObject, getLongKey().c_str(), getValue<int>());
    } else {
        ESP_LOGW("ConfigProperty", "toJson: uncertain type!");
    }
}

bool ConfigProperty::fromJson(cJSON *const propertyObject)
{
    assert(propertyObject != nullptr);

    if (m_type == TYPE_STRING && cJSON_IsString(propertyObject)) {
        setValue(propertyObject->valuestring);
        return true;
    } else if (m_type == TYPE_INT && cJSON_IsNumber(propertyObject)) {
        setValue(propertyObject->valueint);
        return true;
    } else {
        ESP_LOGW("ConfigProperty", "toJson: uncertain type!");
        return false;
    }
}

}
