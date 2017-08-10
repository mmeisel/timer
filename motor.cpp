#include <Arduino.h>
#include "motor.h"

Motor::Motor(int enablePin, int pin1, int pin2)
    : initialized_(false), enablePin_(enablePin), pin1_(pin1), pin2_(pin2),
      currentDirection_(MotorDirection::OFF)
{
}

MotorDirection Motor::direction() const {
    return currentDirection_;
}

bool Motor::isTurning() const {
    return currentDirection_ == MotorDirection::REVERSE ||
           currentDirection_ == MotorDirection::FORWARD;
}

void Motor::setDirection(MotorDirection direction) {
    if (initialized_ && currentDirection_ == direction) {
        return;
    }

    currentDirection_ = direction;
    initialized_ = true;

    switch (direction) {
        case MotorDirection::REVERSE:
            digitalWrite(pin1_, LOW);
            digitalWrite(pin2_, HIGH);
            digitalWrite(enablePin_, HIGH);
            break;
        case MotorDirection::STOP:
            digitalWrite(pin1_, LOW);
            digitalWrite(pin2_, LOW);
            digitalWrite(enablePin_, HIGH);
            break;
        case MotorDirection::OFF:
            digitalWrite(pin1_, LOW);
            digitalWrite(pin2_, LOW);
            digitalWrite(enablePin_, LOW);
            break;
        case MotorDirection::FORWARD:
            digitalWrite(pin1_, HIGH);
            digitalWrite(pin2_, LOW);
            digitalWrite(enablePin_, HIGH);
            break;
    }
}
