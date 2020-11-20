#ifndef EBLI_EVENTS_H
#define EBLI_EVENTS_H

#include "esp_event.h"

ESP_EVENT_DECLARE_BASE(EBLI_EVENTS);

enum {
    EBLI_EVENT_CONFIG_PROPERTY_CHANGED,    // arg: (ConfigProperty**) configProperty
    EBLI_EVENT_CONFIG_PROPERTY_REGISTERED, // arg: (ConfigProperty**) configProperty
    EBLI_EVENT_WIFI_CONNECTING,
    EBLI_EVENT_WIFI_PROVISIONING,
    EBLI_EVENT_WIFI_CONNECTED,
    EBLI_EVENT_WIFI_DISCONNECTED,
    EBLI_EVENT_MQTT_CONNECTED,
    EBLI_EVENT_MQTT_DISCONNECTED,
    EBLI_EVENT_OTA_STARTED,  // arg: char*, URL of the update
    EBLI_EVENT_OTA_PROGRESS, // arg: char*, some info
    EBLI_EVENT_OTA_FAILED,   // arg: char*, error message
    EBLI_EVENT_OTA_SUCCEED,  // arg: char*, some info
};

// maximum ticks to wait to post an event to the event queue
#define EBLI_EVENTS_MAX_TICKS_TO_POST 100

#endif //EBLI_EVENTS_H
