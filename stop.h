#ifndef __TIMER_STOP_H__
#define __TIMER_STOP_H__

#define STOP_OFF -1

struct Stop {
    int position;
    unsigned long ms;

    Stop(int _position, unsigned long _ms) : position(_position), ms(_ms) {}
};

#endif
