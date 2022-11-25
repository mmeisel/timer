#include <avr/pgmspace.h>
#include "stop.h"

namespace {
    // There can be extra times here, they just won't get used.
    const uint16_t STOP_TIMES[] PROGMEM = {
        // For 39 stops
        // Every 30 seconds up to 3 minutes (6 stops)
        30, 60, 90, 120, 150, 180,
        // Every minute up to 30 minutes (27 stops)
        240,  300,  360,  420,  480,  540,  600,  660,  720,
        780,  840,  900,  960,  1020, 1080, 1140, 1200, 1260,
        1320, 1380, 1440, 1500, 1560, 1620, 1680, 1740, 1800,
        // Every 5 minutes up to 1 hour (6 stops)
        2100, 2400, 2700, 3000, 3300, 3600
        /*
        // For 60 stops
        // Every 15 seconds up to 2 minutes
        15, 30, 45, 60, 75, 90, 105, 120,
        // Every 30 seconds up to 5 minutes
        150, 180, 210, 240, 270, 300,
        // Every minute up to 30 minutes
        360,  420,  480,  540,  600,  660,  720,  780,  840,  900,
        960,  1020, 1080, 1140, 1200, 1260, 1320, 1380, 1440, 1500,
        1560, 1620, 1680, 1740, 1800,
        // Every 2 minutes up to 1 hour
        1920, 2040, 2160, 2280, 2400, 2520, 2640, 2760, 2880, 3000, 3120, 3240, 3360, 3480, 3600,
        // Every 20 minutes up to 3 hours
        4800, 6000, 7200, 8400, 9600, 10800
        */
        // Every two seconds for testing
        /*
        2,  4,  6,  8,  10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40,
        42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80
        */
    };

    stop::Stop stops_[STOP_COUNT];
}

namespace stop {
    void createStops() {
        int numStopTimes = sizeof(STOP_TIMES) / sizeof(uint16_t);

        for (int i = 0; i < STOP_COUNT; i++) {
            int startPos = STOP_SIZE_OFF + STOP_SIZE_NORMAL * (i + 1);
            int stopIndex = i >= numStopTimes ? (numStopTimes - 1) : i;
            uint16_t stopTime = pgm_read_word(&(STOP_TIMES[stopIndex]));

            stops_[i] = stop::Stop(startPos, startPos + STOP_SIZE_NORMAL, stopTime, i);
        }
    }

    Stop byIndex(int index) {
        if (index == STOP_INDEX_OFF) {
            return STOP_OFF;
        }
        else if (index == STOP_INDEX_ZERO) {
            return STOP_ZERO;
        }
        else if (index < STOP_COUNT) {
            return stops_[index];
        }

        return STOP_NOT_A_STOP;
    }

    Stop byPosition(int position) {
        if (position < 0 || position > 1023) {
            return STOP_NOT_A_STOP;
        }

        // The first two stops are "special" stops: off and zero, respectively.
        if (position < STOP_SIZE_OFF) {
            return STOP_OFF;
        }

        if (position < STOP_SIZE_OFF + STOP_SIZE_NORMAL) {
            return STOP_ZERO;
        }

        return stops_[(position - STOP_SIZE_OFF - STOP_SIZE_NORMAL) / STOP_SIZE_NORMAL];
    }
}
