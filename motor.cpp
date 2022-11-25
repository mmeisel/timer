#include <Arduino.h>
#include "motor.h"

Motor::Motor(int enablePin)
    : isRunning_(false), enablePin_(enablePin)
{
}

bool Motor::isRunning() const {
    return isRunning_;
}

void Motor::start() {
    if (isRunning_) {
        return;
    }

    isRunning_ = true;
    digitalWrite(enablePin_, HIGH);
}

void Motor::stop() {
    if (!isRunning_) {
        return;
    }

    digitalWrite(enablePin_, LOW);
    isRunning_ = false;
}
