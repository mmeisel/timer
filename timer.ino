#include <avr/pgmspace.h>
#include <LowPower.h>
#include "adc.h"
#include "audio.h"
#include "clock.h"
#include "config.h"
#include "debug.h"
#include "motor.h"
#include "PCM.h"
#include "stop.h"

// Constants
const uint8_t BELL_INTERVALS[] PROGMEM = { 10, 10, 30, 30, 30, 60, 60, 60 };
const int BELL_INTERVAL_COUNT = sizeof(BELL_INTERVALS) / sizeof(uint8_t);

// Globals
int currentPosition_ = ~0;
bool setByHuman_ = false;
unsigned setByHumanAt_ = 0;
stop::Stop currentStop_ = stop::STOP_NOT_A_STOP;
stop::Stop desiredStop_ = stop::STOP_NOT_A_STOP;
clock::Stopwatch stopwatch_;

Motor motor_(CONFIG_PIN_MOTOR_ENABLE);

int ringCount_ = 0;
clock::Stopwatch bellStopwatch_;

volatile bool nextSecond_ = false;



#if CONFIG_DEBUG

unsigned long loopCount_ = 0UL;

#define DEBUG_LOOP_COUNT() (loopCount_++)
#define DEBUG_LOOP_COUNT_RESET() (loopCount_ = 0UL)
#define DEBUG_REPORT(message) printReport(message)

void printReport(const __FlashStringHelper* message) {
    DEBUG_PRINT(message);
    DEBUG_PRINT(F("\n\tremaining="));
    DEBUG_PRINT(stopwatch_.remaining());
    DEBUG_PRINT(F("s currentStop="));
    DEBUG_PRINT(currentStop_.index);
    DEBUG_PRINT(F(" desiredStop="));
    DEBUG_PRINT(desiredStop_.index);
    DEBUG_PRINT(F(" currentPosition="));
    DEBUG_PRINT(currentPosition_);
    DEBUG_PRINT(F(" desiredPosition="));
    DEBUG_PRINT(desiredStop_.endPosition - STOP_MARGIN);
    DEBUG_PRINT(F(" loop="));
    DEBUG_PRINT(loopCount_);
    DEBUG_PRINT(F("\n"));
    DEBUG_FLUSH();
}

#else

#define DEBUG_LOOP_COUNT() do {} while (false)
#define DEBUG_LOOP_COUNT_RESET() do {} while (false)
#define DEBUG_REPORT(message) do {} while (false)

#endif



void handleClockInterrupt() {
    nextSecond_ = true;
}

void goToSleep() {
    DEBUG_FLUSH();
    // De-energize the slider potentiometer during sleep to save power
    pinMode(CONFIG_PIN_SLIDER_GROUND, INPUT_PULLUP);
    // The clock has some preparation to do before we go into a deep sleep mode
    clock::prepareForSleep();

    LowPower.powerSave(SLEEP_FOREVER, ADC_OFF, BOD_OFF, TIMER2_ON);

    // Re-energize the slide potentiometer
    pinMode(CONFIG_PIN_SLIDER_GROUND, OUTPUT);
    digitalWrite(CONFIG_PIN_SLIDER_GROUND, LOW);
}

void updateStateFromAdc() {
    const int newPosition = adc::lastValue();

    if (newPosition == currentPosition_) {
        return;
    }
    currentPosition_ = newPosition;

    const stop::Stop& newStop = stop::byPosition(newPosition);

    if (newStop.index == currentStop_.index) {
        return;
    }
    currentStop_ = newStop;

    // If we just reached stop zero (where we ring the bell), reset the bell parameters
    if (newStop.index == STOP_INDEX_ZERO) {
        ringCount_ = 0;
        bellStopwatch_ = clock::stopwatch(0);
    }

    // Ignore humans while the motor is running, this prevents the inherent jitter in repeated ADC
    // measurements from being falsely interpreted as human intervention.
    if (motor_.isRunning()) {
        return;
    }

    // If the stop changes when the motor isn't running, it must have been the result of a human
    // moving it. Update state accordingly.
    desiredStop_ = newStop;

    // Add CONFIG_FEEDBACK_DELAY_SECONDS so that the timer starts when the bell sounds
    stopwatch_ = clock::stopwatch(newStop.seconds + CONFIG_FEEDBACK_DELAY_SECONDS, true);
    // When resetting the stopwatch, unset nextSecond_ since any unhandled interrupts are stale now.
    nextSecond_ = false;
    setByHuman_ = true;
    setByHumanAt_ = clock::time();
}

