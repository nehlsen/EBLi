#ifndef EBLI_MQTT_PUBLISHER_H
#define EBLI_MQTT_PUBLISHER_H

#include <string>

namespace EBLi
{

class Mqttp;
class MqttPublisher
{
friend class Mqttp;

public:
    std::string getTopic() const;

    void publishValue(uint8_t value);
    void publishValue(std::string value);
    // TODO publish: int, float, JSON, ...

private:
    explicit MqttPublisher(Mqttp *mqtt, std::string topic);
    Mqttp *m_mqtt;
    const std::string m_topic;
};

}

#endif //EBLI_MQTT_PUBLISHER_H
