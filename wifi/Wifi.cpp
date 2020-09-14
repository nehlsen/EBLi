#include "Wifi.h"
#include "Wifip.h"

namespace EBLi
{

esp_err_t Wifi::init()
{
    return Wifip::init();
}

}
