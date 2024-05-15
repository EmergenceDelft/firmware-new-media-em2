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

#define ULTRASONIC_TRIGGER_PIN 4
#define ULTRASONIC_ECHO_PIN 16
#define CORE_0 0

enum SensorType {
    ULTRASOUND,
    LOUDNESS,
    CAPACITIVE
};

using namespace websockets;

#endif
