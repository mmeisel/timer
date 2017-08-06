#include <FrequencyTimer2.h>
#include "stopwatch.h"

namespace {
    unsigned long resolutionMs_ = 1UL;
    volatile unsigned long ticksRemaining_ = 0UL;

    void tick_() {
        if (ticksRemaining_ > 0) {
            ticksRemaining_--;
        }
    }
}

namespace stopwatch {
    void setResolution(unsigned long resolutionMs) {
        resolutionMs_ = resolutionMs;
        FrequencyTimer2::setPeriod(resolutionMs_ * 1000UL);
        FrequencyTimer2::setOnOverflow(tick_);
    }

    void reset(unsigned long ms) {
        FrequencyTimer2::disable();
        ticksRemaining_ = ms / resolutionMs_;
        FrequencyTimer2::enable();
    }

    unsigned long remaining() {
        unsigned long ticksRemaining;

        noInterrupts();
        ticksRemaining = ticksRemaining_;
        interrupts();

        return ticksRemaining * resolutionMs_;
    }
}
