#include <avr/pgmspace.h>
#include "stop.h"

namespace {
    // These won't necessarily all get used.
    /*
    const uint16_t STOP_TIMES[] PROGMEM = {
        3, 6, 9, 13, 15, 18, 21, 24, 27, 30,
        36, 42, 48, 54,
        60, 66, 72, 78, 84, 90, 96, 102, 108, 114, 120, 126, 132, 138, 144, 150, 156, 162, 168, 174, 180,
        192, 204, 216, 228, 240, 252, 264, 276, 288, 300, 312, 324, 336, 348, 360,
        396, 432, 468, 504, 540, 576, 612, 648, 684, 720,
        840, 960, 1080, 1200
    };
    */
    const uint16_t STOP_TIMES[] PROGMEM = {
        1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60
    };

    stop::Stop stops_[STOP_COUNT];
}

namespace stop {
    void createStops() {
        int numStopTimes = sizeof(STOP_TIMES) / sizeof(uint16_t);

        for (int i = 0; i < STOP_COUNT; i++) {
            int startPos = STOP_SIZE_SPECIAL * 2 + STOP_SIZE_NORMAL * i;
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
        if (position < STOP_SIZE_SPECIAL) {
            return STOP_OFF;
        }
        
        if (position < STOP_SIZE_SPECIAL * 2) {
            return STOP_ZERO;
        }

        return stops_[(position - STOP_SIZE_SPECIAL * 2) / STOP_SIZE_NORMAL];
    }
}
