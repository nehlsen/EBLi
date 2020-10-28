#ifndef EBLI_CONFIGPROPERTY_H
#define EBLI_CONFIGPROPERTY_H

#include <string>
#include <functional>

#define EventDataToConfigPropertyHandle(event_data) *(static_cast<ConfigProperty**>(event_data))

class cJSON;

namespace EBLi {
namespace Config { class MqttBridge; }
class ConfigManager;
class ConfigPropertyConstraint;

// FIXME value and default value could maybe be stored in a std::variant<> ?!
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
    ConfigProperty *setChangeHandler(ChangeHandlerCallback cb);

    // properties are always visible via the config manager and the property itself
    // the visibility flag has an effect on MQTT and REST only
    enum Visibility {
        Hidden,
        Device,
        Group,
    };
    Visibility getVisibility() const;
    bool isVisibilityHidden() const;
    bool isVisibilityDevice() const;
    bool isVisibilityGroup() const;
    ConfigProperty *setVisibility(Visibility visibility);

    // properties are always writable via the config manager and the property itself
    // the accessibility flag has an effect on MQTT and REST only
    enum Accessibility {
        ReadOnly,
        ReadWrite,
    };
    Accessibility getAccessibility() const;
    bool isAccessibilityReadOnly() const;
    ConfigProperty *setAccessibility(Accessibility accessibility);

private:
    ConfigProperty(std::string shortKey, std::string longKey);

    const std::string m_shortKey;
    const std::string m_longKey;

    enum {
        TYPE_UNKNOWN = 0,
        TYPE_INT = 1,
        TYPE_STRING = 2,
    } m_type = TYPE_UNKNOWN;
    int m_defaultValueInt = 0;
    std::string m_defaultValueString;

    ConfigPropertyConstraint *m_constraint = nullptr;
    ChangeHandlerCallback m_changeHandler;

    Visibility m_visibility = Group;
    Accessibility m_accessibility = ReadWrite;

    void emitChangeEvent();

    void toJson(cJSON *configObject) const;
    bool fromJson(cJSON *const propertyObject);
};

}

#endif //EBLI_CONFIGPROPERTY_H
