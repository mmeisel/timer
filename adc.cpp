#include <Arduino.h>
#include <LowPower.h>
#include <util/atomic.h>
#include "adc.h"



// Inspired by https://www.gammon.com.au/adc

namespace {
    bool initialized_ = false;
    uint8_t pin_ = 0;
    volatile bool isRunning_ = false;
    volatile int lastValue_ = 0;

    void preflightCheck_() {
        if (!initialized_) {
            initialized_ = true;
            // Enable ADC, enable ADC interrupt, and set prescaler to 64. For this project,
            // this is the same speed as the default Uno setup, which is a 16 MHz clock divided by
            // 128. We are using an 8 MHz clock, so we divide by 64.
            ADCSRA = bit(ADEN) | bit(ADIE) | bit(ADPS1) | bit(ADPS2);
        }

        // Set the pin for the conversion (and the reference voltage to the default (VCC))
        ADMUX = bit(REFS0) | (pin_ & 0x07);
    }

    ISR(ADC_vect) {
        isRunning_ = false;
        lastValue_ = ADC;
    }
}


namespace adc {
    void setPin(uint8_t pin) {
        // Allow for channel or pin numbers
        pin_ = pin >= 14 ? (pin - 14) : pin;
    }

    void start() {
        if (!isRunning_) {
            preflightCheck_();
            isRunning_ = true;
            // Start the conversion
            bitSet(ADCSRA, ADSC);
        }
    }

    void startAndSleep() {
        preflightCheck_();
        isRunning_ = true;
        // Idle and ADC noise reduction modes start the conversion automatically if the ADC is on.
        // Use idle rather than adcNoiseReduction as it does not interfere with Timer2 interrupts.
        LowPower.idle(SLEEP_FOREVER, ADC_ON, TIMER2_ON, TIMER1_OFF, TIMER0_OFF,
                      SPI_OFF, USART0_OFF, TWI_OFF);
    }

    int read() {
        start();
        while (isRunning_);
        return lastValue();
    }

    bool isRunning() {
        // NOTE: no need for ATOMIC_BLOCKs with this variable as bool should be only 8 bits (which
        // our 8-bit CPU can read and write atomically).
        return isRunning_;
    }

    int lastValue() {
        // The atomic block is only necessary if the interrupt might fire, which can only happen
        // if the ADC is currently running.
        if (isRunning_) {
            int output;

            ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                output = lastValue_;
            }
            return output;
        }
        else {
            return lastValue_;
        }
    }
}
