#ifndef EBLI_MQTTP_H
#define EBLI_MQTTP_H

#include <string>
#include <vector>
#include "mqtt_client.h"
#include "MqttSubscriber.h"

namespace EBLi
{

class MqttPublisher;

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

    void startMqttClient();
    void onMqttConnected();
    void onMqttDisconnected();
    void onMqttData(esp_mqtt_event_handle_t event);

private:
    bool m_isConnected = false;
    esp_mqtt_client_handle_t m_client;

    std::vector<MqttSubscriber*> m_subscribers;
    void setupAllSubscriptions();
    void setupOneSubscription(MqttSubscriber *subscriber);
};

}

#endif //EBLI_MQTTP_H
