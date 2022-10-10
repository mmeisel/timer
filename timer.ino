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
const uint8_t BELL_INTERVALS[] PROGMEM = { 10, 10, 10, 30, 30, 30, 60, 60, 60 };
const int BELL_INTERVAL_COUNT = sizeof(BELL_INTERVALS) / sizeof(uint8_t);

// Globals
int currentPosition_ = ~0;
int positionChange_ = 0;
bool setByHuman_ = false;
stop::Stop currentStop_;
stop::Stop nextStop_;
clock::Stopwatch stopwatch_;

Motor motor_(CONFIG_PIN_MOTOR_ENABLE);

bool ringing_ = false;
int ringCount_ = 0;
clock::Stopwatch bellStopwatch_;

volatile bool ticked_ = false;



#if CONFIG_DEBUG

volatile unsigned int interruptCount_ = 0;

#define DEBUG_RECORD_INTERRUPT() interruptCount_++
#define DEBUG_REPORT(message) printReport(message)

void printReport(const __FlashStringHelper* message) {
    DEBUG_PRINT(message);
    DEBUG_PRINT(F("\n\tremaining="));
    DEBUG_PRINT(stopwatch_.remaining());
    DEBUG_PRINT(F(" currentStop="));
    DEBUG_PRINT(currentStop_.index);
    DEBUG_PRINT(F(" nextStop="));
    DEBUG_PRINT(nextStop_.index);
    DEBUG_PRINT(F(" currentPosition="));
    DEBUG_PRINT(currentPosition_);
    DEBUG_PRINT(F(" interruptCount="));
    DEBUG_PRINT(interruptCount_);
    DEBUG_PRINT(F("\n"));
    DEBUG_FLUSH();
}

#else

#define DEBUG_RECORD_INTERRUPT() do {} while (false)
#define DEBUG_REPORT(message) do {} while (false)

#endif



void handlePinInterrupt() {
    DEBUG_RECORD_INTERRUPT();
}

void handleTick() {
    ticked_ = true;
}

void goToSleep() {
    clock::prepareForSleep();
    LowPower.powerSave(SLEEP_FOREVER, ADC_OFF, BOD_OFF, TIMER2_ON);
}

void shutdown() {
    motor_.stop();
    clock::pause();

    if (digitalRead(CONFIG_PIN_POWER) == LOW) {
        DEBUG_PRINT(F("Can't fully power down, adjust resistor values\n"));
        DEBUG_FLUSH();
        LowPower.powerSave(SLEEP_1S, ADC_OFF, BOD_OFF, TIMER2_ON);
        return;
    }

    DEBUG_PRINT(F("Powering down\n"));
    DEBUG_FLUSH();

    // Power down and wake up only if we get an interrupt from the power pin
    attachInterrupt(digitalPinToInterrupt(CONFIG_PIN_POWER), handlePinInterrupt, LOW);
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

    // When turned on again, stop listening for interrupts on the power pin
    detachInterrupt(digitalPinToInterrupt(CONFIG_PIN_POWER));
    waitForCrystal();
}

bool updatePosition() {
    if (!adc::isRunning()) {
        int newPosition = adc::lastValue();

        positionChange_ = abs(newPosition - currentPosition_);
        currentPosition_ = newPosition;
        return positionChange_ != 0;
    }

    return false;
}

void updateMotor() {
    if (currentPosition_ >= nextStop_.endPosition - STOP_MARGIN) {
        // Normal movement to the next stop.
        if (CONFIG_DEBUG && !motor_.isRunning()) {
            DEBUG_REPORT(F("Motor start"));
        }
        motor_.start();
    }
    else {
        // We reached the stop!
        motor_.stop();
        currentStop_ = nextStop_;
        DEBUG_REPORT(F("Motor stop"));
        updateBell();
    }
}

