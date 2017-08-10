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
#define PIN_START_INTERRUPT 2   // Needs to be an interruptable pin! (2 or 3)
#define PIN_BELL LED_BUILTIN

// Constants
#define ADC_FUDGE_FACTOR 2



int currentPosition_ = ~0;
stop::Stop currentStop_ = stop::STOP_NOT_A_STOP;
stop::Stop nextStop_ = stop::STOP_NOT_A_STOP;
Motor motor_(PIN_ENABLE, PIN_DIR1, PIN_DIR2);

volatile bool ticked_ = false;



#ifdef DEBUG

volatile unsigned int interruptCount_;

#define DEBUG_REPORT() printReport()
#define DEBUG_REPORT_IF(condition) ((condition) ? printReport() : (void) 0)

void printReport() {
    Serial.print(F("remaining = "));
    Serial.print(stopwatch::remaining());
    Serial.print(F(", currentStop = "));
    Serial.print(currentStop_.index);
    Serial.print(F(", nextStop = "));
    Serial.print(nextStop_.index);
    Serial.print(F(", motorDirection = "));
    Serial.print((int) motor_.direction());
    Serial.print(F(", currentPosition = "));
    Serial.print(currentPosition_);
    Serial.print(F(", interruptCount = "));
    Serial.print(interruptCount_);
    Serial.print(F(", millisAwake = "));
    Serial.print(millis());
    Serial.print(F("\n"));
    Serial.flush();
}

#else

#define DEBUG_REPORT() ((void) 0)
#define DEBUG_REPORT_IF(x) ((void) 0)

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

    if (digitalRead(PIN_START_INTERRUPT) == LOW) {
#ifdef DEBUG
        Serial.print(F("Can't fully power down, adjust resistor values\n"));
        Serial.flush();
#endif
        LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
        return;
    }

    attachInterrupt(digitalPinToInterrupt(PIN_START_INTERRUPT), handlePinInterrupt, LOW);

#ifdef DEBUG
    Serial.print(F("Powering down\n"));
    Serial.flush();
#endif
    
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

    // When turned on again, stop listening for interrupts on the start detection pin, and start
    // a fresh ADC check right away.
    detachInterrupt(digitalPinToInterrupt(PIN_START_INTERRUPT));
}

bool updatePosition() {
    if (!adc::isRunning()) {
        int newPosition = adc::lastValue();

        if (abs(currentPosition_ - newPosition) > ADC_FUDGE_FACTOR) {
            // Ignore position changes that aren't greater than ADC_FUDGE_FACTOR
            currentPosition_ = newPosition;
            return true;
        }
    }

    return false;
}

void updateMotor() {
    if (currentPosition_ < nextStop_.startPosition) {
        // Handle overshoot
        DEBUG_REPORT_IF(motor_.direction() != MotorDirection::FORWARD);
        motor_.setDirection(MotorDirection::FORWARD);
    }
    else if (currentPosition_ > nextStop_.startPosition + STOP_DETENTE_SIZE) {
        // Normal movement to the next stop. Special case for position 0: make sure the slider gets
        // all the way down there!
        DEBUG_REPORT_IF(motor_.direction() != MotorDirection::REVERSE);
        motor_.setDirection(MotorDirection::REVERSE);
    }
    else {
        // We reached the detente!
        motor_.setDirection(MotorDirection::OFF);
        currentStop_ = nextStop_;
        DEBUG_REPORT();
    }
}

void updateStop() {
    stop::Stop nearestStop = stop::byPosition(currentPosition_);

    currentStop_ = nearestStop;
    nextStop_ = nearestStop;

    digitalWrite(PIN_BELL, nextStop_.index == STOP_INDEX_ZERO ? HIGH : LOW);

    if (currentStop_.index != STOP_INDEX_OFF) {
        stopwatch::reset(currentStop_.seconds);
    }

    DEBUG_REPORT();
}



void setup() {
#ifdef DEBUG
    interruptCount_ = 0;
    Serial.begin(57600);
#endif

#ifndef DEBUG
    // We have our own clock (using timer2). Initialize that and disable timer0, which is what
    // powers millis(). We don't need it, and it won't be accurate anyway.
    TIMSK0 &= ~bit(TOIE0);
#endif

    pinMode(PIN_ENABLE, OUTPUT);
    pinMode(PIN_DIR1, OUTPUT);
    pinMode(PIN_DIR2, OUTPUT);
    pinMode(PIN_BELL, OUTPUT);
    pinMode(PIN_START_INTERRUPT, INPUT_PULLUP);

    digitalWrite(PIN_BELL, LOW);
    adc::setPin(PIN_SLIDER_IN);
    stopwatch::attachInterrupt(handleTick);
    motor_.setDirection(MotorDirection::OFF);

    currentPosition_ = adc::read();
    updateStop();
    updateMotor();
}

void loop() {
    adc::startAndSleep();

    if (updatePosition()) {
        if (motor_.direction() != MotorDirection::OFF) {
            // When the motor is moving, just keep going until we hit the desired position.
            updateMotor();
        }
        else if (currentPosition_ < currentStop_.startPosition - ADC_FUDGE_FACTOR ||
            currentPosition_ > currentStop_.endPosition + ADC_FUDGE_FACTOR)                
        {
            // The motor isn't moving, but the position changed, so a human must have moved the
            // slider. If they actually moved it to a new stop, update the stop.
            updateStop();
        }
    }
    else if (ticked_) {
        // If the position didn't change but the stopwatch ticked, check if it's time to go to the
        // next stop.
        ticked_ = false;

        int index = nextStop_.index;

        if (index != STOP_INDEX_OFF && index != STOP_INDEX_ZERO) {
            // Normal operation (not off or ringing): move the slider down as time passes
            stop::Stop nextNextStop = stop::byIndex(index - 1);

            if (stopwatch::remaining() <= nextNextStop.seconds) {
                if (nextNextStop.index == STOP_INDEX_ZERO) {
                    digitalWrite(PIN_BELL, HIGH);
                }
                nextStop_ = nextNextStop;
                updateMotor();
            }
        }
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
