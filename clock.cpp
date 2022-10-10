#include <Arduino.h>
#include <util/atomic.h>
#include "clock.h"
#include "config.h"
#include "debug.h"

// Based on http://www.embedds.com/avr-timer2-asynchronous-mode/

#define UNDEFINED -1L
#define STABILIZE_TICK_LIMIT 24000L  // 3 seconds at 125 us per tick

namespace {
    volatile unsigned time_ = 0;
    bool ready_ = false;
    void (*userFn_)(void) = nullptr;

    volatile long average_ = UNDEFINED;
    volatile long deviation_ = UNDEFINED;
    volatile unsigned long ticks_ = 0;
    volatile unsigned long lastTicks_ = 0;

    // Compare match ISRs, used only in stabilization mode
    ISR(TIMER0_COMPB_vect) {
        ticks_++;
    }

    ISR(TIMER2_COMPB_vect) {
        unsigned long curTicks = ticks_;
        long ticksPast = (long) (curTicks - lastTicks_);
        long diff = ticksPast - average_;

        if (average_ == UNDEFINED) {
            average_ = ticksPast;
            // Leave deviation_ UNDEFINED
        }
        else {
            // Jacobson-Karels TCP RTT algorithm
            average_ += diff >> 3;
            deviation_ += (abs(diff) - deviation_) >> 2;
        }

        lastTicks_ = curTicks;
    }

    // Timer2 overflow ISR, used in normal operation
    ISR(TIMER2_OVF_vect) {
        if (ready_) {
            // Normal operation
            time_++;

            if (userFn_) {
                userFn_();
            }
        }
    }

    void init_(bool stabilizationMode) {
        // Enable asynchronous mode using the watch crystal
        ASSR = bit(AS2);

        if (stabilizationMode) {
            // Clear timer on compare match (comparison value of 63) with prescaler of 8. This
            // gives a compare match every 15,625 us (125 * 125).
            TCCR2A = bit(WGM21);
            TCCR2B = bit(CS21);
            OCR2A = 63;
            TCNT2 = 0;
        }
        else {
            // Disable compare output, use normal waveform generation mode (counter), and set the
            // prescaler to 128 (32768 / 256), which gives an overflow every second
            TCCR2A = 0;
            TCCR2B = bit(CS22) | bit(CS20);
            OCR2A = 0;
        }

        // Wait for registers to update
        while (ASSR & (bit(TCR2BUB) | bit(TCR2AUB) | bit(TCN2UB) | bit(OCR2AUB)));

        // Reset prescaler and wait for it to finish
        GTCCR |= bit(PSRASY);
        while (GTCCR & bit(PSRASY));
    }
}



namespace clock {
    Stopwatch::Stopwatch() : endTime_(0), running_(false) {
    }

    // Must only be called when the clock is paused!
    Stopwatch::Stopwatch(unsigned seconds) : endTime_(time_ + seconds), running_(true) {
    }

    unsigned Stopwatch::remaining() {
        if (running_) {
            int secondsRemaining = (int) (endTime_ - clock::time());

            if (secondsRemaining > 0) {
                return (unsigned) secondsRemaining;
            }
            else {
                running_ = false;
            }
        }
        return 0U;
    }



    void stabilize() {
        ready_ = false;
        pause();

        // Set up Timer0 for comparison with internal oscillator
        // Clear timer on compare match (comparison value of 124) with prescaler of 8. This gives
        // a compare match every 125 us (for an 8 MHz clock).
        TCCR0A = bit(WGM01);
        TCCR0B = bit(CS01);
        OCR0A = 124;
        TCNT0 = 0;

        // Reconfigure Timer2 registers for stabilization mode
        init_(true);

        // Start both timers, use the interrupt to track the deviation and wait for it to stabilize
        ticks_ = 0;
        lastTicks_ = 0;
        average_ = UNDEFINED;
        deviation_ = UNDEFINED;

        TIFR0 = bit(OCF0B);
        TIMSK0 = bit(OCIE0B);

        TIFR2 = bit(OCF2B);
        TIMSK2 = bit(OCIE2B);

        #if CONFIG_DEBUG
        long debugLastDeviation = deviation_;
        #endif

        // Wait for stabilization, or for STABILIZE_TICK_LIMIT to elapse
        while (deviation_ != 0 && ticks_ < STABILIZE_TICK_LIMIT) {
            #if CONFIG_DEBUG
            if (deviation_ != debugLastDeviation) {
                DEBUG_PRINT(F(" average="));
                DEBUG_PRINT(average_);
                DEBUG_PRINT(F(" deviation="));
                DEBUG_PRINT(deviation_);
                DEBUG_PRINT(F(" ticks="));
                DEBUG_PRINT(ticks_);
                DEBUG_PRINT(F("\n"));
                DEBUG_FLUSH();
                debugLastDeviation = deviation_;
            }
            #endif
        }

        DEBUG_PRINT(F(" average="));
        DEBUG_PRINT(average_);
        DEBUG_PRINT(F(" deviation="));
        DEBUG_PRINT(deviation_);
        DEBUG_PRINT(F("\n"));
        DEBUG_FLUSH();

        // Prepare for normal operation, but don't do anything else until stopwatch() or resume()
        // is called
        pause();
        init_(false);

        // Disable Timer0 interrupts
        TIMSK0 = 0;
        ready_ = true;
    }

    unsigned time() {
        unsigned curTime;

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            curTime = time_;
        }

        return curTime;
    }

    Stopwatch stopwatch(unsigned seconds, bool syncClock) {
        Stopwatch output;

        // Only return a running stopwatch if the clock is ready
        if (ready_) {
            pause();

            if (syncClock) {
                // Reset initial counter value
                TCNT2 = 0;
                // Wait for register to update
                while (ASSR & bit(TCN2UB));
            }

            output = Stopwatch(seconds);

            resume();
        }

        return output;
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

    void prepareForSleep() {
        // See ATmega328 datasheet, page 201
        OCR2A = 0;
        while (ASSR & bit(OCR2AUB));

    }

    void attachInterrupt(void (*userFn)(void)) {
        userFn_ = userFn;
    }

    void detachInterrupt() {
        userFn_ = nullptr;
    }

}
