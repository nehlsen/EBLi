#ifndef LEDWALL_MQTTBRIDGE_H
#define LEDWALL_MQTTBRIDGE_H

namespace EBLi
{

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
#endif //LEDWALL_MQTTBRIDGE_H
