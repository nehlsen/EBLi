#include "Battery.h"
#include <driver/adc.h>
#include <ebli_log.h>

namespace EBLi::Sensors {

#define DEFAULT_VREF 1100

Battery::Battery(adc1_channel_t adcChannel):
    m_adcChannel(adcChannel)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(m_adcChannel, ADC_ATTEN_DB_6);

    m_adcCalCharacteristics = (esp_adc_cal_characteristics_t*)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_6, ADC_WIDTH_BIT_12, DEFAULT_VREF, m_adcCalCharacteristics);
}

bool Battery::readRawData()
{
    const int numberOfSamples = 64;
    int reading = 0;
    for (int i = 0; i< numberOfSamples; ++i) {
        reading += adc1_get_raw(m_adcChannel);
    }

    m_adcRawValue = reading / numberOfSamples;

    return m_adcRawValue != -1;
}

int Battery::getRawValue()
{
    return m_adcRawValue;
}

float Battery::getVoltage()
{
    float voltageOnPort = getVoltageOnPort();
    float voltage = voltageOnPort * getVoltageDividerInverseFactor();

    ESP_LOGI(LOG_TAG_SENSORS, "Battery: raw:%d, onPort: %.3f, voltage:%.3f", m_adcRawValue, voltageOnPort, voltage);

    return voltage;
}

float Battery::getVoltageOnPort()
{
    return esp_adc_cal_raw_to_voltage(m_adcRawValue, m_adcCalCharacteristics);
}

float Battery::getVoltageDividerInverseFactor() const
{
    return m_voltageDividerInverseFactor;
}

void Battery::setVoltageDividerInverseFactor(float voltageDividerInverseFactor)
{
    m_voltageDividerInverseFactor = voltageDividerInverseFactor;
}

void Battery::setVoltageDividerResistorValues(float r1, float r2)
{
    setVoltageDividerInverseFactor((r1+r2)/r1);
}

}
