#include "ebli.h"
#include <sdkconfig.h>
#include <ebli_log.h>

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
#include <MqttBridge.h>
#endif
#endif

#if defined(CONFIG_ENABLE_EBLI_TIME)
#include <Time.h>
#endif

#include <esp_event_base.h>
ESP_EVENT_DEFINE_BASE(EBLI_EVENTS);

namespace EBLi {

void init_all()
{
#if defined(CONFIG_EBLI_DEBUG)
    esp_log_level_set(LOG_TAG_CONFIG, ESP_LOG_VERBOSE);
    esp_log_level_set(LOG_TAG_MQTT, ESP_LOG_VERBOSE);
    esp_log_level_set(LOG_TAG_SENSORS, ESP_LOG_VERBOSE);
#endif

#if defined(CONFIG_ENABLE_EBLI_FS)
    ESP_ERROR_CHECK(EBLi::FS::init());
#endif

#if defined(CONFIG_ENABLE_EBLI_WIFI)
    ESP_ERROR_CHECK(EBLi::Wifi::init());
#endif

#if defined(CONFIG_ENABLE_EBLI_CONFIG_MANAGER)
#if defined(CONFIG_ENABLE_EBLI_MQTT)
    EBLi::Config::MqttBridge::setup();
#endif
    EBLi::ConfigManager::init();
#endif

#if defined(CONFIG_ENABLE_EBLI_MQTT)
    EBLi::Mqtt::init();
#endif

#if defined(CONFIG_ENABLE_EBLI_TIME)
    EBLi::Time::init();
#endif
}

}
