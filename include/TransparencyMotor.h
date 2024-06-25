// TransparencyMotor.h
#ifndef TRANSPARENCYMOTOR_H
#define TRANSPARENCYMOTOR_H

#include "Motor.h"

class TransparencyMotor : public Motor {
public:
    TransparencyMotor(int address, Adafruit_PWMServoDriver pwm, int interval);
    void update() override;
    void setActiveAngle(int x);
    int getActiveAngle();
    void setInactiveAngle(int x);
    int getInactiveAngle();
    void setSnapIncrement(int x);
private:
    int ACTIVE_ANGLE = 0;
    int INACTIVE_ANGLE = 90;
    int SNAP_INCREMENT = 10;
};

#endif
