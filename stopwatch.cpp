#include <Arduino.h>
#include <util/atomic.h>
#include "stopwatch.h"

namespace {
    volatile uint32_t secondsRemaining_ = 0UL;
    bool initialized_ = false;

//Overflow ISR
    ISR(TIMER2_OVF_vect) {
        if (secondsRemaining_ > 0) {
            secondsRemaining_--;
        }
    }
}

namespace stopwatch {
    void reset(uint32_t seconds) {
        pause();

        secondsRemaining_ = seconds;

        if (!initialized_) {
            initialized_ = true;
            // Enable asynchronous mode
            ASSR = bit(AS2);
            // Set prescaler to 128 (32768 / 256, which gives an overflow every second)
            TCCR2B |= bit(CS22) | bit(CS00);
        }
        
        // Reset initial counter value
        TCNT2 = 0;
        // Wait for registers to update
        while (ASSR & (bit(TCR2BUB) | bit(TCN2UB)));

        resume();
    }

    void pause() {
        // Disable timer2 interrupts
        TIMSK2 = 0;
    }

    void resume() {
        // Clear interrupt flags
        TIFR2 = bit(TOV2);
        // Enable Timer2 overflow interrupt
        TIMSK2 = bit(TOIE2);
    }

    uint32_t remaining() {
        uint32_t secondsRemaining;

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            secondsRemaining = secondsRemaining_;
        }

        return secondsRemaining;
    }
}
