#include "Mqtt.h"
#include "Mqttp.h"

namespace EBLi::mqtt {

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

MqttSubscriber *Mqtt::createSubscriber(std::string topic, MqttSubscriber::SubscriptionCallbackType cb, MqttSubscriber::SubscriptionScope subscriptionScope)
{
    return m_mqtt->createSubscriber(topic, cb, subscriptionScope);
}

MqttPublisher *Mqtt::createPublisher(std::string topic, MqttPublisher::RetainFlag retainFlag)
{
    return m_mqtt->createPublisher(topic, retainFlag);
}

std::string Mqtt::getBroker() const
{
    return m_mqtt->getBroker();
}

void Mqtt::setBroker(const std::string &brokerUrl)
{
    m_mqtt->setBroker(brokerUrl);
}

std::string Mqtt::getDeviceTopic() const
{
    return m_mqtt->getDeviceTopic();
}

void Mqtt::setDeviceTopic(const std::string &topic)
{
    m_mqtt->setDeviceTopic(topic);
}

std::string Mqtt::getGroupTopic() const
{
    return m_mqtt->getGroupTopic();
}

void Mqtt::setGroupTopic(const std::string &topic)
{
    m_mqtt->setGroupTopic(topic);
}

Mqtt::Mqtt()
{
    m_mqtt = new Mqttp;
}

}
