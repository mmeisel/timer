#include <Arduino.h>
#include "motor.h"

Motor::Motor(int enablePin)
    : initialized_(false), isRunning_(false), enablePin_(enablePin)
{
}

bool Motor::isRunning() const {
    return isRunning_;
}

void Motor::start() {
    if (!initialized_) {
        initialized_ = true;
        pinMode(enablePin_, OUTPUT);
    }
    else if (isRunning_) {
        return;
    }

    isRunning_ = true;
    digitalWrite(enablePin_, HIGH);
}

void Motor::stop() {
    if (!initialized_) {
        initialized_ = true;
        pinMode(enablePin_, OUTPUT);
    }
    else if (!isRunning_) {
        return;
    }

    isRunning_ = false;
    digitalWrite(enablePin_, LOW);
}
