/*
 * Microphone.h - Library for Microphone MAX4466.
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
};

#endif
