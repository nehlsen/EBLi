#include "MqttBridge.h"
#include "ebli_events.h"
#include <Mqtt.h>
#include <MqttPublisher.h>
#include "ConfigProperty.h"
#include <ebli_log.h>

namespace EBLi::Config {

#define MQTT_PROPERTY_PREFIX "config/"

static void onConfigPropertyEvent(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    auto configProperty = EventDataToConfigPropertyHandle(event_data);

    ESP_ERROR_CHECK_WITHOUT_ABORT(nullptr == configProperty ? ESP_FAIL : ESP_OK);
    if (nullptr == configProperty) return;

    if (event_id == EBLI_EVENT_CONFIG_PROPERTY_REGISTERED) {
        MqttBridge::handleRegistered(configProperty);
    } else if (event_id == EBLI_EVENT_CONFIG_PROPERTY_CHANGED) {
        MqttBridge::handleChanged(configProperty);
    }
}

void MqttBridge::setup()
{
    esp_event_handler_register(EBLI_EVENTS, EBLI_EVENT_CONFIG_PROPERTY_REGISTERED, &onConfigPropertyEvent, nullptr);
    esp_event_handler_register(EBLI_EVENTS, EBLI_EVENT_CONFIG_PROPERTY_CHANGED, &onConfigPropertyEvent, nullptr);
}

inline std::string createTopic(ConfigProperty *configProperty)
{
    // FIXME should MQTT use long or short key?! or once for each?!
    return MQTT_PROPERTY_PREFIX + configProperty->getLongKey();
}

void MqttBridge::handleRegistered(ConfigProperty *configProperty)
{
    assert(configProperty != nullptr);

    if (configProperty->isVisibilityHidden() || configProperty->isAccessibilityReadOnly()) {
        ESP_LOGD(LOG_TAG_CONFIG_MQTT,
                 "Not subscribing, property '%s' is hidden or read-only",
                 configProperty->getShortKey().c_str()
                 );
        return;
    }

    auto mqtt = Mqtt::instance();
    mqtt->createSubscriber(createTopic(configProperty), [configProperty](const std::string &value) {
        ESP_LOGV(LOG_TAG_CONFIG_MQTT,
                 "Config Property Change-Callback triggered, property: '%s'",
                 configProperty->getShortKey().c_str()
        );

        if (configProperty->m_type == ConfigProperty::TYPE_INT) {
            configProperty->setValue(std::stoi(value));
        } else if (configProperty->m_type == ConfigProperty::TYPE_STRING) {
            configProperty->setValue(value);
        } else {
            ESP_LOGW(LOG_TAG_CONFIG_MQTT,
                     "Can not update value for property '%s' - Type(%d) unknown",
                     configProperty->getShortKey().c_str(), configProperty->m_type
                     );
        }
    }, configProperty->isVisibilityGroup() ? MqttSubscriber::ScopeGroup : MqttSubscriber::ScopeDevice);

    // the subscriber will be left as dangling pointer for the rest of the program runtime
}

void MqttBridge::handleChanged(ConfigProperty *configProperty)
{
    assert(configProperty != nullptr);

    if (configProperty->isVisibilityHidden()) {
        ESP_LOGD(LOG_TAG_CONFIG_MQTT,
                 "Not publishing, property '%s' is hidden",
                 configProperty->getShortKey().c_str()
        );
        return;
    }

    MqttPublisher *publisher = EBLi::Mqtt::instance()
            ->createPublisher(createTopic(configProperty));

    if (configProperty->m_type == ConfigProperty::TYPE_INT) {
        publisher->publishValue(configProperty->getValue<int>());
    } else if (configProperty->m_type == ConfigProperty::TYPE_STRING) {
        publisher->publishValue(configProperty->getValue<std::string>());
    } else {
        ESP_LOGW(LOG_TAG_CONFIG_MQTT, "Can not publish value for property '%s' - Type unknown", configProperty->getShortKey().c_str());
    }

    delete publisher;
    // TODO the publisher should be stored somewhere for re-use
}

}
