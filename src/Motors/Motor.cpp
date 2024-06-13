/*
 * Motor.cpp - Library managing an PWM motor driver PCA9685.
 */

#include "Motor.h"
#include <random>

bool _movingTowards180;

Motor::Motor(int address, Adafruit_PWMServoDriver pwm, int interval, bool isColor)
{

    //address 0,1,2,3,4,5,6,7,8,9
    _address = address;
    
    //corresponding to voxels 0,0,1,1,2,2,3,3,4,4
    _voxel = _address / 2 ;

    _id = WiFi.macAddress() + "::VOXEL_" + _voxel + "::MOTOR_" + (_address % 2);
    
    _movement = "AUTO";

    _pwm = pwm;
    _interval = interval; 
    _last_update = 0; //time millis
    _current_angle = 0; //angle which should be proparly synced with the actual pwm angle
    _target_angle = 0;
    _is_color_filter = isColor;
     //where we'd like to go, this is updated from the json server message
     //how many angles at a time to increase (careful if more than 1)
}

void Motor::setAngle(int angle) {
   long pulse_width = angleToPulseWidth(angle);
   _pwm.setPWM(_address, 0, pulse_width);
}

void Motor::setTargetAngle(int angle){
    _target_angle = angle;
}

void Motor::setMovement(bool move){
    _moving = move;
}

void Motor::setInterval(int interval){
    _interval = interval;
}

void Motor::setJitter(bool jitter){
    _jitter = jitter;
}

long Motor::angleToPulseWidth(int angle) {
    return map(angle, 0, 180, SERVO_MIN_PULSE_WIDTH, SERVO_MAX_PULSE_WIDTH);
}

String Motor::getJsonAngle() {
    JsonDocument doc;


    doc["type"] = "motor_angle";
    doc["id"] = _id; 
    doc["value"] = _current_angle;
    doc["movement"] = _movement;

    String serializedDoc;

    serializeJson(doc, serializedDoc);
    return serializedDoc;
}

void Motor::update() {

    if (_is_color_filter) {
        updateColorMotor();
    }else{
        updateTransparencyMotor();
    }
}

void Motor::updateTransparencyMotor() {
     if((millis() - _last_update) > _interval && _current_angle != _target_angle) {

        //set increment according to whether we need to increase or decrease current_angle
        if(_current_angle < _target_angle){
            _increment = 10;
        }else{
            _increment = -10;
        }

        _last_update = millis();
        _current_angle += _increment;
        setAngle(_current_angle);

    }
}
int generateRandomBetween(int a, int b) {
    int random_integer = rand() % (b+1);
    return random_integer + a;
}

void Motor::updateColorMotor() {

    if((millis() - _last_update) > _interval && _moving){

        if(_current_angle >= 180){
            _movingTowards180 = false;
        }
        if(_current_angle <= 0){
            _movingTowards180 = true;
        }

        if(_jitter) {
            int random_nr = generateRandomBetween(-2,5);
            if(_movingTowards180) {
                _increment = random_nr;
            }else {
                _increment = -random_nr;
            }
        }else{
            if(_movingTowards180) {
                _increment = 1;
            }else {
                _increment = -1;
            }
        }

        _last_update = millis();
        _current_angle += _increment;
        setAngle(_current_angle);

    }
}





