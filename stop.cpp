#include "stop.h"

namespace {
    const unsigned STOP_TIMES[] = {
        3, 6, 9, 13, 15, 18, 21, 24, 27, 30,
        36, 42, 48, 54, 
        60, 66, 72, 78, 84, 90, 96, 102, 108, 114, 120, 126, 132, 138, 144, 150, 156, 162, 168, 174, 180,
        192, 204, 216, 228, 240, 252, 264, 276, 288, 300, 312, 324, 336, 348, 360,
        396, 432, 468, 504, 540, 576, 612, 648, 684, 720,
        // The won't necessarily all get used.
        840, 960, 1080, 1200
    };
}

// TODO: Generate stop objects once at startup rather than on demand.

namespace stop {
    Stop byIndex(int index) {
        if (index == STOP_INDEX_OFF) {
            return STOP_OFF;
        }
        else if (index == STOP_INDEX_ZERO) {
            return STOP_ZERO;
        }
        else if ((unsigned) index < sizeof(STOP_TIMES) / sizeof(unsigned)) {
            int startPos = STOP_SIZE_SPECIAL * 2 + STOP_SIZE_NORMAL * index;

            return Stop(startPos, startPos + STOP_SIZE_NORMAL - 1, STOP_TIMES[index], index);
        }

        return STOP_NOT_A_STOP;        
    }

    Stop byPosition(int position) {
        if (position < 0 || position > 1023) {
            return STOP_NOT_A_STOP;
        }

        // The first two stops are "special" stops: off and zero, respectively.
        if (position <= STOP_SIZE_SPECIAL) {
            return Stop(0, STOP_SIZE_SPECIAL - 1, 0, STOP_INDEX_OFF);
        }
        
        if (position <= STOP_SIZE_SPECIAL * 2) {
            return Stop(STOP_SIZE_SPECIAL, STOP_SIZE_SPECIAL * 2 - 1, 0, STOP_INDEX_ZERO);            
        }

        int index = (position - STOP_SIZE_SPECIAL * 2) / STOP_SIZE_NORMAL;
        int startPos = STOP_SIZE_SPECIAL * 2 + STOP_SIZE_NORMAL * index;

        return Stop(startPos, startPos + STOP_SIZE_NORMAL - 1, STOP_TIMES[index], index);
    }
}