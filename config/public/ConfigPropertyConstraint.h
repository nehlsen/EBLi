#ifndef EBLI_CONFIGPROPERTYCONSTRAINT_H
#define EBLI_CONFIGPROPERTYCONSTRAINT_H

namespace EBLi::config {

class ConfigPropertyConstraint
{
public:
    static ConfigPropertyConstraint *Number(int minimum, int maximum);
    static ConfigPropertyConstraint *StringLength(int maximum);

    template<typename T>
    T constrain(const T &value) const;

private:
    ConfigPropertyConstraint(int minimum, int maximum);
    const int m_minimum;
    const int m_maximum;
};

}

#endif //EBLI_CONFIGPROPERTYCONSTRAINT_H
