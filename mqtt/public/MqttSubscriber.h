#ifndef EBLI_MQTT_SUBSCRIBER_H
#define EBLI_MQTT_SUBSCRIBER_H

#include <string>
#include <functional>

namespace EBLi
{

class MqttSubscriber
{
friend class Mqttp;

public:
    std::string getTopic() const;

    typedef std::function<void(const std::string &value)> SubscriptionCallbackType;

    enum SubscriptionScope {
        ScopeDevice = 0x1,
        ScopeGroup = 0x2,
        ScopeDeviceAndGroup = ScopeDevice | ScopeGroup,
    };
    SubscriptionScope getSubscriptionScope() const;

protected:
    MqttSubscriber(std::string topic, SubscriptionCallbackType cb, SubscriptionScope subscriptionScope);

private:
    const std::string m_topic;
    SubscriptionCallbackType m_subscriptionCallback;
    SubscriptionScope m_subscriptionScope;

    void setDeviceTopic(const std::string &deviceTopic);
    std::string getDeviceTopic() const;
    std::string m_deviceTopic;

    void setGroupTopic(const std::string &groupTopic);
    std::string getGroupTopic() const;
    std::string m_groupTopic;

    bool isMatchForTopic(const std::string &topic) const;
    void handleEventData(char *data, int data_len);
};

}

#endif //EBLI_MQTT_SUBSCRIBER_H
