#ifndef EBLI_TIME_H
#define EBLI_TIME_H

#include <esp_err.h>

namespace EBLi {

class Time
{
public:
    static esp_err_t init();

private:
    static bool m_isInitialized;

#if defined(CONFIG_ENABLE_EBLI_CONFIG_MANAGER)
    static void initConfig();
#endif
};

}

#endif //EBLI_TIME_H
