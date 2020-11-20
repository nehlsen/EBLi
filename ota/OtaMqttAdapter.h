#ifndef EBLI_OTAMQTTADAPTER_H
#define EBLI_OTAMQTTADAPTER_H

namespace EBLi {
class MqttPublisher;
}

namespace EBLi::ota {

class OtaUpdater;
class OtaMqttAdapter
{
public:
    explicit OtaMqttAdapter(OtaUpdater *updater);

private:
    OtaUpdater *m_updater;

    void subscribeForUpdateRequests();
    void subscribeOtaEvents();

    MqttPublisher *m_otaEventPublisher = nullptr;
    void createOtaEventPublisher();
};

}

#endif //EBLI_OTAMQTTADAPTER_H
