#ifndef __TIMER_STOPWATCH_H__
#define __TIMER_STOPWATCH_H__

namespace stopwatch {
    void reset(unsigned seconds);
    void pause();
    void resume();
    unsigned remaining();
    void attachInterrupt(void (*userFn)(void));
    void detachInterrupt();
}

#endif
