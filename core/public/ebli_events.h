#ifndef EBLI_EVENTS_H
#define EBLI_EVENTS_H

#include "esp_event.h"

ESP_EVENT_DECLARE_BASE(EBLI_EVENTS);

enum {
    EBLI_EVENT_CONFIG_PROPERTY_CHANGED,    // arg: (ConfigProperty*) configProperty
    EBLI_EVENT_CONFIG_PROPERTY_REGISTERED, // arg: (ConfigProperty*) configProperty
};

#endif //EBLI_EVENTS_H
