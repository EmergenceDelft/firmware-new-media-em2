/*
 * Lock.h - Library managing an Ultrasonic Sensor.
 */
#ifndef Motor_h
#define Motor_h

#include <main.h>

class Motor
{
public:
  Motor(int address, Adafruit_PWMServoDriver pwm, int interval, bool isColor);
  void update();
  void setTargetAngle(int angle);
  void setInterval(int interval);
  void setMovement(bool angle);

private:

  String _id;
  int _address;
  Adafruit_PWMServoDriver _pwm;
  bool _is_color_filter;
  bool _moving;
  int _current_angle;
  int _target_angle;
  int _increment;
  int _interval;
  unsigned long _last_update; // last update of position
  long angleToPulseWidth(int angle);
  void setAngle(int degrees);
  void updateTransparencyMotor();
  void updateColorMotor();
};

#endif
