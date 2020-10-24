#ifndef EBLI_LOG_TAGS_H
#define EBLI_LOG_TAGS_H

#if defined(CONFIG_EBLI_DEBUG)
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#endif
#include <esp_log.h>

#define LOG_TAG_CONFIG          "EBLi:Config"
#define LOG_TAG_CONFIG_PROPERTY "EBLi:Config:Property"
#define LOG_TAG_CONFIG_MQTT     "EBLi:Config:Mqtt"
#define LOG_TAG_MQTT            "EBLi:Mqtt"
#define LOG_TAG_MQTT_SUBSCRIBER "EBLi:Mqtt:Subscriber"
#define LOG_TAG_SENSORS         "EBLi:Sensors"
#define LOG_TAG_TIME            "EBLi:Time"
#define LOG_TAG_WIFI            "EBLi:Wifi"
// TODO add remaining sub-components

#endif // EBLI_LOG_TAGS_H
