#ifndef EBLI_LEDBLINKER_H
#define EBLI_LEDBLINKER_H

#include <driver/gpio.h>
#include <esp_timer.h>

namespace EBLi::Utility {

struct LedBlinkerP;

/**
 * blink a led
 * the blink will be the following sequence
 * 1. LED on, time in ms is configured via setPulseTime - onTimeMs
 * 2. LED off, time in ms is configured via setPulseTime - offTimeMs - if equal 0, same as onTime
 * 3. repeat for pulse count, configured via setPulseCount
 * 4. pause for pauseTimeMs, configured via setPauseTime
 * 5. repeat from 1
 */
class LedBlinker
{
public:
    explicit LedBlinker(gpio_num_t gpio_num);
    ~LedBlinker();

    LedBlinker* setPulseTime(uint16_t onTimeMs, uint16_t offTimeMs = 0);
    LedBlinker* setPulseCount(uint8_t pulseCount);
    LedBlinker* setPauseTime(uint16_t pauseTimeMs);

private:
    LedBlinkerP *m_p;
    esp_timer_handle_t m_ledUpdateTimer;
    void restartTimer();
};

}

#endif //EBLI_LEDBLINKER_H
