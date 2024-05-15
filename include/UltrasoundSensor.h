/*
 * Lock.h - Library managing an Ultrasonic Sensor.
 */
#ifndef UltrasoundSensor_h
#define UltrasoundSensor_h

#include <main.h>

class UltrasoundSensor
{
public:
  UltrasoundSensor();
  String getJsonSerializedReadings();
  void setup(String id);

private:
  String _id;
};

#endif
