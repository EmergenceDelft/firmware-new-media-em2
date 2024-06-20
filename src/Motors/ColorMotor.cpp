// ColorMotor.cpp
#include "ColorMotor.h"
#include "Motor.h"

ColorMotor::ColorMotor(int address, Adafruit_PWMServoDriver pwm, int interval)
    : Motor(address, pwm, interval) {}

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
    if(_current_angle >= 180){
        _movingTowards180 = false;
    }
    if(_current_angle <= 0){
        _movingTowards180 = true;
    }
    if(_jitter) {
        int random_nr = ColorMotor::generateRandomBetween(-2,5);
        if(_movingTowards180) {
            _increment = random_nr;
        }else {
            _increment = -random_nr;
        }
    }else{
        if(_movingTowards180) {
            _increment = 1;
        }else {
            _increment = -1;
        }
    }
    _last_update = millis();
    _current_angle += _increment;
    setAngle(_current_angle);

}

void ColorMotor::moveToAngle() {
    if(_current_angle != _target_angle) {
        //set increment according to whether we need to increase or decrease current_angle
        if(_current_angle < _target_angle){
            _increment = 10;
        }else{
            _increment = -10;
        }

        _last_update = millis();
        _current_angle += _increment;
        setAngle(_current_angle);
    }
}



int ColorMotor::generateRandomBetween(int a, int b) {
    int random_integer = rand() % (b+1);
    return random_integer + a;
}
