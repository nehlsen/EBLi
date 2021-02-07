#include "MqttSubscriber.h"
#include <utility>
#include <ebli_log.h>

namespace EBLi::mqtt {

std::string MqttSubscriber::getTopic() const
{
    return m_topic;
}

MqttSubscriber::SubscriptionScope MqttSubscriber::getSubscriptionScope() const
{
    return m_subscriptionScope;
}

MqttSubscriber::MqttSubscriber(std::string topic, SubscriptionCallbackType cb, SubscriptionScope subscriptionScope):
    m_topic(std::move(topic)), m_subscriptionCallback(cb), m_subscriptionScope(subscriptionScope)
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

bool MqttSubscriber::isMatchForTopic(const std::string &topic) const
{
    const bool isMatch = m_deviceTopic == topic || m_groupTopic == topic;

    ESP_LOGD(LOG_TAG_MQTT_SUBSCRIBER,
             "isMatchForTopic %s, topic: '%s', device topic: '%s', group topic: '%s'",
             (isMatch ? "YES" : "NO"),
             topic.c_str(),
             m_deviceTopic.c_str(),
             m_groupTopic.c_str()
             );

    return isMatch;
}

void MqttSubscriber::handleEventData(char *data, int data_len)
{
    m_subscriptionCallback(std::string(data, data_len));
}

}
