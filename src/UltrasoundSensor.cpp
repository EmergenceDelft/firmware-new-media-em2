/*
 * UltrasoundSensor.cpp - Library for Ultrasonic Sensor.
 */

#include "UltrasoundSensor.h"

UltrasoundSensor::UltrasoundSensor(String id, uint8_t triggerEchoPin)
    : sensor(triggerEchoPin, triggerEchoPin)
{
    _id = id;
}

unsigned long UltrasoundSensor::getMedianValue(int pings){
    return sensor.ping_median(pings, 400);
}

unsigned long UltrasoundSensor::getValue(){
    return sensor.ping_cm(450);
}