#include <Arduino.h>
#include <LowPower.h>
#include <util/atomic.h>
#include "adc.h"



// Inspired by https://www.gammon.com.au/adc

namespace {
    uint8_t pin_ = 0;
    volatile bool isRunning_ = false;
    volatile int lastValue_ = 0;

    ISR(ADC_vect) {
        lastValue_ = ADC;
        isRunning_ = false;
    }
}


namespace adc {
    void init(uint8_t pin) {
        // Allow for channel or pin numbers
        pin_ = pin >= 14 ? (pin - 14) : pin;
        // Set the pin for the conversion (and the reference voltage to the default (VCC))
        ADMUX = bit(REFS0) | (pin_ & 0x07);
        // Enable ADC, enable ADC interrupt, and set prescaler. The default Uno setup is a 16
        // MHz clock divided by 128. We are using an 8 MHz clock divided by 64.
        ADCSRA = bit(ADEN) | bit(ADIE) | bit(ADPS2) | bit(ADPS1);
    }

    void read() {
        if (!isRunning_) {
            isRunning_ = true;

            // If the processor wakes up before the reading is done, keep waiting.
            while (isRunning_) {
                // ADC noise reduction mode starts the conversion automatically if the ADC is on.
                LowPower.adcNoiseReduction(SLEEP_FOREVER, ADC_ON, TIMER2_ON);
            }
        }
    }

    int lastValue() {
        return lastValue_;
    }
}
