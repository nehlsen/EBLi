#include "Mqttp.h"
#include <MqttSubscriber.h>
#include <MqttPublisher.h>
#include <ebli_log.h>
#include <ebli_events.h>
#if defined(CONFIG_EBLI_CONFIG_MANAGER_ENABLE)
#include <ConfigManager.h>
#include <ConfigPropertyConstraint.h>
#endif

namespace EBLi {

static void on_got_ip(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    auto mqtt = static_cast<Mqttp*>(event_handler_arg);
    ESP_ERROR_CHECK(nullptr == mqtt ? ESP_FAIL : ESP_OK);

    mqtt->initializeAndStartClient();
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
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, on_got_ip, this);
}

bool Mqttp::isConnected() const
{
    return m_isConnected;
}

bool Mqttp::isInitializedAndLog() const
{
    if (m_client == nullptr) {
        ESP_LOGE(LOG_TAG_MQTT, "Not initialized!");
        return false;
    }

    return true;
}

MqttSubscriber *Mqttp::createSubscriber(std::string topic, MqttSubscriber::SubscriptionCallbackType cb, MqttSubscriber::SubscriptionScope subscriptionScope)
{
    ESP_LOGD(LOG_TAG_MQTT, "Creating subscriber for \"%s\"", topic.c_str());

    auto subscriber = new MqttSubscriber(topic, cb, subscriptionScope); // FIXME delete subscribers in DESTRUCTOR
    m_subscribers.push_back(subscriber);

    if (isConnected()) {
        setupOneSubscription(subscriber);
    }

    return subscriber;
}

MqttPublisher *Mqttp::createPublisher(std::string topic)
{
    ESP_LOGD(LOG_TAG_MQTT, "Creating publisher for \"%s\"", topic.c_str());

    return new MqttPublisher(this, topic);
}

std::string Mqttp::getBroker() const
{
    std::string broker;

#if defined(CONFIG_EBLI_CONFIG_MANAGER_ENABLE)
    broker = Config::broker()->getValue<std::string>();
#else
    broker = CONFIG_EBLI_MQTT_DEFAULT_BROKER;
#endif

    const std::string mqttPrefix("mqtt://");
    if (broker.substr(0, mqttPrefix.length()) != mqttPrefix) {
        broker = mqttPrefix + broker;
    }

    return broker;
}

void Mqttp::setBroker(const std::string &brokerUrl)
{
#if defined(CONFIG_EBLI_CONFIG_MANAGER_ENABLE)
    Config::broker()->setValue(brokerUrl);
#else
    ESP_LOGE(LOG_TAG_MQTT, "ERROR: Setting the MQTT Broker has no effect when config manager is not enabled!");
#endif
}

std::string Mqttp::getDeviceTopic() const
{
#if defined(CONFIG_EBLI_CONFIG_MANAGER_ENABLE)
    return Config::deviceTopic()->getValue<std::string>();
#endif
    return CONFIG_EBLI_MQTT_DEFAULT_DEVICE_TOPIC;
}

void Mqttp::setDeviceTopic(const std::string &topic)
{
#if defined(CONFIG_EBLI_CONFIG_MANAGER_ENABLE)
    Config::groupTopic()->setValue(topic);
#else
    ESP_LOGE(LOG_TAG_MQTT, "ERROR: Setting the MQTT Device Topic has no effect when config manager is not enabled!");
#endif
}

std::string Mqttp::getGroupTopic() const
{
#if defined(CONFIG_EBLI_CONFIG_MANAGER_ENABLE)
    return Config::groupTopic()->getValue<std::string>();
#endif
    return CONFIG_EBLI_MQTT_DEFAULT_GROUP_TOPIC;
}

void Mqttp::setGroupTopic(const std::string &topic)
{
#if defined(CONFIG_EBLI_CONFIG_MANAGER_ENABLE)
    Config::deviceTopic()->setValue(topic);
#else
    ESP_LOGE(LOG_TAG_MQTT, "ERROR: Setting the MQTT Group Topic has no effect when config manager is not enabled!");
#endif
}

void Mqttp::publish(MqttPublisher *publisher, const std::string &value)
{
    if (!isConnected()) {
        ESP_LOGW(LOG_TAG_MQTT, "Not Connected! Not publishing from publisher \"%s\"", publisher->getTopic().c_str());
        return;
    }

    std::string topic = getDeviceTopic() + "/state/" + publisher->getTopic();
    ESP_LOGD(LOG_TAG_MQTT, "Publishing to \"%s\"", topic.c_str());

    esp_mqtt_client_publish(m_client, topic.c_str(), value.c_str(), 0, 0, 0);
}

