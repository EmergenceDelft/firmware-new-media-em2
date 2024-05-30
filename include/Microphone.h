/*
 * Microphone.h - Library managing an Ultrasonic Sensor.
 */
#ifndef Microphone_h
#define Microphone_h

#include <main.h>

class Microphone
{
public:
  Microphone(String id, int AMP_PIN);
  String getJsonSerializedReadings();
  unsigned long measureAnalog();

private:
  String _id;
  int _AMP_PIN;
  //int _sample_window = 50;
//   unsigned int _latest_value = 0;
};

#endif
