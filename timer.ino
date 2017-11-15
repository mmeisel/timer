#include <LowPower.h>
#include "adc.h"
#include "audio.h"
#include "clock.h"
#include "motor.h"
#include "PCM.h"
#include "stop.h"

//#define DEBUG 1

// Pins
#define PIN_SLIDER_IN A0
#define PIN_MOTOR_ENABLE 5
#define PIN_MOTOR_OFF 12
#define PIN_DIR1 8
#define PIN_DIR2 7
#define PIN_POWER 2   // Needs to be an interruptable pin! (2 or 3)
#define PIN_SPEAKER 6   // Can't be changed

// Constants
#define SIGNIFICANT_POSITION_CHANGE STOP_SIZE_NORMAL
#define BELL_REPEAT_SECS 6



int currentPosition_ = ~0;
int positionChange_ = 0;
stop::Stop currentStop_;
stop::Stop nextStop_;
clock::Stopwatch stopwatch_;

Motor motor_(PIN_MOTOR_OFF, PIN_MOTOR_ENABLE, PIN_DIR1, PIN_DIR2);

bool ringing_ = false;
clock::Stopwatch bellStopwatch_;

volatile bool ticked_ = false;



#ifdef DEBUG

volatile unsigned int interruptCount_;

#define DEBUG_REPORT(message) printReport(message)
#define DEBUG_REPORT_IF(condition, message) ((condition) ? printReport(message) : (void) 0)

void printReport(const __FlashStringHelper* message) {
    Serial.print(message);
    Serial.print(F("\n\tremaining="));
    Serial.print(stopwatch_.remaining());
    Serial.print(F(" currentStop="));
    Serial.print(currentStop_.index);
    Serial.print(F(" nextStop="));
    Serial.print(nextStop_.index);
    Serial.print(F(" currentPosition="));
    Serial.print(currentPosition_);
    Serial.print(F(" interruptCount="));
    Serial.print(interruptCount_);
    Serial.print(F("\n"));
    Serial.flush();
}

#else

#define DEBUG_REPORT(x) ((void) 0)
#define DEBUG_REPORT_IF(x, y) ((void) 0)

#endif



void handlePinInterrupt() {
#ifdef DEBUG
    interruptCount_++;
#endif
}

void handleTick() {
    ticked_ = true;
}

void sleep() {
    clock::prepareForSleep();
    LowPower.powerSave(SLEEP_FOREVER, ADC_OFF, BOD_OFF, TIMER2_ON);
}

void shutdown() {
    motor_.setDirection(MotorDirection::OFF);
    clock::pause();

    if (digitalRead(PIN_POWER) == LOW) {
#ifdef DEBUG
        Serial.print(F("Can't fully power down, adjust resistor values\n"));
        Serial.flush();
#endif
        LowPower.powerSave(SLEEP_1S, ADC_OFF, BOD_OFF, TIMER2_ON);
        return;
    }

#ifdef DEBUG
    Serial.print(F("Powering down\n"));
    Serial.flush();
#endif

    // Power down and wake up only if we get an interrupt from the power pin
    attachInterrupt(digitalPinToInterrupt(PIN_POWER), handlePinInterrupt, LOW);
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

    // When turned on again, stop listening for interrupts on the power pin
    detachInterrupt(digitalPinToInterrupt(PIN_POWER));
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
    if (currentPosition_ < nextStop_.startPosition) {
        // Handle overshoot
        DEBUG_REPORT_IF(motor_.direction() != MotorDirection::FORWARD, F("Motor forward"));
        motor_.setDirection(MotorDirection::FORWARD);
    }
    else if (currentPosition_ > nextStop_.startPosition + STOP_DETENTE_SIZE) {
        // Normal movement to the next stop.
        DEBUG_REPORT_IF(motor_.direction() != MotorDirection::REVERSE, F("Motor reverse"));
        motor_.setDirection(MotorDirection::REVERSE);
    }
    else {
        // We reached the stop!
        motor_.setDirection(MotorDirection::OFF);
        currentStop_ = nextStop_;
        DEBUG_REPORT(F("Motor off"));
        updateBell();
    }
}

void updateBell() {
    bool shouldRing = currentStop_.index == STOP_INDEX_ZERO;

    if (ringing_ && !shouldRing) {
        // Set ringing_ to false only when the sound completes
        ringing_ = isPlaying();
    }
    else if (shouldRing) {
        if (!ringing_ || bellStopwatch_.remaining() == 0) {
#ifdef DEBUG
            Serial.print(F("DING!\n"));
            Serial.flush();
#endif
            startPlayback(audio::DATA, audio::DATA_SIZE);
            bellStopwatch_ = clock::stopwatch(BELL_REPEAT_SECS);
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
#ifdef DEBUG
        Serial.print(F("Tick "));
        Serial.print(nextStop_.index);
        Serial.print(F(" ("));
        Serial.print(nextStop_.seconds);
        Serial.print(F(") to "));
        Serial.print(index);
        Serial.print(F(" ("));
        Serial.print(stop::byIndex(index).seconds);
        Serial.print(F(")"));
#endif

        nextStop_ = stop::byIndex(index);
        // The DEBUG_REPORT is here instead of the above block so nextStop is printed correctly
        DEBUG_REPORT(F(""));
        updateMotor();
    }
}

void waitForCrystal() {
    // After starting from power down, the crystal needs to stabilize, see:
    // http://www.atmel.com/images/atmel-1259-real-time-clock-rtc-using-the-asynchronous-timer_ap-note_avr134.pdf
#ifdef DEBUG
    Serial.print(F("Waiting for crystal\n"));
    Serial.flush();
#endif

    clock::stabilize();
}

void setup() {
#ifdef DEBUG
    interruptCount_ = 0;
    Serial.begin(57600);
#endif

    pinMode(PIN_MOTOR_ENABLE, OUTPUT);
    pinMode(PIN_DIR1, OUTPUT);
    pinMode(PIN_DIR2, OUTPUT);
    pinMode(PIN_POWER, INPUT_PULLUP);
    pinMode(PIN_SPEAKER, OUTPUT);

    digitalWrite(PIN_SPEAKER, LOW);
    adc::setPin(PIN_SLIDER_IN);
    stop::createStops();
    clock::attachInterrupt(handleTick);
    motor_.setDirection(MotorDirection::OFF);

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
        if (motor_.direction() != MotorDirection::OFF) {
            // When the motor is moving, just keep going until we hit the desired position.
            updateMotor();
        }
        else if (positionChange_ >= SIGNIFICANT_POSITION_CHANGE &&
            (currentPosition_ < currentStop_.startPosition ||
             currentPosition_ >= currentStop_.endPosition))
        {
            // If the position changed significantly and isn't where we're intending to be, a human
            // must have moved the slider. If they actually moved it to a new stop, update the stop.
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

    if (motor_.direction() == MotorDirection::OFF) {
        // When the motor isn't running, enter low power operation. If the slider is in the off
        // position, shutdown completely.
        if (currentStop_.index == STOP_INDEX_OFF) {
            shutdown();
        }
        else {
            sleep();
        }
    }
}
