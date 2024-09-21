// ColourMotor.cpp
#include "ColourMotor.h"
#include "Motor.h"

ColourMotor::ColourMotor(int address, Adafruit_PWMServoDriver pwm, int interval)
    : Motor(address, pwm, interval) {}

void ColourMotor::setMinAngle(int minAngle){
    MIN_ANGLE = minAngle;
    Serial.print("Setting ColourMotor min angle to: ");
    Serial.println(MIN_ANGLE);
}
void ColourMotor::setMaxAngle(int maxAngle){
    MAX_ANGLE = maxAngle;
    Serial.print("Setting ColourMotor max angle to: ");
    Serial.println(MAX_ANGLE);
}

void ColourMotor::setRotationIncrement(int rotationIncrement){
    ROTATION_INCREMENT = rotationIncrement;
    Serial.print("Setting ColourMotor rotation increment to: ");
    Serial.println(ROTATION_INCREMENT);
}
void ColourMotor::setSnapIncrement(int snapIncrement){
    SNAP_INCREMENT = snapIncrement;
    Serial.print("Setting ColourMotor snap increment to: ");
    Serial.println(SNAP_INCREMENT);
}
void ColourMotor::setMinJitterIncrement(int minJitterIncrement){
    MIN_JITTER_INCREMENT = minJitterIncrement;
    Serial.print("Setting ColourMotor minJitterIncrement to: ");
    Serial.println(MIN_JITTER_INCREMENT);
}
void ColourMotor::setMaxJitterIncrement(int maxJitterIncrement){
    MAX_JITTER_INCREMENT = maxJitterIncrement;
    Serial.print("Setting ColourMotor maxJitterIncrement to: ");
    Serial.println(MAX_JITTER_INCREMENT);
}



void ColourMotor::update() {
    if(millis() - _last_update > _interval){
        //colour motor either moves continuosly or moves to a specific angle and stops
        if(_moving) {
            ColourMotor::moveContinuously();
        }else{
            ColourMotor::moveToAngle();
        }
    }
}

void ColourMotor::moveContinuously() {
    //named moving towards 180 because that was the original max angle
    if(_current_angle >= MAX_ANGLE){
        _movingTowards180 = false;
    }
    if(_current_angle <= MIN_ANGLE){
        _movingTowards180 = true;
    }
    int increment;
    //maybe a little bit ugly if elses, you could move the moving towards 180 if else outside?
    if(_jitter) {
        int random_nr = ColourMotor::generateRandomBetween(MIN_JITTER_INCREMENT,MAX_JITTER_INCREMENT);
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

void ColourMotor::moveToAngle() {
    if(_current_angle != _target_angle) {

        int difference = _target_angle - _current_angle;
        int increment;
        //set increment according to whether we need to increase or decrease current_angle
        //when difference is small enough than go directly to where we need to go, prevent overshooting target
        if (difference > 0) {
            increment = (difference < SNAP_INCREMENT) ? difference : SNAP_INCREMENT;
        } else {
            increment = (difference > -SNAP_INCREMENT) ? difference : -SNAP_INCREMENT;
        }

        _current_angle += increment;
        setAngle(_current_angle);
    }
}



int ColourMotor::generateRandomBetween(int a, int b) {
    int random_integer = rand() % (b+1);
    return random_integer + a;
}
