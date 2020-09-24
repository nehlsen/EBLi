#ifndef EBLI_CONFIGPROPERTY_H
#define EBLI_CONFIGPROPERTY_H

#include <string>
#include <functional>

namespace EBLi {

class ConfigPropertyConstraint;

class ConfigProperty
{
friend class ConfigManager;

public:
    std::string getShortKey() const;
    std::string getLongKey() const;

//    int getDefaultValueInt() const;
    ConfigProperty *setDefaultValue(const int &defaultValue);
//    int getDefaultValueString() const;
    ConfigProperty *setDefaultValue(const std::string &defaultValue);
    template<typename T> T getDefaultValue() const;

//    int getValueInt() const;
    void setValue(const int &value);
//    std::string getValueString() const;
    void setValue(const std::string &value);
    template<typename T> T getValue() const;

    ConfigProperty *setConstraint(ConfigPropertyConstraint *constraint);

    // new value? bool: change handler may deny new value?
//    typedef std::function<bool(ConfigProperty *property)> ChangeHandlerCallback;
    typedef std::function<void(ConfigProperty *property)> ChangeHandlerCallback;
    ConfigProperty *setChangeHandler(ChangeHandlerCallback cb);

private:
    explicit ConfigProperty(const std::string &shortKey);
    ConfigProperty(std::string shortKey, std::string longKey);

    const std::string m_shortKey;
    const std::string m_longKey;

    int m_defaultValueInt;
    int m_valueInt;
    std::string m_defaultValueString;
    std::string m_valueString;

    ConfigPropertyConstraint *m_constraint;
    ChangeHandlerCallback m_changeHandler;
};

}

#endif //EBLI_CONFIGPROPERTY_H
