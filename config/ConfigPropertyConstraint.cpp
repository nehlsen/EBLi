#include "ConfigPropertyConstraint.h"
#include <esp_log.h>

namespace EBLi
{

template<typename T>
T ConfigPropertyConstraint::constrain(const T &value) const
{
    ESP_LOGE("ConfigPropertyConstraint", "Error: Invalid Constraint!");
    return value;
}

}
