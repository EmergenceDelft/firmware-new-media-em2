/*
 * UltrasoundSensor.h - Library managing an ultrasound sensor HY-SRF05.
 */
#ifndef UltrasoundSensor_h
#define UltrasoundSensor_h

#include <main.h>

class UltrasoundSensor
{
public:
  UltrasoundSensor(String id, uint8_t triggerEchoPin);
  String getJsonSerializedReadings();

private:
  String _id;
  NewPing sensor;
};

#endif
