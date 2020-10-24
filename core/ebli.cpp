#include "ebli.h"
#include <sdkconfig.h>
#include <ebli_log.h>

#if defined(CONFIG_EBLI_STATUS_ENABLE)
#include <Status.h>
#endif

#if defined(CONFIG_EBLI_FS_ENABLE)
#include <FS.h>
#endif

#if defined(CONFIG_EBLI_WIFI_ENABLE)
#include <Wifi.h>
#endif

#if defined(CONFIG_EBLI_CONFIG_MANAGER_ENABLE)
#include <ConfigManager.h>
#endif

#if defined(CONFIG_EBLI_WIFI_ENABLE)
#include <Mqtt.h>
#if defined(CONFIG_EBLI_CONFIG_MANAGER_ENABLE)
#include <MqttBridge.h>
#endif
#endif

#if defined(CONFIG_EBLI_TIME_ENABLE)
#include <Time.h>
#endif

#if defined(CONFIG_EBLI_SENSORS_ENABLED)
#include <Sensors.h>
#endif

#include <esp_event_base.h>
#include <esp_event.h>

ESP_EVENT_DEFINE_BASE(EBLI_EVENTS);

namespace EBLi {

void init_all()
{
#if defined(CONFIG_EBLI_DEBUG)
    esp_log_level_set(LOG_TAG_CONFIG, ESP_LOG_VERBOSE);
    esp_log_level_set(LOG_TAG_CONFIG_PROPERTY, ESP_LOG_VERBOSE);
    esp_log_level_set(LOG_TAG_CONFIG_MQTT, ESP_LOG_VERBOSE);
    esp_log_level_set(LOG_TAG_MQTT, ESP_LOG_VERBOSE);
    esp_log_level_set(LOG_TAG_MQTT_SUBSCRIBER, ESP_LOG_VERBOSE);
    esp_log_level_set(LOG_TAG_SENSORS, ESP_LOG_VERBOSE);
    esp_log_level_set(LOG_TAG_TIME, ESP_LOG_VERBOSE);
    esp_log_level_set(LOG_TAG_WIFI, ESP_LOG_VERBOSE);
    esp_log_level_set(LOG_TAG_STATUS, ESP_LOG_VERBOSE);
#endif

// TODO ifdef STATUS or WIFI or MQTT or ?
    esp_err_t ret = esp_event_loop_create_default();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE("EBLi:Core", "Failed to start event loop");
    }

#if defined(CONFIG_EBLI_STATUS_ENABLE)
    auto s = new Status::Status();
#endif

#if defined(CONFIG_EBLI_FS_ENABLE)
    ESP_ERROR_CHECK(EBLi::FS::init());
#endif

#if defined(CONFIG_EBLI_WIFI_ENABLE)
    ESP_ERROR_CHECK(EBLi::Wifi::init());
#endif

#if defined(CONFIG_EBLI_CONFIG_MANAGER_ENABLE)
#if defined(CONFIG_EBLI_WIFI_ENABLE)
    EBLi::Config::MqttBridge::setup();
#endif
    EBLi::ConfigManager::init();
#endif

#if defined(CONFIG_EBLI_WIFI_ENABLE)
    EBLi::Mqtt::init();
#endif

#if defined(CONFIG_EBLI_TIME_ENABLE)
    EBLi::Time::init();
#endif

#if defined(CONFIG_EBLI_SENSORS_ENABLED)
    EBLi::Sensors::Sensors::init();
#endif
}

}
