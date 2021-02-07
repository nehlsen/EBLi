#ifndef EBLI_CONFIG_MQTTBRIDGE_H
#define EBLI_CONFIG_MQTTBRIDGE_H

namespace EBLi::config {
class ConfigProperty;

class MqttBridge
{
public:
    static void setup();

    static void handleRegistered(ConfigProperty *configProperty);
    static void handleChanged(ConfigProperty *configProperty);

private:
    MqttBridge() = default;
};

}

#endif //EBLI_CONFIG_MQTTBRIDGE_H
