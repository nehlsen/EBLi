#ifndef EBLI_SENSORS_H
#define EBLI_SENSORS_H

#include <esp_err.h>

namespace EBLi::Sensors {

class SensorsP;
class Sensors
{
public:
    static esp_err_t init();

private:
    static SensorsP *p;
};

}

#endif //EBLI_SENSORS_H
