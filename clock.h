#ifndef __TIMER_CLOCK_H__
#define __TIMER_CLOCK_H__

namespace clock {

    class Stopwatch {
    public:
        Stopwatch();
        unsigned remaining();

    private:
        explicit Stopwatch(unsigned seconds);
        unsigned endTime_;
        bool running_;

        friend Stopwatch stopwatch(unsigned, bool);
    };

    void stabilize();
    Stopwatch stopwatch(unsigned seconds, bool syncClock = false);
    void pause();
    void resume();
    void attachInterrupt(void (*userFn)(void));
    void detachInterrupt();
}

#endif
