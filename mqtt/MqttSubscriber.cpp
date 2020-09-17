#include "MqttSubscriber.h"

#include <utility>

namespace EBLi
{

std::string MqttSubscriber::getTopic() const
{
    return m_topic;
}

MqttSubscriber::MqttSubscriber(std::string topic, SubscriptionCallbackType cb):
    m_topic(std::move(topic)), m_subscriptionCallback(cb)
{}

void MqttSubscriber::setDeviceTopic(const std::string &deviceTopic)
{
    m_deviceTopic = deviceTopic;
}

std::string MqttSubscriber::getDeviceTopic() const
{
    return m_deviceTopic;
}

void MqttSubscriber::setGroupTopic(const std::string &groupTopic)
{
    m_groupTopic = groupTopic;
}

std::string MqttSubscriber::getGroupTopic() const
{
    return m_groupTopic;
}

bool MqttSubscriber::matchesTopic(const std::string &topic) const
{
    return m_deviceTopic == topic || m_groupTopic == topic;
}

void MqttSubscriber::handleEventData(char *data, int data_len)
{
    m_subscriptionCallback(std::string(data, data_len));
}

}
