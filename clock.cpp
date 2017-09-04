#include <Arduino.h>
#include <util/atomic.h>
#include "clock.h"

// Based on http://www.embedds.com/avr-timer2-asynchronous-mode/

#define UNDEFINED -1L

// TODO: move to some common header
// #define DEBUG 1

namespace {
    volatile unsigned secondsRemaining_ = 0;
    bool ready_ = false;
    void (*userFn_)(void) = nullptr;

    volatile long average_ = UNDEFINED;
    volatile long deviation_ = UNDEFINED;
    volatile unsigned long lastMillis_ = 0;

    // Overflow ISR
    ISR(TIMER2_OVF_vect) {
        if (ready_) {
            // Normal countdown operation
            if (secondsRemaining_ > 0) {
                secondsRemaining_--;
            }
            if (userFn_) {
                userFn_();
            }
        }
        else {
            // Stabilization mode
            unsigned long curMillis = millis();
            long millisPast = (long) (curMillis - lastMillis_);
            long diff = millisPast - average_;

            if (average_ == UNDEFINED) {
                average_ = millisPast;
                deviation_ = abs(diff);
            }
            else {
                // Jacobson-Karels TCP RTT algorithm
                average_ += diff >> 3;
                deviation_ += (abs(diff) - deviation_) >> 2;
            }

            lastMillis_ = curMillis;
        }
    }

    void init_(uint8_t prescaler) {
        // Enable asynchronous mode using the watch crystal
        ASSR = bit(AS2);
        // Disable compare output, use normal waveform generation mode (counter), and set the
        // prescaler value
        TCCR2A = 0;
        TCCR2B = prescaler & 0x7;

        // Wait for registers to update
        while (ASSR & (bit(TCR2BUB) | bit(TCR2AUB)));

        // Reset prescaler and wait for it to finish
        GTCCR |= bit(PSRASY);
        while (GTCCR & bit(PSRASY));
    }
}

namespace clock {
    void stabilize() {
        ready_ = false;
        pause();

        // Don't prescale so we can get as many samples as possible
        init_(0x1);

        // Use the interrupt to track the deviation and wait for it to stabilize
        average_ = UNDEFINED;
        deviation_ = UNDEFINED;
        resume();

#ifdef DEBUG
        long debugLastDeviation = deviation_;
#endif

        while (deviation_ != 0) {
#ifdef DEBUG
            if (deviation_ != debugLastDeviation) {
                Serial.print(F(" average="));
                Serial.print(average_);
                Serial.print(F(" deviation="));
                Serial.print(deviation_);
                Serial.print(F("\n"));
                Serial.flush();
                debugLastDeviation = deviation_;
            }
#endif
        }

#ifdef DEBUG
        Serial.print(F(" average="));
        Serial.print(average_);
        Serial.print(F(" deviation="));
        Serial.print(deviation_);
        Serial.print(F("\n"));
        Serial.flush();
#endif

        // Don't do anything else until reset() is called
        pause();
    }

    void reset(unsigned seconds) {
        pause();

        secondsRemaining_ = seconds;

        if (!ready_) {
            // Set prescaler to 128 (32768 / 256), which gives an overflow every second
            init_(0x5);
            ready_ = true;
        }

        // Reset initial counter value
        TCNT2 = 0;
        // Wait for register to update
        while (ASSR & bit(TCN2UB));

        resume();
    }

    void pause() {
        // Disable timer2 interrupts
        TIMSK2 = 0;
    }

    void resume() {
        // Clear interrupt flag
        TIFR2 = bit(TOV2);
        // Enable Timer2 overflow interrupt
        TIMSK2 = bit(TOIE2);
    }

    unsigned remaining() {
        unsigned secondsRemaining;

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            secondsRemaining = secondsRemaining_;
        }

        return secondsRemaining;
    }

    void attachInterrupt(void (*userFn)(void)) {
        userFn_ = userFn;
    }

    void detachInterrupt() {
        userFn_ = nullptr;
    }

}
