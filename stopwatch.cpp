#include <Arduino.h>
#include <util/atomic.h>
#include "stopwatch.h"

// Based on http://www.embedds.com/avr-timer2-asynchronous-mode/

namespace {
    volatile unsigned secondsRemaining_ = 0;
    bool initialized_ = false;
    void (*userFn_)(void) = nullptr;

    // Overflow ISR
    ISR(TIMER2_OVF_vect) {
        if (secondsRemaining_ > 0) {
            secondsRemaining_--;
        }
        if (userFn_) {
            userFn_();
        }
    }
}

namespace stopwatch {
    void reset(unsigned seconds) {
        pause();

        secondsRemaining_ = seconds;

        if (!initialized_) {
            initialized_ = true;
            // Enable asynchronous mode using the watch crystal
            ASSR = bit(AS2);
            // Disable compare output, use normal waveform generation mode (counter), and
            // set prescaler to 128 (32768 / 256), which gives an overflow every second
            TCCR2A = 0;
            TCCR2B = bit(CS22) | bit(CS20);

            // Wait for registers to update
            while (ASSR & (bit(TCR2BUB) | bit(TCR2AUB)));

            // Reset prescaler and wait for it to finish
            GTCCR |= bit(PSRASY);
            while (GTCCR & bit(PSRASY));
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
