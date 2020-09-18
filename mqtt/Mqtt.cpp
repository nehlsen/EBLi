#include "Mqtt.h"
#include "Mqttp.h"

namespace EBLi
{

Mqtt *Mqtt::s_instance = nullptr;

Mqtt *Mqtt::init()
{
    return instance();
}

Mqtt *Mqtt::instance()
{
    if (nullptr == s_instance) {
        s_instance = new Mqtt;
    }

    return s_instance;
}

bool Mqtt::isConnected() const
{
    return m_mqtt->isConnected();
}

MqttSubscriber *Mqtt::createSubscriber(std::string topic, MqttSubscriber::SubscriptionCallbackType cb)
{
    return m_mqtt->createSubscriber(topic, cb);
}

MqttPublisher *Mqtt::createPublisher(std::string topic)
{
    return m_mqtt->createPublisher(topic);
}

Mqtt::Mqtt()
{
    m_mqtt = new Mqttp;
}

}
