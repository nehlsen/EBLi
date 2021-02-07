#include "OtaMqttAdapter.h"
#include <OtaUpdater.h>
#include <Mqtt.h>
#include <ebli_events.h>

namespace EBLi::ota {

OtaMqttAdapter::OtaMqttAdapter(OtaUpdater *updater) : m_updater(updater)
{
    subscribeForUpdateRequests();
    createOtaEventPublisher();
    subscribeOtaEvents();
}

void OtaMqttAdapter::subscribeForUpdateRequests()
{
    mqtt::Mqtt::instance()->createSubscriber("ota", [=](const std::string &value) {
        m_updater->setUpdateUrl(value.c_str());
        m_updater->startUpdate();
    }, mqtt::MqttSubscriber::ScopeDevice);
}

static void otaEventHandler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    auto otaEventPublisher = static_cast<mqtt::MqttPublisher*>(event_handler_arg);
    if (nullptr == otaEventPublisher) {
        return;
    }

    auto eventMsg = [event_data]() -> std::string {
        return static_cast<char*>(event_data);
    };

    std::string msg;
    switch (event_id) {
        case EBLI_EVENT_OTA_STARTED:
            msg += "Started, URL: \"" + eventMsg() + "\"";
            break;

        case EBLI_EVENT_OTA_PROGRESS:
            msg += eventMsg();
            break;

        case EBLI_EVENT_OTA_FAILED:
            msg += "FAILED: \"" + eventMsg() + "\"";
            break;

        case EBLI_EVENT_OTA_SUCCEED:
            msg += "SUCCEED: \"" + eventMsg() + "\"";
            break;

        default:
            // unknown message, fail
            return;
    }

    otaEventPublisher->publishValue(msg);
}

void OtaMqttAdapter::subscribeOtaEvents()
{
    esp_event_handler_register(EBLI_EVENTS, EBLI_EVENT_OTA_STARTED, &otaEventHandler, m_otaEventPublisher);
    esp_event_handler_register(EBLI_EVENTS, EBLI_EVENT_OTA_PROGRESS, &otaEventHandler, m_otaEventPublisher);
    esp_event_handler_register(EBLI_EVENTS, EBLI_EVENT_OTA_FAILED, &otaEventHandler, m_otaEventPublisher);
    esp_event_handler_register(EBLI_EVENTS, EBLI_EVENT_OTA_SUCCEED, &otaEventHandler, m_otaEventPublisher);
}

void OtaMqttAdapter::createOtaEventPublisher()
{
    // FIXME atw we just post some "text" messages. it would probably be better to post some machine readable info/data
    //  possibly using 4 topics: start, progress, failed, succeed
    m_otaEventPublisher = mqtt::Mqtt::instance()->createPublisher("ota");
}

}
