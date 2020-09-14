#ifndef EBLI_MQTTP_H
#define EBLI_MQTTP_H

#include <string>
#include <vector>
#include "mqtt_client.h"

namespace EBLi
{

class MqttSubscriber;
class MqttPublisher;

class Mqttp
{
public:
    Mqttp();

    bool isConnected() const;

    void addSubscriber(MqttSubscriber *subscriber);
    MqttPublisher *createPublisher(std::string topic);
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

    std::string getMqttBroker() const;
    std::string getDeviceTopic() const;
    std::string getGroupTopic() const;
};

}

#endif //EBLI_MQTTP_H
