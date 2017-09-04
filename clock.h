#ifndef __TIMER_CLOCK_H__
#define __TIMER_CLOCK_H__

namespace clock {
    void stabilize();
    void reset(unsigned seconds);
    void pause();
    void resume();
    unsigned remaining();
    void attachInterrupt(void (*userFn)(void));
    void detachInterrupt();
}

#endif
