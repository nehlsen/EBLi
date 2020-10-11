#ifndef EBLI_MQTTP_H
#define EBLI_MQTTP_H

#include <string>
#include <vector>
#include "mqtt_client.h"
#include "MqttSubscriber.h"

namespace EBLi {

class MqttPublisher;
#if defined(CONFIG_ENABLE_EBLI_CONFIG_MANAGER)
class ConfigProperty;
#endif

class Mqttp
{
public:
    Mqttp();

    bool isConnected() const;

    MqttSubscriber *createSubscriber(std::string topic, MqttSubscriber::SubscriptionCallbackType cb);
    MqttPublisher *createPublisher(std::string topic);

    std::string getBroker() const;
    void setBroker(const std::string& brokerUrl);
    std::string getDeviceTopic() const;
    void setDeviceTopic(const std::string& topic);
    std::string getGroupTopic() const;
    void setGroupTopic(const std::string& topic);

    void publish(MqttPublisher *publisher, const std::string &value);

    void initializeAndStartClient();
    void startMqttClient();
    void onMqttConnected();
    void onMqttDisconnected();
    void onMqttData(esp_mqtt_event_handle_t event);

private:
    bool m_isConnected = false;
    bool isInitializedAndLog() const;
    esp_mqtt_client_handle_t m_client = nullptr;

    std::vector<MqttSubscriber*> m_subscribers;
    void setupAllSubscriptions();
    void setupOneSubscription(MqttSubscriber *subscriber);

#if defined(CONFIG_ENABLE_EBLI_CONFIG_MANAGER)
    struct Config {
        static void init();
        static ConfigProperty *broker();
        static ConfigProperty *deviceTopic();
        static ConfigProperty *groupTopic();
    };
#endif
};

}

#endif //EBLI_MQTTP_H
