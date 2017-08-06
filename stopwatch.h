#ifndef __TIMER_STOPWATCH_H__
#define __TIMER_STOPWATCH_H__

namespace stopwatch {
    void setResolution(unsigned long resolutionMs);
    void reset(unsigned long ms);
    unsigned long remaining();
}

#endif
