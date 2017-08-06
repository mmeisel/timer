#ifndef _TIMER_STOPWATCH_H_
#define _TIMER_STOPWATCH_H_

namespace stopwatch {
    void setResolution(unsigned long resolutionMs);
    void reset(unsigned long ms);
    unsigned long remaining();
}

#endif
