#include "Mqtt.h"
#include "Mqttp.h"

namespace EBLi
{

Mqtt *Mqtt::s_instance = nullptr;

Mqtt *Mqtt::init()
{
    return instance();
}

#if defined(CONFIG_ENABLE_EBLI_CONFIG_MANAGER)
Mqtt *Mqtt::init(ConfigManager *configManager)
{
}
#endif

Mqtt *Mqtt::instance()
{
    if (nullptr == s_instance) {
        s_instance = new Mqtt;
    }

    return s_instance;
}

bool Mqtt::isConnected() const
{
    return m_mqtt->isConnected();
}

void Mqtt::addSubscriber(MqttSubscriber *subscriber)
{
    m_mqtt->addSubscriber(subscriber);
}

MqttPublisher *Mqtt::createPublisher(std::string topic)
{
    return m_mqtt->createPublisher(topic);
}

Mqtt::Mqtt()
{
    m_mqtt = new Mqttp;
}

}
