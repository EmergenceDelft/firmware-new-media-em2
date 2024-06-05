/*
 * UltrasoundSensor.cpp - Library for Ultrasonic Sensor.
 */

#include "UltrasoundSensor.h"

UltrasoundSensor::UltrasoundSensor(String id, uint8_t triggerEchoPin)
    : sensor(triggerEchoPin, triggerEchoPin)
{
    _id = id;
}

String UltrasoundSensor::getJsonSerializedReadings() {
    JsonDocument doc;

    doc["type"] = "sensor_reading";
    doc["sensor_type"] = "ULTRASOUND";
    doc["sensor_id"] = _id;
    doc["value"] = sensor.ping_cm();

    String serializedDoc;
    serializeJson(doc, serializedDoc);
    return serializedDoc;
}
