#include "MqttPublisher.h"
#include "Mqttp.h"
#include <utility>

namespace EBLi
{

MqttPublisher::MqttPublisher(Mqttp *mqtt, std::string topic):
    m_mqtt(mqtt), m_topic(std::move(topic))
{}

std::string MqttPublisher::getTopic() const
{
    return m_topic;
}

void MqttPublisher::publishValue(uint8_t value)
{
    publishValue(std::to_string(value));
}

void MqttPublisher::publishValue(std::string value)
{
    m_mqtt->publish(this, value);
}

}
