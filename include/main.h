/*
 * main.h - defaults for all classes
 */

#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <SRF05.h>
#include <Adafruit_PWMServoDriver.h> 


#define ULTRASONIC_TRIGGER_PIN 4
#define ULTRASONIC_ECHO_PIN 16
#define CORE_0 0
#define SERVO_PWM_FREQUENCY 50.0f
#define SERVO_MIN_PULSE_WIDTH 125 // Minimum pulse width for the servo
#define SERVO_MAX_PULSE_WIDTH 550 // Maximum pulse width for the servo


enum SensorType {
    ULTRASOUND,
    LOUDNESS,
    CAPACITIVE
};

using namespace websockets;

#endif
