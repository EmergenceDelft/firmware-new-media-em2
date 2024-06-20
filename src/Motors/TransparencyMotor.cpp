// TransparencyMotor.cpp
#include "TransparencyMotor.h"
#include "Motor.h"

TransparencyMotor::TransparencyMotor(int address, Adafruit_PWMServoDriver pwm, int interval)
    : Motor(address, pwm, interval) {}

void TransparencyMotor::update() {
    if((millis() - _last_update) > _interval && _current_angle != _target_angle) {
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

int generateRandomBetween(int a, int b) {
    int random_integer = rand() % (b+1);
    return random_integer + a;
}
