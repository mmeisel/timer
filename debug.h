#ifndef __TIMER_DEBUG_H__
#define __TIMER_DEBUG_H__

#include "config.h"

#if CONFIG_DEBUG
#define DEBUG_BEGIN(baud) Serial.begin(baud)
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DEBUG_FLUSH() Serial.flush()
#else
#define DEBUG_BEGIN(baud) do {} while (false)
#define DEBUG_PRINT(...) do {} while (false)
#define DEBUG_PRINTLN(...) do {} while (false)
#define DEBUG_FLUSH() do {} while (false)
#endif

#endif
