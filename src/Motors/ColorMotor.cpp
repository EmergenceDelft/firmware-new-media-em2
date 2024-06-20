// ColorMotor.cpp
#include "ColorMotor.h"
#include "Motor.h"

ColorMotor::ColorMotor(int address, Adafruit_PWMServoDriver pwm, int interval)
    : Motor(address, pwm, interval) {}

void ColorMotor::setMinAngle(int x){
    MIN_ANGLE = x;
}
void ColorMotor::setMaxAngle(int x){
    MAX_ANGLE = x;
}

void ColorMotor::setRotationIncrement(int x){
    ROTATION_INCREMENT = x;
}
void ColorMotor::setSnapIncrement(int x){
    SNAP_INCREMENT = x;
}
void ColorMotor::setMinJitterIncrement(int x){
    MIN_JITTER_INCREMENT = x;
}
void ColorMotor::setMaxJitterIncrement(int x){
    MAX_JITTER_INCREMENT = x;
}



void ColorMotor::update() {
    if(millis() - _last_update > _interval){
        if(_moving) {
            ColorMotor::moveContinously();
        }else{
            ColorMotor::moveToAngle();
        }
    }
}

void ColorMotor::moveContinously() {
    if(_current_angle >= MAX_ANGLE){
        _movingTowards180 = false;
    }
    if(_current_angle <= MIN_ANGLE){
        _movingTowards180 = true;
    }
    int increment;
    if(_jitter) {
        int random_nr = ColorMotor::generateRandomBetween(MIN_JITTER_INCREMENT,MAX_JITTER_INCREMENT);
        if(_movingTowards180) {
            increment = random_nr;
        }else {
            increment = -random_nr;
        }
    }else{
        if(_movingTowards180) {
            increment = ROTATION_INCREMENT;
        }else {
            increment = -ROTATION_INCREMENT;
        }
    }
    _current_angle += increment;
    setAngle(_current_angle);

}

void ColorMotor::moveToAngle() {
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



int ColorMotor::generateRandomBetween(int a, int b) {
    int random_integer = rand() % (b+1);
    return random_integer + a;
}
