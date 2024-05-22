/*
 * Motor.cpp - Library for Ultrasonic Sensor.
 */

#include "Motor.h"

Motor::Motor(String id, int address, Adafruit_PWMServoDriver pwm)
{
    _id = id;
    _pwm = pwm;
    _address = address;
}

void Motor::setAngle(int angle) {
   long pulse_width = angleToPulseWidth(angle);
   _pwm.setPWM(_address, 0, pulse_width);
}

long Motor::angleToPulseWidth(int angle) {
    return map(angle, 0, 180, SERVO_MIN_PULSE_WIDTH, SERVO_MAX_PULSE_WIDTH);
}



