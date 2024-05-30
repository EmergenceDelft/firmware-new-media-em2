/*
 * Lock.h - Library managing an Ultrasonic Sensor.
 */
#ifndef Motor_h
#define Motor_h

#include <main.h>

class Motor
{
public:
  Motor(int address, Adafruit_PWMServoDriver pwm, int interval);
  void update();
  void setTargetAngle(int angle);
  void setInterval(int interval);

private:

  String _id;
  int _address;
  Adafruit_PWMServoDriver _pwm;
  int _current_angle;
  int _target_angle;
  int _increment;
  int _interval;
  unsigned long _last_update; // last update of position
  long angleToPulseWidth(int angle);
  void setAngle(int degrees);
};

#endif
