#include "Mqttp.h"
#include <esp_log.h>
#include <MqttSubscriber.h>
#include <MqttPublisher.h>

namespace EBLi
{

static const char *LOG_TAG = "EBLi:Mqtt";

static void on_got_ip(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    auto mqtt = static_cast<Mqttp*>(event_handler_arg);
    ESP_ERROR_CHECK(nullptr == mqtt ? ESP_FAIL : ESP_OK);

    mqtt->startMqttClient();
}

static void mqtt_event_connected(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    auto mqtt = static_cast<Mqttp*>(event_handler_arg);
    ESP_ERROR_CHECK(nullptr == mqtt ? ESP_FAIL : ESP_OK);

    mqtt->onMqttConnected();
}

static void mqtt_event_disconnected(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    auto mqtt = static_cast<Mqttp*>(event_handler_arg);
    ESP_ERROR_CHECK(nullptr == mqtt ? ESP_FAIL : ESP_OK);

    mqtt->onMqttDisconnected();
}

static void mqtt_event_data(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    auto mqtt = static_cast<Mqttp*>(event_handler_arg);
    ESP_ERROR_CHECK(nullptr == mqtt ? ESP_FAIL : ESP_OK);

    mqtt->onMqttData((esp_mqtt_event_handle_t)event_data);
}

Mqttp::Mqttp()
{
    const std::string broker = getMqttBroker();
    esp_mqtt_client_config_t mqtt_cfg = {
            .uri = broker.c_str(),
    };

    m_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, on_got_ip, this);
    esp_mqtt_client_register_event(m_client, MQTT_EVENT_CONNECTED, mqtt_event_connected, this);
    esp_mqtt_client_register_event(m_client, MQTT_EVENT_DISCONNECTED, mqtt_event_disconnected, this);
    esp_mqtt_client_register_event(m_client, MQTT_EVENT_DATA, mqtt_event_data, this);
}

bool Mqttp::isConnected() const
{
    return m_isConnected;
}

MqttSubscriber *Mqttp::createSubscriber(std::string topic, MqttSubscriber::SubscriptionCallbackType cb)
{
    auto subscriber = new MqttSubscriber(topic, cb); // FIXME delete subscribers in DESTRUCTOR
    m_subscribers.push_back(subscriber);

    if (isConnected()) {
        setupOneSubscription(subscriber);
    }

    return subscriber;
}

MqttPublisher *Mqttp::createPublisher(std::string topic)
{
    return new MqttPublisher(this, topic);
}

void Mqttp::publish(MqttPublisher *publisher, const std::string &value)
{
    if (!isConnected()) {
        return;
    }

    std::string topic = getDeviceTopic() + "/state/" + publisher->getTopic();

    esp_mqtt_client_publish(m_client, topic.c_str(), value.c_str(), 0, 0, 0);
}

void Mqttp::startMqttClient()
{
    esp_err_t err = esp_mqtt_client_start(m_client);
    if (err != ESP_OK) {
        ESP_LOGW(LOG_TAG, "MQTT Start Error: \"%s\"", esp_err_to_name(err));
        return;
    }
    ESP_LOGI(LOG_TAG, "MQTT Started.");
}

void Mqttp::onMqttConnected()
{
    setupAllSubscriptions();

    m_isConnected = true;
    ESP_LOGI(LOG_TAG, "MQTT Connection established...");
}

void Mqttp::onMqttDisconnected()
{
    m_isConnected = false;
    ESP_LOGI(LOG_TAG, "MQTT Connection lost...");
}

void Mqttp::onMqttData(esp_mqtt_event_handle_t event)
{
    std::string topic(event->topic, event->topic_len);
    ESP_LOGD(LOG_TAG, "onMqttData, topic: '%s'", topic.c_str());

    for (auto subscriber : m_subscribers) {
        ESP_LOGV(LOG_TAG, "onMqttData, subscriber: '%s' & '%s'",
                 subscriber->getDeviceTopic().c_str(),
                 subscriber->getGroupTopic().c_str()
                 );

        if (subscriber->matchesTopic(topic)) {
            ESP_LOGV(LOG_TAG, "onMqttData, MATCH");
            // TODO maybe forward the matched topic (for the subscriber to know whether it was group or device topic)
            subscriber->handleEventData(event->data, event->data_len);
        } else {
            ESP_LOGV(LOG_TAG, "onMqttData, NO MATCH");
        }
    }
}

void Mqttp::setupAllSubscriptions()
{
    for (auto subscriber : m_subscribers) {
        setupOneSubscription(subscriber);
    }
}

void Mqttp::setupOneSubscription(MqttSubscriber *subscriber)
{
    std::string deviceTopic = getDeviceTopic() + "/cmd/" + subscriber->getTopic();
    if (esp_mqtt_client_subscribe(m_client, deviceTopic.c_str(), 0) < 0) {
        ESP_LOGE(LOG_TAG, "Failed to subscribe to \"%s\"", deviceTopic.c_str());
        subscriber->setDeviceTopic("");
    } else {
        ESP_LOGI(LOG_TAG, "Subscribed to \"%s\"", deviceTopic.c_str());
        subscriber->setDeviceTopic(deviceTopic);
    }

    std::string groupTopic = getGroupTopic() + "/cmd/" + subscriber->getTopic();
    if (esp_mqtt_client_subscribe(m_client, groupTopic.c_str(), 0) < 0) {
        ESP_LOGE(LOG_TAG, "Failed to subscribe to \"%s\"", groupTopic.c_str());
        subscriber->setGroupTopic("");
    } else {
        ESP_LOGI(LOG_TAG, "Subscribed to \"%s\"", groupTopic.c_str());
        subscriber->setGroupTopic(groupTopic);
    }
}

std::string Mqttp::getMqttBroker() const
{
    std::string broker;

#if defined(CONFIG_ENABLE_EBLI_CONFIG_MANAGER)
    broker = configManager->get("mqtt/broker", CONFIG_EBLI_MQTT_DEFAULT_BROKER);
#else
    broker = CONFIG_EBLI_MQTT_DEFAULT_BROKER;
#endif

    const std::string mqttPrefix("mqtt://");
    if (broker.substr(0, mqttPrefix.length()) != mqttPrefix) {
        broker = mqttPrefix + broker;
    }

    return broker;
}

std::string Mqttp::getDeviceTopic() const
{
#if defined(CONFIG_ENABLE_EBLI_CONFIG_MANAGER)
    return configManager->get("mqtt/device_topic", CONFIG_EBLI_MQTT_DEFAULT_DEVICE_TOPIC);
#endif
    return CONFIG_EBLI_MQTT_DEFAULT_DEVICE_TOPIC;
}

std::string Mqttp::getGroupTopic() const
{
#if defined(CONFIG_ENABLE_EBLI_CONFIG_MANAGER)
    return configManager->get("mqtt/group_topic", CONFIG_EBLI_MQTT_DEFAULT_GROUP_TOPIC);
#endif
    return CONFIG_EBLI_MQTT_DEFAULT_GROUP_TOPIC;
}

}
