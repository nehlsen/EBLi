#ifndef EBLI_CONFIGPROPERTY_H
#define EBLI_CONFIGPROPERTY_H

#include <string>
#include <functional>

class cJSON;

namespace EBLi {
namespace Config { class MqttBridge; }
class ConfigManager;
class ConfigPropertyConstraint;

class ConfigProperty
{
friend class ConfigManager;
friend class Config::MqttBridge;

public:
    std::string getShortKey() const;
    std::string getLongKey() const;

    // FIXME rename to fallback
    ConfigProperty *setDefaultValue(const int &defaultValue);
    ConfigProperty *setDefaultValue(const std::string &defaultValue);
    template<typename T> T getDefaultValue() const;

    void setValue(const int &value);
    void setValue(const std::string &value);
    template<typename T> T getValue() const;

    ConfigProperty *setConstraint(ConfigPropertyConstraint *constraint);

    // new value? bool: change handler may deny new value?
//    typedef std::function<bool(ConfigProperty *property)> ChangeHandlerCallback;
    typedef std::function<void(ConfigProperty *property)> ChangeHandlerCallback;
    [[deprecated("use events instead")]]
    ConfigProperty *setChangeHandler(ChangeHandlerCallback cb);

private:
    ConfigProperty(std::string shortKey, std::string longKey);

    const std::string m_shortKey;
    const std::string m_longKey;

    enum {
        TYPE_UNKNOWN,
        TYPE_INT,
        TYPE_STRING,
    } m_type = TYPE_UNKNOWN;
    int m_defaultValueInt = 0;
    std::string m_defaultValueString;

    ConfigPropertyConstraint *m_constraint = nullptr;
    ChangeHandlerCallback m_changeHandler;

    void toJson(cJSON *configObject) const;
    bool fromJson(cJSON *const propertyObject);
};

}

#endif //EBLI_CONFIGPROPERTY_H