void updateBell() {
    bool shouldRing = currentStop_.index == STOP_INDEX_ZERO && ringCount_ <= BELL_INTERVAL_COUNT;

    if (ringing_ && !shouldRing) {
        // Set ringing_ to false only when the sound completes
        ringing_ = isPlaying();

        if (!ringing_) {
            // If we reach the maximum number of rings, wait until the last ring sound has finished
            // playing, then shut ourselves off. We only need to get the slider moving, the other
            // logic will handle everything else.
            if (ringCount_ >= BELL_INTERVAL_COUNT) {
                nextStop_ = stop::STOP_OFF;
                updateMotor();
            }

            // Reset the ring count for next time
            ringCount_ = 0;
        }
    }
    else if (shouldRing) {
        if (!ringing_ || bellStopwatch_.remaining() == 0) {
            DEBUG_PRINT(F("DING "));
            DEBUG_PRINT(ringCount_);
            DEBUG_PRINT(F("\n"));
            DEBUG_FLUSH();
            startPlayback(audio::DATA, audio::DATA_SIZE);

            if (ringCount_ < BELL_INTERVAL_COUNT) {
                bellStopwatch_ = clock::stopwatch(pgm_read_byte(&(BELL_INTERVALS[ringCount_])));
            }
            ringCount_++;
        }
        ringing_ = true;
    }
}

void setStopFromPosition() {
    currentStop_ = stop::byPosition(currentPosition_);
    nextStop_ = currentStop_;

    if (currentStop_.index != STOP_INDEX_OFF) {
        stopwatch_ = clock::stopwatch(currentStop_.seconds, true);
        // When resetting the stopwatch, unset ticked_ since any unhandled interrupts are useless
        // now, anyway.
        ticked_ = false;
    }

    DEBUG_REPORT(F("Stop set by human"));
    setByHuman_ = true;
    updateBell();
}

void checkStopwatch() {
    unsigned remaining = stopwatch_.remaining();
    int index = nextStop_.index;

    // Find the lowest stop with seconds greater than or equal to remaining
    while (index > STOP_INDEX_ZERO && stop::byIndex(index - 1).seconds >= remaining) {
        index--;
    }

    if (index < nextStop_.index) {
        DEBUG_PRINT(F("Tick "));
        DEBUG_PRINT(nextStop_.index);
        DEBUG_PRINT(F(" ("));
        DEBUG_PRINT(nextStop_.seconds);
        DEBUG_PRINT(F(") to "));
        DEBUG_PRINT(index);
        DEBUG_PRINT(F(" ("));
        DEBUG_PRINT(stop::byIndex(index).seconds);
        DEBUG_PRINT(F(")"));

        nextStop_ = stop::byIndex(index);
        // The DEBUG_REPORT is here instead of the above block so nextStop is printed correctly
        DEBUG_REPORT(F(""));
        updateMotor();
    }
    else if (setByHuman_ && nextStop_.seconds - remaining > 0) {
        // Check if we should correct the position of the slider. Wait at least one second after
        // the position was set manually so it hopefully happens after the person lets go.
        setByHuman_ = false;
        updateMotor();
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
    DEBUG_BEGIN(57600);

    pinMode(CONFIG_PIN_POWER, INPUT_PULLUP);

    pinMode(CONFIG_PIN_SPEAKER, OUTPUT);
    digitalWrite(CONFIG_PIN_SPEAKER, LOW);

    motor_.stop();
    adc::setPin(CONFIG_PIN_SLIDER_IN);
    stop::createStops();
    clock::attachInterrupt(handleTick);

    waitForCrystal();

    currentPosition_ = adc::read();
    setStopFromPosition();
}

void loop() {
    while (isPlaying()) {
        // Don't do anything but play the sound. Use idle mode so we can leave Timer0 and Timer1
        // running to output sound.
        LowPower.idle(SLEEP_FOREVER, ADC_OFF, TIMER2_ON, TIMER1_ON, TIMER0_ON,
                      SPI_OFF, USART0_OFF, TWI_OFF);
    }

    adc::startAndSleep();

    if (updatePosition()) {
        if (motor_.isRunning()) {
            // When the motor is moving, just keep going until we hit the desired position.
            updateMotor();
        }
        else if (currentPosition_ < currentStop_.startPosition ||
                 currentPosition_ >= currentStop_.endPosition)
        {
            // If the slider is at a different stop than it's supposed to be, a human must have
            // moved it. Update the stop.
            setStopFromPosition();
        }
    }

    if (ticked_) {
        // If the stopwatch ticked, check if it's time to go to the next stop.
        ticked_ = false;
        checkStopwatch();
    }

    if (ringing_) {
        // When the bell is ringing, allow the sound effect to repeat until the state changes
        updateBell();
    }

    if (!motor_.isRunning()) {
        // When the motor isn't running, enter low power operation. If the slider is in the off
        // position, shutdown completely.
        if (currentStop_.index == STOP_INDEX_OFF) {
            shutdown();
        }
        else {
            goToSleep();
        }
    }
}
