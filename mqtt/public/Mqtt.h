#ifndef EBLI_MQTT_H
#define EBLI_MQTT_H

#include <sdkconfig.h>
#include <string>
#include "MqttSubscriber.h"

namespace EBLi
{

class MqttPublisher;
class Mqttp;

class Mqtt
{
public:
    static Mqtt *init();
    static Mqtt *instance();

    bool isConnected() const;

    MqttSubscriber *createSubscriber(std::string topic, MqttSubscriber::SubscriptionCallbackType cb, MqttSubscriber::SubscriptionScope subscriptionScope = MqttSubscriber::ScopeDeviceAndGroup);
    MqttPublisher *createPublisher(std::string topic);

    std::string getBroker() const;
    void setBroker(const std::string& brokerUrl);
    std::string getDeviceTopic() const;
    void setDeviceTopic(const std::string& topic);
    std::string getGroupTopic() const;
    void setGroupTopic(const std::string& topic);

private:
    Mqtt();
    static Mqtt *s_instance;

    Mqttp *m_mqtt = nullptr;
};

}

#endif //EBLI_MQTT_H
