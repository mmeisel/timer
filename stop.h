#ifndef __TIMER_STOP_H__
#define __TIMER_STOP_H__

#define STOP_SIZE_SPECIAL 32
#define STOP_SIZE_NORMAL 16
#define STOP_DETENTE_SIZE 8

#define STOP_INDEX_OFF -2
#define STOP_INDEX_ZERO -1

namespace stop {
    struct Stop {
        int startPosition;
        int endPosition;
        unsigned seconds;
        int index;

        Stop(int sp, int ep, unsigned secs, int i)
            : startPosition(sp), endPosition(ep), seconds(secs), index(i)
        {
        }
    };

    Stop byIndex(int index);
    Stop byPosition(int position);

    const Stop STOP_OFF(0, STOP_SIZE_SPECIAL - 1, 0, STOP_INDEX_OFF);
    const Stop STOP_ZERO(STOP_SIZE_SPECIAL, STOP_SIZE_SPECIAL * 2 - 1, 0, STOP_INDEX_ZERO);
    const Stop STOP_NOT_A_STOP(-255, -255, 0, -255);
}

#endif
