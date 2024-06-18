/*
 * Motor.cpp - Library managing an PWM motor driver PCA9685.
 */

#include "Motor.h"
#include <random>

bool _movingTowards120;

Motor::Motor(int address, Adafruit_PWMServoDriver pwm, int interval, bool isColor)
{

    //address 0,1,2,3,4,5,6,7,8,9
    _address = address;
    
    //corresponding to voxels 0,0,1,1,2,2,3,3,4,4
    _voxel = _address / 2 ;

    _id = WiFi.macAddress() + "::VOXEL_" + _voxel + "::MOTOR_" + (_address % 2);

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

void Motor::setMovement(String move){
    _movement_type = move;
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
    doc["value"] = (_movement_type.equals("MANUAL")) ? _target_angle : _current_angle;
    doc["movement"] = _movement_type;

    String serializedDoc;

    serializeJson(doc, serializedDoc);
    return serializedDoc;
}

void Motor::update() {

    if((millis() - _last_update) > _interval){
        if(_movement_type.equals("AUTO")){
            updateAuto();
        }
        if(_movement_type.equals("MANUAL")){
            updateManual();
        }
        if(_movement_type.equals("AUTOJITTER")){
            updateAutoJitter();
        }
        _last_update = millis();
    }
}

void Motor::updateManual() {
     if(_current_angle != _target_angle) {
        _current_angle = _target_angle;
        setAngle(_current_angle);
    }
}


void Motor::updateAuto() {

    //seperate this into auto and auto jitter
        if(_current_angle >= 120) {
            _increment = -1;
        }
        if(_current_angle <=0) {
            _increment = 1;
        }
        _current_angle += _increment;
        setAngle(_current_angle);
}

int Motor::generateRandomBetween(int a, int b) {
    int random_integer = rand() % (b+1);
    return random_integer + a;
}

void Motor::updateAutoJitter() {
    int random_nr = generateRandomBetween(-2,5);

    if(_current_angle >= 120){
        _movingTowards120 = false;
    }
    if(_current_angle <= 0){
        _movingTowards120 = true;
    }

    if(_movingTowards120) {
        _increment = random_nr;
    }else{
        _increment = -random_nr;
    }

    _current_angle += _increment;
    setAngle(_current_angle);
}








