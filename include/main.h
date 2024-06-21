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
#define BLOCKING_STATE_INTERVAL 2000
int AUDIO_SAMPLE_INTERVAL = 500;
int AUDIO_SAMPLE_AMOUNT = 100;

/* Sensor tresholds*/
int MIN_AUDIO_JITTER_THRESHOLD = 500;
int MAX_AUDIO_JITTER_THRESHOLD = 5000;
int MIN_PROXIMITY_THRESHOLD = 1;
int MAX_PROXIMITY_THRESHOLD = 100;


/* Ultrasound sensor  */
#define ULTRASOUND_TRIGGER_ECHO_PIN 22

/* Microphone */
#define NOISE_INPUT_PIN 34

/* Servo controller */
#define SCL_PIN 14
#define SDA_PIN 27
#define SERVO_PWM_FREQUENCY 50.0f
#define SERVO_MIN_PULSE_WIDTH 100 
#define SERVO_MAX_PULSE_WIDTH 525 

/* Router credentials */
#define SSID "NMPRouter"
#define PASSWORD "keepitquantum"
#define CONNECTION_STRING "ws://192.168.12.241:5050/" 

enum SensorType {
    ULTRASOUND,
    LOUDNESS,
    CAPACITIVE
};

String getJsonMeasured();
String getJsonUnmeasured();

using namespace websockets;

#endif
