// ColorMotor.h
#ifndef COLORMOTOR_H
#define COLORMOTOR_H

#include "Motor.h"

class ColorMotor : public Motor {
public:
    ColorMotor(int address, Adafruit_PWMServoDriver pwm, int interval);
    void setMinAngle(int x);
    void setMaxAngle(int x);
    void setRotationIncrement(int x);
    void setSnapIncrement(int x);
    void setMinJitterIncrement(int x);
    void setMaxJitterIncrement(int x);
    void update() override;
private:
    int MIN_ANGLE = 0;
    int MAX_ANGLE = 180;
    int ROTATION_INCREMENT = 1;
    int MIN_JITTER_INCREMENT = -2;
    int MAX_JITTER_INCREMENT = 5;
    int SNAP_INCREMENT = 10;
    void moveContinously();
    void moveToAngle();
    int generateRandomBetween(int a, int b);
};

#endif
