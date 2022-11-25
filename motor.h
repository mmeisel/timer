#ifndef __TIMER_MOTOR_H__
#define __TIMER_MOTOR_H__

class Motor {
public:
    explicit Motor(int enablePin);
    bool isRunning() const;
    void start();
    void stop();

private:
    bool isRunning_;
    int enablePin_;
};

#endif
