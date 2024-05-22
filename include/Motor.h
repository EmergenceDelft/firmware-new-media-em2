/*
 * Lock.h - Library managing an Ultrasonic Sensor.
 */
#ifndef Motor_h
#define Motor_h

#include <main.h>

class Motor
{
public:
  Motor(String id, int address, Adafruit_PWMServoDriver pwm);
  void setAngle(int degrees);
  void setup(String id);

private:
  String _id;
  int _address;
  Adafruit_PWMServoDriver _pwm;
  long angleToPulseWidth(int angle);
};

#endif
