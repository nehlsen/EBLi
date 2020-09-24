#ifndef LEDWALL_CONFIGPROPERTYCONSTRAINTSTRING_H
#define LEDWALL_CONFIGPROPERTYCONSTRAINTSTRING_H

#include "ConfigPropertyConstraint.h"

namespace EBLi {

class ConfigPropertyConstraintString : public ConfigPropertyConstraint
{
public:
    ConfigPropertyConstraintString(int minimum, int maximum);
};

}

#endif //LEDWALL_CONFIGPROPERTYCONSTRAINTSTRING_H
