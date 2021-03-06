#ifndef __TIMER_ADC_H__
#define __TIMER_ADC_H__

namespace adc {
    void setPin(uint8_t pin);
    void start();
    void startAndSleep(bool enableTimer2 = true);
    int read();
    bool isRunning();
    int lastValue();
}

#endif
