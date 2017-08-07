#ifndef __TIMER_STOP_H__
#define __TIMER_STOP_H__

#define STOP_OFF -1

struct Stop {
    int position;
    uint32_t seconds;

    Stop(int _position, uint32_t _seconds) : position(_position), seconds(_seconds) {
    }
};

#endif
