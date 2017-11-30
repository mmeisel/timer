#ifndef __TIMER_STOP_H__
#define __TIMER_STOP_H__

#define STOP_SIZE_OFF 64    // The absolute max here is 65 with the current resistor setup
#define STOP_SIZE_NORMAL 24
#define STOP_COUNT ((1024 - STOP_SIZE_OFF - STOP_SIZE_NORMAL) / STOP_SIZE_NORMAL)
#define STOP_DETENTE_SIZE 8

#define STOP_INDEX_OFF -2
#define STOP_INDEX_ZERO -1
#define STOP_INDEX_NOT_A_STOP -255

namespace stop {
    struct Stop {
        int startPosition;
        int endPosition;
        unsigned seconds;
        int index;

        Stop()
            : startPosition(-255), endPosition(-255), seconds(0), index(STOP_INDEX_NOT_A_STOP)
        {
        }

        Stop(int sp, int ep, unsigned secs, int i)
            : startPosition(sp), endPosition(ep), seconds(secs), index(i)
        {
        }
    };

    void createStops();
    Stop byIndex(int index);
    Stop byPosition(int position);
    Stop byRemaining();

    const Stop STOP_OFF(0, STOP_SIZE_OFF, 0, STOP_INDEX_OFF);
    const Stop STOP_ZERO(STOP_SIZE_OFF, STOP_SIZE_OFF + STOP_SIZE_NORMAL, 0, STOP_INDEX_ZERO);
    const Stop STOP_NOT_A_STOP;
}

#endif
