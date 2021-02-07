#include "ConfigPropertyConstraint.h"
#include <esp_log.h>
#include <string>

namespace EBLi::config {

ConfigPropertyConstraint::ConfigPropertyConstraint(int minimum, int maximum):
    m_minimum(minimum), m_maximum(maximum)
{}

ConfigPropertyConstraint *ConfigPropertyConstraint::Number(int minimum, int maximum)
{
    if (minimum > maximum) {
        return new ConfigPropertyConstraint(maximum, minimum);
    }

    return new ConfigPropertyConstraint(minimum, maximum);
}

ConfigPropertyConstraint *ConfigPropertyConstraint::StringLength(int maximum)
{
    return new ConfigPropertyConstraint(0, (maximum < 0 ? 0 : maximum));
}

template<typename T>
T ConfigPropertyConstraint::constrain(const T &value) const
{
    ESP_LOGE("ConfigPropertyConstraint", "Error: Invalid Constraint!");
    return value;
}

template<>
int ConfigPropertyConstraint::constrain(const int &value) const
{
    return value < m_minimum ? m_minimum : (value > m_maximum ? m_maximum : value);
}

template<>
std::string ConfigPropertyConstraint::constrain(const std::string &value) const
{
    if (value.length() > m_maximum) {
        return value.substr(0, m_maximum);
    }

    return value;
}

}
