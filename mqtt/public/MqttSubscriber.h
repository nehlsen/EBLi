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

protected:
    MqttSubscriber(std::string topic, SubscriptionCallbackType cb);

private:
    const std::string m_topic;
    SubscriptionCallbackType m_subscriptionCallback;

    void setDeviceTopic(const std::string &deviceTopic);
    std::string getDeviceTopic() const;
    std::string m_deviceTopic;

    void setGroupTopic(const std::string &groupTopic);
    std::string getGroupTopic() const;
    std::string m_groupTopic;

    bool matchesTopic(char *topic) const;
    void handleEventData(char *data, int data_len);
};

}

#endif //EBLI_MQTT_SUBSCRIBER_H
