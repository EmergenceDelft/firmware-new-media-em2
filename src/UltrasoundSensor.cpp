/*
 * UltrasoundSensor.cpp - Library for Ultrasonic Sensor.
 */

#include "UltrasoundSensor.h"

UltrasoundSensor::UltrasoundSensor(String id, uint8_t triggerEchoPin)
    : sensor(triggerEchoPin, triggerEchoPin)
{
    _id = id;
}

unsigned long UltrasoundSensor::getValue(){
    return sensor.ping_cm();
}
