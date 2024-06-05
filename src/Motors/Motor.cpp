/*
 * Motor.cpp - Library managing an PWM motor driver PCA9685.
 */

#include "Motor.h"


Motor::Motor(int address, Adafruit_PWMServoDriver pwm, int interval, bool isColor)
{
    _address = address;
    _pwm = pwm;
    _interval = interval; 
    _last_update = 0; //time millis
    _current_angle = 0; //angle which should be proparly synced with the actual pwm angle
    _target_angle = 0;
    _is_color_filter = isColor;
     //where we'd like to go, this is updated from the json server message
     //how many angles at a time to increase (careful if more than 1)
}

void Motor::setAngle(int angle) {
   long pulse_width = angleToPulseWidth(angle);
   _pwm.setPWM(_address, 0, pulse_width);
}

void Motor::setTargetAngle(int angle){
    _target_angle = angle;
}

void Motor::setMovement(bool move){
    _moving = move;
}

void Motor::setInterval(int interval){
    _interval = interval;
}

long Motor::angleToPulseWidth(int angle) {
    return map(angle, 0, 180, SERVO_MIN_PULSE_WIDTH, SERVO_MAX_PULSE_WIDTH);
}

void Motor::update() {

    if (_is_color_filter) {
        updateColorMotor();
    }else{
        updateTransparencyMotor();
    }
}

void Motor::updateTransparencyMotor() {
     if((millis() - _last_update) > _interval && _current_angle != _target_angle) {

        //set increment according to whether we need to increase or decrease current_angle
        if(_current_angle < _target_angle){
            _increment = 1;
        }else{
            _increment = -1;
        }

        _last_update = millis();
        _current_angle += _increment;
        setAngle(_current_angle);

    }
}

void Motor::updateColorMotor() {

    if((millis() - _last_update) > _interval && _moving){
        if(_current_angle >= 180){
            _increment = -1;
        }
        if(_current_angle <= 0){
            _increment = 1;
        }

        _last_update = millis();
        _current_angle += _increment;
        setAngle(_current_angle);

    }
}



