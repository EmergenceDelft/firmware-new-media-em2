/*
 * UltrasoundSensor.cpp - Library for Ultrasonic Sensor.
 */

#include "UltrasoundSensor.h"

SRF05 sensor(ULTRASONIC_TRIGGER_PIN, ULTRASONIC_ECHO_PIN);

UltrasoundSensor::UltrasoundSensor(String id)
{
    _id = id;

}

String UltrasoundSensor::getJsonSerializedReadings() {
    JsonDocument doc;

    doc["type"] = "sensor_reading";
    doc["sensor_type"] = "ULTRASOUND";
    doc["sensor_id"] = _id;
    doc["value"] = sensor.getMeter();

    String serializedDoc;

    serializeJson(doc, serializedDoc);
    return serializedDoc;
}




