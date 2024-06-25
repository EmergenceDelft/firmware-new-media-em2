// ColorMotor.cpp
#include "ColorMotor.h"
#include "Motor.h"

ColorMotor::ColorMotor(int address, Adafruit_PWMServoDriver pwm, int interval)
    : Motor(address, pwm, interval) {}

void ColorMotor::setMinAngle(int minAngle){
    MIN_ANGLE = minAngle;
    Serial.print("Setting ColorMotor min angle to: ");
    Serial.println(MIN_ANGLE);
}
void ColorMotor::setMaxAngle(int maxAngle){
    MAX_ANGLE = maxAngle;
    Serial.print("Setting ColorMotor max angle to: ");
    Serial.println(MAX_ANGLE);
}

void ColorMotor::setRotationIncrement(int rotationIncrement){
    ROTATION_INCREMENT = rotationIncrement;
    Serial.print("Setting ColorMotor rotation increment to: ");
    Serial.println(ROTATION_INCREMENT);
}
void ColorMotor::setSnapIncrement(int snapIncrement){
    SNAP_INCREMENT = snapIncrement;
    Serial.print("Setting ColorMotor snap increment to: ");
    Serial.println(SNAP_INCREMENT);
}
void ColorMotor::setMinJitterIncrement(int minJitterIncrement){
    MIN_JITTER_INCREMENT = minJitterIncrement;
    Serial.print("Setting ColorMotor minJitterIncrement to: ");
    Serial.println(MIN_JITTER_INCREMENT);
}
void ColorMotor::setMaxJitterIncrement(int maxJitterIncrement){
    MAX_JITTER_INCREMENT = maxJitterIncrement;
    Serial.print("Setting ColorMotor maxJitterIncrement to: ");
    Serial.println(MAX_JITTER_INCREMENT);
}



void ColorMotor::update() {
    if(millis() - _last_update > _interval){
        if(_moving) {
            ColorMotor::moveContinously();
        }else{
            ColorMotor::moveToAngle();
        }
    }
}

void ColorMotor::moveContinously() {
    if(_current_angle >= MAX_ANGLE){
        _movingTowards180 = false;
    }
    if(_current_angle <= MIN_ANGLE){
        _movingTowards180 = true;
    }
    int increment;
    if(_jitter) {
        int random_nr = ColorMotor::generateRandomBetween(MIN_JITTER_INCREMENT,MAX_JITTER_INCREMENT);
        if(_movingTowards180) {
            increment = random_nr;
        }else {
            increment = -random_nr;
        }
    }else{
        if(_movingTowards180) {
            increment = ROTATION_INCREMENT;
        }else {
            increment = -ROTATION_INCREMENT;
        }
    }
    _current_angle += increment;
    setAngle(_current_angle);

}

void ColorMotor::moveToAngle() {
    if(_current_angle != _target_angle) {

        int difference = _target_angle - _current_angle;
        int increment;
        //set increment according to whether we need to increase or decrease current_angle
        if (difference > 0) {
            increment = (difference < SNAP_INCREMENT) ? difference : SNAP_INCREMENT;
        } else {
            increment = (difference > -SNAP_INCREMENT) ? difference : -SNAP_INCREMENT;
        }

        _current_angle += increment;
        setAngle(_current_angle);
    }
}



int ColorMotor::generateRandomBetween(int a, int b) {
    int random_integer = rand() % (b+1);
    return random_integer + a;
}
