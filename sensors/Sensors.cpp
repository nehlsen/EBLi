#include "Sensors.h"
#include <ebli_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Mqtt.h>
#include <MqttPublisher.h>
#if defined(CONFIG_EBLI_SENSORS_AHT10_ENABLED)
#include <AHT10.h>
#endif
#if defined(CONFIG_EBLI_CONFIG_MANAGER_ENABLE)
#include <ConfigManager.h>
#endif

// TODO do not publish directly to mqtt
//  but use events to communicate the value and use an MQTT Bridge to pick up and publish
//  see config for samples
//  this would make a lot sens when we want to use the value in-program, instead of only via MQTT

namespace EBLi::Sensors {

SensorsP* Sensors::p = nullptr;

class SensorsP
{
public:
    SensorsP();

    void publishAll();

private:
#if defined(CONFIG_EBLI_SENSORS_AHT10_ENABLED)
    AHT10 *m_aht10 = nullptr;
    EBLi::MqttPublisher *m_aht10TemperaturePublisher;
    EBLi::MqttPublisher *m_aht10HumidityPublisher;
#endif
};

[[noreturn]] void sensors_task(void *pvParameter)
{
    ESP_LOGI(LOG_TAG_SENSORS, "Starting Sensors task...");
    auto sp = static_cast<SensorsP*>(pvParameter);

    while (true) {
        if (sp) sp->publishAll();
        vTaskDelay(CONFIG_EBLI_SENSORS_PUBLISH_DELAY / portTICK_PERIOD_MS);
    }
}

SensorsP::SensorsP()
{
#if defined(CONFIG_EBLI_SENSORS_AHT10_ENABLED)
#if defined(CONFIG_EBLI_CONFIG_MANAGER_ENABLE)
    auto configManager = EBLi::ConfigManager::instance();
    int i2cPinSda = configManager->property("aht10_i2c_sda")
            ->setDefaultValue(CONFIG_AHT10_PIN_SDA)
            ->getValue<int>();
    int i2cPinScl = configManager->property("aht10_i2c_scl")
            ->setDefaultValue(CONFIG_AHT10_PIN_SCL)
            ->getValue<int>();
    ESP_LOGI(LOG_TAG_SENSORS, "Initializing AHT10 Sensor SDA:%d, SCL:%d", i2cPinSda, i2cPinScl);
    m_aht10 = new AHT10(i2cPinSda, i2cPinScl);
#else
    m_aht10 = new AHT10;
#endif
    auto mqtt = EBLi::Mqtt::instance();
    m_aht10TemperaturePublisher = mqtt->createPublisher("aht10/temperature");
    m_aht10HumidityPublisher = mqtt->createPublisher("aht10/humidity");
#endif

    xTaskCreatePinnedToCore(
            &sensors_task,
            "sensors_task",
            3*1024,
            this,
            tskIDLE_PRIORITY,
            nullptr,
            0
    );
}

void SensorsP::publishAll()
{
    ESP_LOGD(LOG_TAG_SENSORS, "publishing data...");

#if defined(CONFIG_EBLI_SENSORS_AHT10_ENABLED)
    m_aht10->readRawData();
    m_aht10TemperaturePublisher->publishValue(std::to_string(m_aht10->getTemperature()));
    m_aht10HumidityPublisher->publishValue(std::to_string(m_aht10->getHumidity()));
#endif
}

/**********************************************************************************************************************/

esp_err_t Sensors::init()
{
    if (p != nullptr) {
        return ESP_OK;
    }

    p = new SensorsP;
    return ESP_OK;
}

}
