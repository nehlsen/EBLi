#include "Delayer.h"
#include <esp_timer.h>

namespace EBLi::Utility {

Delayer::Delayer(unsigned long delayMs):
    m_delay(0), m_lastTime(0)
{
    setDelay(delayMs);
}

void Delayer::setDelay(unsigned long delayMs)
{
    m_delay = delayMs;
}

unsigned long Delayer::delay() const
{
    return m_delay;
}

void Delayer::disable()
{
    setDelay(0);
}

bool Delayer::isActive() const
{
    return m_delay > 0;
}

bool Delayer::peek() const
{
    if (!isActive()) {
        return false;
    }

    return millis() - m_lastTime >= m_delay;
}

bool Delayer::isTimedOut()
{
    if (!isActive()) {
        return false;
    }

    unsigned long currentTime = millis();

    if (currentTime - m_lastTime >= m_delay) {
        m_lastTime = currentTime;
        return true;
    } else if (currentTime < m_lastTime) {
        // after a couple of days millis rolls back to zero
        // this fall back will result in a little skipping
        // but the delayer should continue to produce time-outs...
        m_lastTime = 0;
    }

    return false;
}

void Delayer::reset()
{
    m_lastTime = millis();
}

unsigned long Delayer::millis()
{
    return esp_timer_get_time() / 1000;
}

}
