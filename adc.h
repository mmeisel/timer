#ifndef __TIMER_ADC_H__
#define __TIMER_ADC_H__

namespace adc {
    void setPin(uint8_t pin);
    void start();
    void startAndSleep();
    int read();
    bool isRunning();
    int lastValue();
}

#endif
