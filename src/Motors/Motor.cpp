/*
 * Motor.cpp - Library managing an PWM motor driver PCA9685.
 */

#include "Motor.h"
#include <random>

bool _movingTowards180;

Motor::Motor(int address, Adafruit_PWMServoDriver pwm, int interval)
{
    _address = address;
    _pwm = pwm;
    _interval = interval; 
    _moving = true;
    _jitter = false;
    _increment = 0;
    _last_update = 0; //time millis
    _current_angle = 0; //angle which should be proparly synced with the actual pwm angle
    _target_angle = 0;
    _movingTowards180 = true;
     //where we'd like to go, this is updated from the json server message
     //how many angles at a time to increase (careful if more than 1)
}

int Motor::getAngle() {
    return _current_angle;
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

void Motor::setJitter(bool jitter){
    _jitter = jitter;
}

long Motor::angleToPulseWidth(int angle) {
    return map(angle, 0, 180, SERVO_MIN_PULSE_WIDTH, SERVO_MAX_PULSE_WIDTH);
}








