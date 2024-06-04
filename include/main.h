/*
 * main.h - defaults for all classes
 */

#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <NewPing.h>
#include <Adafruit_PWMServoDriver.h>

/* Ultrasonic sensor  */
#define ULTRASONIC_TRIGGER_ECHO_PIN 22

/* Microphone */
#define NOISE_INPUT_PIN 15
#define BACKGROUND_AMPLITUDE 2500

/* Servo controller */
#define SERVO_PWM_FREQUENCY 50.0f
#define SERVO_MIN_PULSE_WIDTH 125 
#define SERVO_MAX_PULSE_WIDTH 550 

/* Router credentials */
#define SSID "NPRouter"
#define PASSWORD "keepitquantum"
#define CONNECTION_STRING "ws://192.168.4.3:3000/echo" 

enum SensorType {
    ULTRASOUND,
    LOUDNESS,
    CAPACITIVE
};

using namespace websockets;

#endif
