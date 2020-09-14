#ifndef EBLI_WIFI_H
#define EBLI_WIFI_H

#include <esp_err.h>

namespace EBLi
{

class Wifi
{
public:
    static esp_err_t init();
};

}

#endif //EBLI_WIFI_H
