// TransparencyMotor.cpp
#include "TransparencyMotor.h"
#include "Motor.h"

TransparencyMotor::TransparencyMotor(int address, Adafruit_PWMServoDriver pwm, int interval)
    : Motor(address, pwm, interval) {}

void TransparencyMotor::setActiveAngle(int x){
    ACTIVE_ANGLE = x;
}
void TransparencyMotor::setInactiveAngle(int x){
    INACTIVE_ANGLE = x;
}
void TransparencyMotor::setSnapIncrement(int x){
    SNAP_INCREMENT = x;
}


void TransparencyMotor::update() {
    if(_current_angle != _target_angle) {
        int difference = _target_angle - _current_angle;
        int increment;
        //set increment according to whether we need to increase or decrease current_angle
        if (difference > 0) {
            increment = (difference < SNAP_INCREMENT) ? difference : SNAP_INCREMENT;
        } else {
            increment = (difference > -SNAP_INCREMENT) ? difference : -SNAP_INCREMENT;
        }

        _current_angle += increment;
        setAngle(_current_angle);

    }
}

int generateRandomBetween(int a, int b) {
    int random_integer = rand() % (b+1);
    return random_integer + a;
}
