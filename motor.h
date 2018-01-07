#ifndef __TIMER_MOTOR_H__
#define __TIMER_MOTOR_H__

enum class MotorDirection {
    OFF = 255,
    STOP = 0,
    REVERSE = -1,
    FORWARD = 1
};

class Motor {
public:
    Motor(int enablePin, int pin1, int pin2);
    MotorDirection direction() const;
    void setDirection(MotorDirection direction);

private:
    bool initialized_;
    int powerOffPin_;
    int enablePin_;
    int pin1_;
    int pin2_;
    MotorDirection currentDirection_;
};

#endif
