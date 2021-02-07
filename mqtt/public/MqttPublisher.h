#ifndef EBLI_MQTT_PUBLISHER_H
#define EBLI_MQTT_PUBLISHER_H

#include <string>

class cJSON;

namespace EBLi::mqtt {

class Mqttp;
class MqttPublisher
{
friend class Mqttp;

public:
    [[nodiscard]] std::string getTopic() const;

    void publishValue(uint8_t value);
    void publishValue(cJSON *jsonObject);
    void publishValue(const std::string& value);

    enum QualityOfService {
        QosAtMostOnce = 0,
        QosAtLeastOnce = 1,
        QosExactlyOnce = 2,
    };
    [[nodiscard]] QualityOfService getQualityOfService() const;

    enum RetainFlag {
        NotRetainValue = 0,
        RetainValue = 1,
    };
    [[nodiscard]] RetainFlag getRetainFlag() const;

private:
    explicit MqttPublisher(Mqttp *mqtt, std::string topic, RetainFlag retainFlag);
    Mqttp *m_mqtt;
    const std::string m_topic;
    const RetainFlag m_retainFlag;
};

}

#endif //EBLI_MQTT_PUBLISHER_H
