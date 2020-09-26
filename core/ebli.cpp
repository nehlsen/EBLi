#include "ebli.h"
#include <sdkconfig.h>

#if defined(CONFIG_ENABLE_EBLI_FS)
#include <FS.h>
#endif

#if defined(CONFIG_ENABLE_EBLI_WIFI)
#include <Wifi.h>
#endif

#if defined(CONFIG_ENABLE_EBLI_CONFIG_MANAGER)
#include <ConfigManager.h>
#endif

#if defined(CONFIG_ENABLE_EBLI_MQTT)
#include <Mqtt.h>
#if defined(CONFIG_ENABLE_EBLI_CONFIG_MANAGER)
#include "../config/MqttBridge.h"
#endif
#endif

#include <esp_event_base.h>
ESP_EVENT_DEFINE_BASE(EBLI_EVENTS);

namespace EBLi {

void init_all()
{
#if defined(CONFIG_ENABLE_EBLI_FS)
    ESP_ERROR_CHECK(EBLi::FS::init());
#endif

#if defined(CONFIG_ENABLE_EBLI_WIFI)
    ESP_ERROR_CHECK(EBLi::Wifi::init());
#endif

#if defined(CONFIG_ENABLE_EBLI_CONFIG_MANAGER)
    EBLi::ConfigManager::init();
#endif

#if defined(CONFIG_ENABLE_EBLI_MQTT)
    EBLi::Mqtt::init();
#if defined(CONFIG_ENABLE_EBLI_CONFIG_MANAGER)
    EBLi::MqttBridge::setup();
#endif
#endif
}

}
