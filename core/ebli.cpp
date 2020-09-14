#include "ebli.h"
#include <sdkconfig.h>

#ifdef CONFIG_ENABLE_EBLI_FS
#include <FS.h>
#endif

#ifdef CONFIG_ENABLE_EBLI_WIFI
#include <Wifi.h>
#endif

namespace EBLi {

void init_all()
{
#ifdef CONFIG_ENABLE_EBLI_FS
    ESP_ERROR_CHECK(EBLi::FS::init());
#endif

#ifdef CONFIG_ENABLE_EBLI_WIFI
    ESP_ERROR_CHECK(EBLi::Wifi::init());
#endif
}

}