void runMotor() {
    if (currentPosition_ >= desiredStop_.endPosition - STOP_MARGIN) {
        if (!motor_.isRunning()) {
            DEBUG_REPORT(F("Motor start"));
            motor_.start();
        }
    }
    else if (motor_.isRunning()) {
        motor_.stop();
        DEBUG_REPORT(F("Motor stop"));
    }
}

void playSound(unsigned char const *data, int length) {
    DEBUG_FLUSH();
    startPlayback(data, length);

    // Don't do anything else until the sound is done playing. Use idle mode so we can leave
    // Timer0 and Timer1 running to output sound.
    while (isPlaying()) {
        LowPower.idle(SLEEP_FOREVER, ADC_OFF, TIMER2_ON, TIMER1_ON, TIMER0_ON,
                      SPI_OFF, USART0_OFF, TWI_OFF);
    }
}

void ringBell() {
    if (bellStopwatch_.remaining() > 0) {
        return;
    }

    if (ringCount_ <= BELL_INTERVAL_COUNT) {
        // Schedule the next ring (if there is one) before we sound this one so they aren't delayed
        // by the time it takes to play the audio
        if (ringCount_ < BELL_INTERVAL_COUNT) {
            bellStopwatch_ = clock::stopwatch(pgm_read_byte(&(BELL_INTERVALS[ringCount_])));
        }

        DEBUG_PRINT(F("DING "));
        DEBUG_PRINTLN(ringCount_);
        playSound(audio::DATA, audio::DATA_SIZE);

        ringCount_++;
    }
    else {
        desiredStop_ = stop::STOP_OFF;
    }
}

void checkStopwatch() {
    if (!nextSecond_) {
        // If a second hasn't passed yet, nothing to do.
        return;
    }
    nextSecond_ = false;

    const unsigned remaining = stopwatch_.remaining();
    int index = desiredStop_.index;

    // Find the lowest stop with seconds greater than or equal to remaining
    while (index > STOP_INDEX_ZERO && stop::byIndex(index - 1).seconds >= remaining) {
        index--;
    }

    if (index < desiredStop_.index) {
        DEBUG_PRINT(F("Tick "));
        DEBUG_PRINT(desiredStop_.index);
        DEBUG_PRINT(F(" ("));
        DEBUG_PRINT(desiredStop_.seconds);
        DEBUG_PRINT(F("s) to "));
        DEBUG_PRINT(index);
        DEBUG_PRINT(F(" ("));
        DEBUG_PRINT(stop::byIndex(index).seconds);
        DEBUG_PRINTLN(F("s)"));

        desiredStop_ = stop::byIndex(index);
    }
}

void waitForCrystal() {
    // After starting from power down, the crystal needs to stabilize, see:
    // http://www.atmel.com/images/atmel-1259-real-time-clock-rtc-using-the-asynchronous-timer_ap-note_avr134.pdf
    DEBUG_PRINT(F("Waiting for crystal\n"));
    DEBUG_FLUSH();

    clock::stabilize();
}

void setup() {
    DEBUG_BEGIN(CONFIG_DEBUG_BAUD_RATE);

    // Disable the analog comparator to save power (we aren't using it)
    ACSR = bit(ACD);

    pinMode(CONFIG_PIN_SLIDER_GROUND, OUTPUT);
    digitalWrite(CONFIG_PIN_SLIDER_GROUND, LOW);

    pinMode(CONFIG_PIN_SPEAKER, OUTPUT);
    digitalWrite(CONFIG_PIN_SPEAKER, LOW);

    pinMode(CONFIG_PIN_MOTOR_ENABLE, OUTPUT);
    digitalWrite(CONFIG_PIN_MOTOR_ENABLE, LOW);

    adc::init(CONFIG_PIN_SLIDER_IN);
    stop::createStops();
    clock::attachInterrupt(handleClockInterrupt);

    waitForCrystal();
}

void loop() {
    DEBUG_LOOP_COUNT();
    DEBUG_FLUSH();
    adc::read();
    updateStateFromAdc();
    checkStopwatch();
    runMotor();

    if (!motor_.isRunning()) {
        if (currentStop_.index == STOP_INDEX_ZERO) {
            ringBell();
        }
        else if (setByHuman_ && clock::time() >= setByHumanAt_ + CONFIG_FEEDBACK_DELAY_SECONDS) {
            // Give feedback that the human set the time after they haven't touched the slider for
            // CONFIG_FEEDBACK_DELAY_SECONDS.
            setByHuman_ = false;
            DEBUG_REPORT(F("Stop set by human"));

            if (currentStop_.index != STOP_INDEX_OFF) {
                // Play a little bit of audio to indicate the timer was set
                playSound(audio::DATA + audio::DATA_SIZE / 2, audio::DATA_SIZE / 2);
            }
        }
        goToSleep();
    }
}
