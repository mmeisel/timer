#include <LowPower.h>
#include "adc.h"
#include "motor.h"
#include "stop.h"
#include "stopwatch.h"

#define DEBUG 1

// Pins
#define PIN_SLIDER_IN A0
#define PIN_ENABLE 5
#define PIN_DIR1 6
#define PIN_DIR2 7
#define PIN_POWER 2   // Needs to be an interruptable pin! (2 or 3)
#define PIN_BELL 9

// Constants
#define SIGNIFICANT_POSITION_CHANGE 8
#define CRYSTAL_STABILIZATION_MS 1000



int currentPosition_ = ~0;
int positionChange_ = 0;
bool ringing_ = false;
stop::Stop currentStop_;
stop::Stop nextStop_;
Motor motor_(PIN_ENABLE, PIN_DIR1, PIN_DIR2);

volatile bool ticked_ = false;



#ifdef DEBUG

volatile unsigned int interruptCount_;

#define DEBUG_REPORT(message) printReport(message)
#define DEBUG_REPORT_IF(condition, message) ((condition) ? printReport(message) : (void) 0)

void printReport(const char* message) {
    Serial.print(message);
    Serial.print(F("\n\tremaining="));
    Serial.print(stopwatch::remaining());
    Serial.print(F(" currentStop="));
    Serial.print(currentStop_.index);
    Serial.print(F(" nextStop="));
    Serial.print(nextStop_.index);
    Serial.print(F(" currentPosition="));
    Serial.print(currentPosition_);
    Serial.print(F(" interruptCount="));
    Serial.print(interruptCount_);
    Serial.print(F(" millisAwake="));
    Serial.print(millis());
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

void shutdown() {
    motor_.setDirection(MotorDirection::OFF);
    stopwatch::pause();

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
        DEBUG_REPORT_IF(motor_.direction() != MotorDirection::FORWARD, "Motor forward");
        motor_.setDirection(MotorDirection::FORWARD);
    }
    else if (currentPosition_ > nextStop_.startPosition + STOP_DETENTE_SIZE) {
        // Normal movement to the next stop. Special case for position 0: make sure the slider gets
        // all the way down there!
        DEBUG_REPORT_IF(motor_.direction() != MotorDirection::REVERSE, "Motor reverse");
        motor_.setDirection(MotorDirection::REVERSE);
    }
    else {
        // We reached the stop!
        motor_.setDirection(MotorDirection::OFF);
        currentStop_ = nextStop_;
        DEBUG_REPORT("Motor off");
    }
}

void updateBell() {
    bool newState = nextStop_.index == STOP_INDEX_ZERO;

    if (newState != ringing_) {
        ringing_ = newState;
        digitalWrite(PIN_BELL, ringing_ ? HIGH : LOW);
    }
}

void setStopFromPosition() {
    currentStop_ = stop::byPosition(currentPosition_);
    nextStop_ = currentStop_;
    updateBell();

    if (currentStop_.index != STOP_INDEX_OFF) {
        stopwatch::reset(currentStop_.seconds);
        // When resetting the stopwatch, unset ticked_ since any unhandled interrupts are useless
        // now, anyway.
        ticked_ = false;
    }

    DEBUG_REPORT("Stop set by human");
}

void checkStopwatch() {
    unsigned remaining = stopwatch::remaining();
    int index = nextStop_.index;

    // Find the lowest stop with seconds greater than or equal to remaining
    while (index > STOP_INDEX_ZERO && stop::byIndex(index - 1).seconds >= remaining) {
        index--;
    }

    if (index < nextStop_.index) {
#ifdef DEBUG
        Serial.print("Tick ");
        Serial.print(nextStop_.index);
        Serial.print(" (");
        Serial.print(nextStop_.seconds);
        Serial.print(") to ");
        Serial.print(index);
        Serial.print(" (");
        Serial.print(stop::byIndex(index).seconds);
        Serial.print(")");
#endif

        nextStop_ = stop::byIndex(index);
        // The DEBUG_REPORT is here instead of the above block so nextStop is printed correctly
        DEBUG_REPORT("");
        updateBell();
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

    stopwatch::stabilize();
}

void setup() {
#ifdef DEBUG
    interruptCount_ = 0;
    Serial.begin(57600);
#endif

    pinMode(PIN_ENABLE, OUTPUT);
    pinMode(PIN_DIR1, OUTPUT);
    pinMode(PIN_DIR2, OUTPUT);
    pinMode(PIN_BELL, OUTPUT);
    pinMode(PIN_POWER, INPUT_PULLUP);

    adc::setPin(PIN_SLIDER_IN);
    stop::createStops();
    stopwatch::attachInterrupt(handleTick);
    motor_.setDirection(MotorDirection::OFF);

    waitForCrystal();

    currentPosition_ = adc::read();
    setStopFromPosition();
    updateMotor();
}

void loop() {
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

    if (motor_.direction() == MotorDirection::OFF) {
        // When the motor isn't running, enter low power operation. If the slider is in the off
        // position, shutdown completely.
        if (currentStop_.index == STOP_INDEX_OFF) {
            shutdown();
        }
        else {
            LowPower.powerSave(SLEEP_FOREVER, ADC_OFF, BOD_OFF, TIMER2_ON);
        }
    }
}
