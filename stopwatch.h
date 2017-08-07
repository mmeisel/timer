#ifndef __TIMER_STOPWATCH_H__
#define __TIMER_STOPWATCH_H__

namespace stopwatch {
    void reset(uint32_t seconds);
    void pause();
    void resume();
    uint32_t remaining();
}

#endif