void Mqttp::initializeAndStartClient()
{
    if (m_client != nullptr) {
        ESP_LOGW(LOG_TAG_MQTT, "initializeAndStartClient, called when already initialized!");
        return;
    }
    ESP_LOGD(LOG_TAG_MQTT, "initializeAndStartClient");

#if defined(CONFIG_EBLI_CONFIG_MANAGER_ENABLE)
    Config::init();
#endif

    const std::string broker = getBroker();
    esp_mqtt_client_config_t mqtt_cfg = {
            .uri = broker.c_str(),
    };
    ESP_LOGI(LOG_TAG_MQTT, "Using Broker \"%s\"", broker.c_str());

    m_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(m_client, MQTT_EVENT_CONNECTED, mqtt_event_connected, this);
    esp_mqtt_client_register_event(m_client, MQTT_EVENT_DISCONNECTED, mqtt_event_disconnected, this);
    esp_mqtt_client_register_event(m_client, MQTT_EVENT_DATA, mqtt_event_data, this);

    startMqttClient();
}

void Mqttp::startMqttClient()
{
    if (m_client == nullptr) {
        ESP_LOGE(LOG_TAG_MQTT, "can not start to connect when not initialized!");
        return;
    }

    esp_err_t err = esp_mqtt_client_start(m_client);
    if (err != ESP_OK) {
        ESP_LOGW(LOG_TAG_MQTT, "MQTT Start Error: \"%s\"", esp_err_to_name(err));
        return;
    }
    ESP_LOGI(LOG_TAG_MQTT, "MQTT Started.");
}

void Mqttp::onMqttConnected()
{
    m_isConnected = true;
    ESP_LOGI(LOG_TAG_MQTT, "MQTT Connection established...");
    esp_event_post(EBLI_EVENTS, EBLI_EVENT_MQTT_CONNECTED, nullptr, 0, EBLI_EVENTS_MAX_TICKS_TO_POST);

    setupAllSubscriptions();
}

void Mqttp::onMqttDisconnected()
{
    m_isConnected = false;
    ESP_LOGI(LOG_TAG_MQTT, "MQTT Connection lost...");
    esp_event_post(EBLI_EVENTS, EBLI_EVENT_MQTT_DISCONNECTED, nullptr, 0, EBLI_EVENTS_MAX_TICKS_TO_POST);
}

void Mqttp::onMqttData(esp_mqtt_event_handle_t event)
{
    std::string topic(event->topic, event->topic_len);
    ESP_LOGD(LOG_TAG_MQTT, "onMqttData, topic: '%s'", topic.c_str());

    for (auto subscriber : m_subscribers) {
        if (subscriber->isMatchForTopic(topic)) {
            // TODO maybe forward the matched topic (for the subscriber to know whether it was group or device topic)
            subscriber->handleEventData(event->data, event->data_len);
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
    if (!isConnected()) {
        ESP_LOGE(LOG_TAG_MQTT, "Not Connected! Can not subscribe for \"%s\"", subscriber->getTopic().c_str());
        return;
    }

    std::string deviceTopic = getDeviceTopic() + "/cmd/" + subscriber->getTopic();
    if (esp_mqtt_client_subscribe(m_client, deviceTopic.c_str(), 0) < 0) {
        ESP_LOGE(LOG_TAG_MQTT, "Failed to subscribe to \"%s\"", deviceTopic.c_str());
        subscriber->setDeviceTopic("");
    } else {
        ESP_LOGI(LOG_TAG_MQTT, "Subscribed to \"%s\"", deviceTopic.c_str());
        subscriber->setDeviceTopic(deviceTopic);
    }

    if (subscriber->getSubscriptionScope() & MqttSubscriber::ScopeGroup) {
        std::string groupTopic = getGroupTopic() + "/cmd/" + subscriber->getTopic();
        if (esp_mqtt_client_subscribe(m_client, groupTopic.c_str(), 0) < 0) {
            ESP_LOGE(LOG_TAG_MQTT, "Failed to subscribe to \"%s\"", groupTopic.c_str());
            subscriber->setGroupTopic("");
        } else {
            ESP_LOGI(LOG_TAG_MQTT, "Subscribed to \"%s\"", groupTopic.c_str());
            subscriber->setGroupTopic(groupTopic);
        }
    }
}

#if defined(CONFIG_EBLI_CONFIG_MANAGER_ENABLE)
void Mqttp::Config::init()
{
    broker()->setDefaultValue(CONFIG_EBLI_MQTT_DEFAULT_BROKER);
    deviceTopic()->setDefaultValue(CONFIG_EBLI_MQTT_DEFAULT_DEVICE_TOPIC);
    groupTopic()->setDefaultValue(CONFIG_EBLI_MQTT_DEFAULT_GROUP_TOPIC);
}

ConfigProperty *Mqttp::Config::broker()
{
    return EBLi::ConfigManager::instance()
        ->property("mqtt_broker", "MqttBroker")
        ->setConstraint(EBLi::ConfigPropertyConstraint::StringLength(64));
}

ConfigProperty *Mqttp::Config::deviceTopic()
{
    return EBLi::ConfigManager::instance()->property("mqtt_tpc_device", "MqttDeviceTopic");
}

ConfigProperty *Mqttp::Config::groupTopic()
{
    return EBLi::ConfigManager::instance()->property("mqtt_tpc_group", "MqttGroupTopic");
}
#endif

}
