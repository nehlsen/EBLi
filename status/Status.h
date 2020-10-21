#ifndef EBLI_STATUS_H
#define EBLI_STATUS_H

namespace EBLi::Status {

class Status
{
public:
    Status();

    enum State {
        NOT_CONNECTED = 1,
        WIFI_CONNECTED = 2,
        MQTT_CONNECTED = 3,
    };
    State getState() const;

    void setWifiConnected(bool isConnected);

private:
    bool m_wifiConnected = false;
};

}

#endif //EBLI_STATUS_H
