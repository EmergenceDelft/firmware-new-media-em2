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
    //angles for calibrating color filter, you have to put some values and see if the filter is straight or not
    //depending the straightness of the real filter, adjust values
    int MIN_ANGLE = 0;
    int MAX_ANGLE = 90;

    //how many angle steps to move at a time
    int ROTATION_INCREMENT = 1;

    //range for the jitter, code generates random value between (-2,5) and adds it to current angle
    //adjust this as needed for different types of jitter

    //IDEA: could  do something smarter and take the noise into account more than jitter on/off
    //some kind of linear activation that does a bit of jitter at some noise and a lot of jitter at a lot of noise
    int MIN_JITTER_INCREMENT = -2;
    int MAX_JITTER_INCREMENT = 5;

    //how fast to move when collapsing/measuring filter
    int SNAP_INCREMENT = 10;
    void moveContinously();
    void moveToAngle();
    int generateRandomBetween(int a, int b);
};

#endif
