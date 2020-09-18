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

    MqttSubscriber *createSubscriber(std::string topic, MqttSubscriber::SubscriptionCallbackType cb);
    MqttPublisher *createPublisher(std::string topic);

private:
    Mqtt();
    static Mqtt *s_instance;

    Mqttp *m_mqtt = nullptr;
};

}

#endif //EBLI_MQTT_H
