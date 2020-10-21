#ifndef EBLI_BATTERY_H
#define EBLI_BATTERY_H

#include <esp_adc_cal.h>

namespace EBLi::Sensors {

class Battery
{
public:
    explicit Battery(adc1_channel_t adcChannel);

    float getVoltageDividerInverseFactor() const;
    void setVoltageDividerInverseFactor(float voltageDividerInverseFactor);
    // voltageDividerInverseFactor = (r1+r2)/r1
    void setVoltageDividerResistorValues(float r1, float r2);

    bool readRawData();
    int getRawValue();
    float getVoltageOnPort();
    float getVoltage();

private:
    float m_voltageDividerInverseFactor = 1.0;

    adc1_channel_t m_adcChannel;
    esp_adc_cal_characteristics_t *m_adcCalCharacteristics;
    int m_adcRawValue = 0;
};

}

#endif //EBLI_BATTERY_H
