#ifndef EBLI_DELAYER_H
#define EBLI_DELAYER_H

namespace EBLi::Utility {

class Delayer
{
public:
    Delayer(unsigned long delayMs);

    // set the delay in milli seconds (1000ms = 1s)
    void setDelay(unsigned long delayMs);

    unsigned long delay() const;

    // set delay to 0
    void disable();

    // whether the delay is set to a value greater than zero
    bool isActive() const;

    // check if timeout is reached without remembering the result
    bool peek() const;

    // check if timeout is reached and restart the delay if necessary (not active delays do not time out)
    bool isTimedOut();

    // re-start delay
    void reset();

protected:
    unsigned long m_delay;
    unsigned long m_lastTime;

    static unsigned long millis();
};

}

#endif //EBLI_DELAYER_H
