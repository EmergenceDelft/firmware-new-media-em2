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
#include <Wire.h>

/* processing intervals  */
#define SENSOR_INTERVAL 100
#define CLIENT_INTERVAL 100

/* Ultrasound sensor  */
#define ULTRASOUND_TRIGGER_ECHO_PIN 22

/* Microphone */
#define NOISE_INPUT_PIN 34
#define BACKGROUND_AMPLITUDE 2500

/* Servo controller */
#define SCL_PIN 14
#define SDA_PIN 27
#define SERVO_PWM_FREQUENCY 50.0f
#define SERVO_MIN_PULSE_WIDTH 100 
#define SERVO_MAX_PULSE_WIDTH 525 

/* Router credentials */
#define SSID "NPRouter"
#define PASSWORD "keepitquantum"
#define CONNECTION_STRING "ws://192.168.4.5:3000/echo" 

enum SensorType {
    ULTRASOUND,
    LOUDNESS,
    CAPACITIVE
};

using namespace websockets;

#endif
