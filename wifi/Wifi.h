#ifndef ONEWHEELMONITOR_WIFI_H
#define ONEWHEELMONITOR_WIFI_H

#include <esp_err.h>

namespace EBLi
{

class Wifi
{
public:
    static esp_err_t init();
};

}

#endif //ONEWHEELMONITOR_WIFI_H
