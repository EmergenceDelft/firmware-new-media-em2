// TransparencyMotor.cpp
#include "TransparencyMotor.h"
#include "Motor.h"

TransparencyMotor::TransparencyMotor(int address, Adafruit_PWMServoDriver pwm, int interval)
    : Motor(address, pwm, interval) {}

void TransparencyMotor::setActiveAngle(int activeAngle){
    ACTIVE_ANGLE = activeAngle;
    Serial.print("Setting TransparencyMotor activeAngle to: ");
    Serial.println(ACTIVE_ANGLE);
}

void TransparencyMotor::setInactiveAngle(int inactiveAngle){
    INACTIVE_ANGLE = inactiveAngle;
    Serial.print("Setting TransparencyMotor inactiveAngle to: ");
    Serial.println(INACTIVE_ANGLE);
}
void TransparencyMotor::setSnapIncrement(int snapIncrement){
    SNAP_INCREMENT = snapIncrement;
    Serial.print("Setting TransparencyMotor snapIncrement to: ");
    Serial.println(SNAP_INCREMENT);
}

int TransparencyMotor::getActiveAngle() {
    return ACTIVE_ANGLE;
}

int TransparencyMotor::getInactiveAngle() {
    return INACTIVE_ANGLE;
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
        if(_current_angle == _target_angle){
            _pwm.setPWM(_address, 0, 0);
        }

    }
}

int generateRandomBetween(int a, int b) {
    int random_integer = rand() % (b+1);
    return random_integer + a;
}


