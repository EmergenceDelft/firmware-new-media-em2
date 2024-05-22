/*
 * UltrasoundSensor.h - Library managing an Ultrasonic Sensor.
 */
#ifndef UltrasoundSensor_h
#define UltrasoundSensor_h

#include <main.h>

class UltrasoundSensor
{
public:
  UltrasoundSensor(String id);
  String getJsonSerializedReadings();

private:
  String _id;
};

#endif
