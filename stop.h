#ifndef __TIMER_STOP_H__
#define __TIMER_STOP_H__

// These values are out of a maximum of 1024 positions (0 - 1023) to match the ADC output
#define STOP_SIZE_OFF 64
#define STOP_SIZE_NORMAL 24
#define STOP_COUNT ((1024 - STOP_SIZE_OFF - STOP_SIZE_NORMAL) / STOP_SIZE_NORMAL)
#define STOP_MARGIN 8

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
    const Stop& byIndex(int index);
    const Stop& byPosition(int position);

    const Stop STOP_OFF(0, STOP_SIZE_OFF, 0, STOP_INDEX_OFF);
    const Stop STOP_ZERO(STOP_SIZE_OFF, STOP_SIZE_OFF + STOP_SIZE_NORMAL, 0, STOP_INDEX_ZERO);
    const Stop STOP_NOT_A_STOP;
}

#endif
