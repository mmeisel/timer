#ifndef __TIMER_STOP_H__
#define __TIMER_STOP_H__

#define STOP_SIZE_SPECIAL 32
#define STOP_SIZE_NORMAL 16
#define STOP_COUNT ((1024 - STOP_SIZE_SPECIAL * 2) / STOP_SIZE_NORMAL)
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

    const Stop STOP_OFF(0, STOP_SIZE_SPECIAL, 0, STOP_INDEX_OFF);
    const Stop STOP_ZERO(STOP_SIZE_SPECIAL, STOP_SIZE_SPECIAL * 2, 0, STOP_INDEX_ZERO);
    const Stop STOP_NOT_A_STOP;
}

#endif
