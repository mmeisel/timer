#ifndef __TIMER_ADC_H__
#define __TIMER_ADC_H__

namespace adc {
    void init(uint8_t pin);
    void read();
    int lastValue();
}

#endif
