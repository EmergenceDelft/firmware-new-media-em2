// TransparencyMotor.h
#ifndef TRANSPARENCYMOTOR_H
#define TRANSPARENCYMOTOR_H

#include "Motor.h"

class TransparencyMotor : public Motor {
public:
    TransparencyMotor(int address, Adafruit_PWMServoDriver pwm, int interval);
    void update() override;
private:
};

#endif
