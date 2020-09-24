#ifndef EBLI_CONFIGPROPERTYCONSTRAINT_H
#define EBLI_CONFIGPROPERTYCONSTRAINT_H

namespace EBLi
{

class ConfigPropertyConstraint
{
public:
    template<typename T>
    T constrain(const T &value) const;
};

}

#endif //EBLI_CONFIGPROPERTYCONSTRAINT_H
