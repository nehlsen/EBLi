#include <algorithm>
#include "LedBlinker.h"

namespace EBLi::Utility {

struct LedBlinkerP
{
    gpio_num_t gpio_num;

    uint16_t onTimeMs = 100;
    uint16_t offTimeMs = 100;
    uint8_t pulseCount = 2;
    uint16_t pauseTimeMs = 750;

    uint16_t timerDelayMs() const
    {
        return std::min(std::min(onTimeMs, offTimeMs), pauseTimeMs);
    }

    uint16_t sequenceDuration() const
    {
        return (onTimeMs + offTimeMs) * pulseCount + pauseTimeMs;
    }

    int lastSequenceStart = 0;

    bool shouldBeOn()
    {
        assert(pulseCount > 0);
        assert(onTimeMs >= 10);

        if (lastSequenceStart == 0) {
            lastSequenceStart = esp_timer_get_time();
            return true;
        }

        int currentTime = esp_timer_get_time();
        int sequenceTime = currentTime - lastSequenceStart; // from 0 to sequenceDuration
        sequenceTime /= 1000;

        int testTime = 0;
        for (int pulse = 0; pulse < pulseCount; ++pulse) {
            testTime += onTimeMs;
            if (sequenceTime <= testTime) return true;

            testTime += offTimeMs;
            if (sequenceTime <= testTime) return false;
        }

        testTime += pauseTimeMs;
        if (sequenceTime <= testTime) return false;

        // restart sequence
        lastSequenceStart = esp_timer_get_time();
        // the pause will actually be pauseTimeMs + timer-interval
        return false;
    }
};

static void led_update_callback(void *cbArg)
{
    auto blinker = static_cast<LedBlinkerP*>(cbArg);
    ESP_ERROR_CHECK(nullptr == blinker ? ESP_FAIL : ESP_OK);

    gpio_set_level(blinker->gpio_num, blinker->shouldBeOn());
    // TODO change timer delay according to next required delay
}

LedBlinker::LedBlinker(gpio_num_t gpio_num)
{
    m_p = new LedBlinkerP;
    m_p->gpio_num = gpio_num;

    gpio_pad_select_gpio(gpio_num);
    gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);

    const esp_timer_create_args_t led_update_timer_args = {
            .callback = &led_update_callback,
            .arg = (void*)m_p,
            .name = "led_update_timer"
    };

    ESP_ERROR_CHECK(esp_timer_create(&led_update_timer_args, &m_ledUpdateTimer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(m_ledUpdateTimer, m_p->timerDelayMs() * 1000));
}

LedBlinker::~LedBlinker()
{
    ESP_ERROR_CHECK(esp_timer_stop(m_ledUpdateTimer));
    ESP_ERROR_CHECK(esp_timer_delete(m_ledUpdateTimer));
    delete m_p;
}

LedBlinker* LedBlinker::setPulseTime(uint16_t onTimeMs, uint16_t offTimeMs)
{
    m_p->onTimeMs = onTimeMs;
    m_p->offTimeMs = offTimeMs == 0 ? onTimeMs : offTimeMs;

    return this;
}

LedBlinker* LedBlinker::setPulseCount(uint8_t pulseCount)
{
    m_p->pulseCount = pulseCount;

    return this;
}

LedBlinker* LedBlinker::setPauseTime(uint16_t pauseTimeMs)
{
    m_p->pauseTimeMs = pauseTimeMs;

    return this;
}

void LedBlinker::restartTimer()
{
    ESP_ERROR_CHECK(esp_timer_stop(m_ledUpdateTimer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(m_ledUpdateTimer, m_p->timerDelayMs() * 1000));
}

}
