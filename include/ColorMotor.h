// ColorMotor.h
#ifndef COLORMOTOR_H
#define COLORMOTOR_H

#include "Motor.h"

class ColorMotor : public Motor {
public:
    ColorMotor(int address, Adafruit_PWMServoDriver pwm, int interval);
    void update() override;
private:
    void moveContinously();
    void moveToAngle();
};

#endif
