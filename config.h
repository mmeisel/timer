#ifndef __TIMER_CONFIG_H__
#define __TIMER_CONFIG_H__

#define CONFIG_DEBUG false
#define CONFIG_DEBUG_BAUD_RATE 57600

// How long to wait after a human sets the timer to give audible feedback and start counting
#define CONFIG_FEEDBACK_DELAY_SECONDS 1

// Pins
#define CONFIG_PIN_MOTOR_ENABLE 5
#define CONFIG_PIN_SLIDER_IN A0
#define CONFIG_PIN_SLIDER_GROUND 2
// This would require code changes to use anything other than pin 6 (OC0A)
// This is just here as a reference of which pins are in use, and for initialization in setup()
#define CONFIG_PIN_SPEAKER 6

#endif
