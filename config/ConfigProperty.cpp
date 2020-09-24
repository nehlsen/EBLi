#include "ConfigProperty.h"
#include <cassert>
#include <utility>

namespace EBLi
{

ConfigProperty::ConfigProperty(const std::string &shortKey):
    ConfigProperty(shortKey, shortKey)
{}

ConfigProperty::ConfigProperty(std::string shortKey, std::string longKey):
    m_shortKey(std::move(shortKey)), m_longKey(std::move(longKey))
{
    assert(m_shortKey.length() > 0);
    assert(m_shortKey.length() < 16);
    assert(m_longKey.length() > 0);
}

std::string ConfigProperty::getShortKey() const
{
    return m_shortKey;
}

std::string ConfigProperty::getLongKey() const
{
    return m_longKey;
}

ConfigProperty * ConfigProperty::setDefaultValue(const int &defaultValue)
{
    m_defaultValueInt = defaultValue;
    return this;
}

ConfigProperty * ConfigProperty::setDefaultValue(const std::string &defaultValue)
{
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
    m_valueInt = value;
}

void ConfigProperty::setValue(const std::string &value)
{
    m_valueString = value;
}

template<>
int ConfigProperty::getValue() const
{
    return m_valueInt;
}

template<>
std::string ConfigProperty::getValue() const
{
    return m_valueString;
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

}
