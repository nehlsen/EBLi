#ifndef EBLI_STATUS_H
#define EBLI_STATUS_H

namespace EBLi::Utility {
class LedBlinker;
}

namespace EBLi::Status {

class Status
{
public:
    Status();

    enum WifiState {
        WIFI_CONNECTING,
        WIFI_PROVISIONING,
        WIFI_CONNECTED,
        WIFI_DISCONNECTED,
    };
    void setWifiState(WifiState state);

    enum MqttState {
        MQTT_CONNECTED,
        MQTT_DISCONNECTED,
    };
    void setMqttState(MqttState state);

private:
    Utility::LedBlinker *m_ledBlinker;
    void updateBlinker();

    WifiState m_wifiState = WIFI_DISCONNECTED;
    MqttState m_mqttState = MQTT_DISCONNECTED;
};

}

#endif //EBLI_STATUS_H
