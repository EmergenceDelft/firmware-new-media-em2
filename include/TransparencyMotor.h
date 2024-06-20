// TransparencyMotor.h
#ifndef TRANSPARENCYMOTOR_H
#define TRANSPARENCYMOTOR_H

#include "Motor.h"

class TransparencyMotor : public Motor {
public:
    TransparencyMotor(int address, Adafruit_PWMServoDriver pwm, int interval);
    void update() override;
    void setActiveAngle(int x);
    void setInactiveAngle(int x);
    void setSnapIncrement(int x);
private:
    int ACTIVE_ANGLE = 0;
    int INACTIVE_ANGLE = 180;
    int SNAP_INCREMENT = 10;


};

#endif
