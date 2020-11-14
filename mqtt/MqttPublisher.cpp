#include "MqttPublisher.h"
#include "Mqttp.h"
#include <utility>
#include <cJSON.h>

namespace EBLi {

MqttPublisher::MqttPublisher(Mqttp *mqtt, std::string topic, RetainFlag retainFlag):
    m_mqtt(mqtt), m_topic(std::move(topic)), m_retainFlag(retainFlag)
{}

std::string MqttPublisher::getTopic() const
{
    return m_topic;
}

void MqttPublisher::publishValue(uint8_t value)
{
    publishValue(std::to_string(value));
}

void MqttPublisher::publishValue(cJSON *jsonObject)
{
    char *jsonBuffer = cJSON_PrintUnformatted(jsonObject);
    publishValue(jsonBuffer);
    free(jsonBuffer);
}

void MqttPublisher::publishValue(const std::string& value)
{
    m_mqtt->publish(this, value);
}

MqttPublisher::QualityOfService MqttPublisher::getQualityOfService() const
{
    return QosAtMostOnce;
}

MqttPublisher::RetainFlag MqttPublisher::getRetainFlag() const
{
    return m_retainFlag;
}

}
