/*
 * Motor.h - Library managing an PWM motor driver PCA9685.
 */
#ifndef Motor_h
#define Motor_h

#include <main.h>

class Motor
{
public:
    Motor(int address, Adafruit_PWMServoDriver pwm, int interval);
    virtual void update() = 0;  // Pure virtual function  
    void setTargetAngle(int angle);
    void setInterval(int interval);
    void setMovement(bool move);
    void setJitter(bool jitter);
    int getAngle();
    void setAngle(int degrees);

protected:

    String _id;
    int _address;
    Adafruit_PWMServoDriver _pwm;
    bool _moving;
    bool _jitter;
    int _current_angle;
    int _target_angle;
    int _interval;

    //determines if the motor is moving towards the left or the right
    //TODO: this only applies to the ColourMotor, it should be moved there
    bool _movingTowards180;
    unsigned long _last_update; // last update of position, this is for timing
    
    //how to translate angle we're inputting into a pulse width the servo can understand
    long angleToPulseWidth(int angle); 
};

#endif
