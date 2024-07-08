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

  //number of samples, 
  //the larger this number the longer the microphone blocks everything but the more accurate it is
  int measureAnalog(int samples);

private:
  String _id;
  int _AMP_PIN;
  int _latest;
};

#endif
