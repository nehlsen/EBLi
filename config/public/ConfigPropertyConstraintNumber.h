#ifndef LEDWALL_CONFIGPROPERTYCONSTRAINTNUMBER_H
#define LEDWALL_CONFIGPROPERTYCONSTRAINTNUMBER_H

#include "ConfigPropertyConstraint.h"

namespace EBLi
{

class ConfigPropertyConstraintNumber : public ConfigPropertyConstraint
{
public:
    ConfigPropertyConstraintNumber(int minimum, int maximum);
};

}

#endif //LEDWALL_CONFIGPROPERTYCONSTRAINTNUMBER_H
