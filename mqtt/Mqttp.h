#ifndef EBLI_MQTTP_H
#define EBLI_MQTTP_H

#include <string>
#include <vector>
#include "mqtt_client.h"
#include "MqttSubscriber.h"
#include "MqttPublisher.h"

#if defined(CONFIG_EBLI_CONFIG_MANAGER_ENABLE)
namespace EBLi::config {
class ConfigProperty;
}
#endif

namespace EBLi::mqtt {

class Mqttp
{
public:
    Mqttp();

    bool isConnected() const;

    MqttSubscriber *createSubscriber(const std::string& topic, MqttSubscriber::SubscriptionCallbackType cb, MqttSubscriber::SubscriptionScope subscriptionScope);
    MqttPublisher *createPublisher(const std::string& topic, MqttPublisher::RetainFlag retainFlag);

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

#if defined(CONFIG_EBLI_CONFIG_MANAGER_ENABLE)
    struct Config {
        static void init();
        static config::ConfigProperty *broker();
        static config::ConfigProperty *deviceTopic();
        static config::ConfigProperty *groupTopic();
    };
#endif
};

}

#endif //EBLI_MQTTP_H
