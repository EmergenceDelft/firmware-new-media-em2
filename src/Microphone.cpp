/*
 * Microphone.cpp - Library for Microphone MAX4466.
 */

#include "Microphone.h"

Microphone::Microphone(String id, int AMP_PIN)
{
    _id = id;
    _AMP_PIN = AMP_PIN;
}

String Microphone::getJsonSerializedReadings() {
    JsonDocument doc;

    int sensorValue = measureAnalog();

    doc["type"] = "sensor_reading";
    doc["sensor_type"] = "MICROPHONE";
    doc["sensor_id"] = _id;
    doc["value"] = sensorValue;

    String serializedDoc;

    serializeJson(doc, serializedDoc);
    return serializedDoc;
}

int Microphone::measureAnalog()
{
    int micSamples = 100;
    int signalAvg = 0, signalMax = 0, signalMin = 50024;
    for (int i = 0; i < micSamples; i++)
    {
        int k = analogRead(_AMP_PIN);
        
        signalMin = min(signalMin, k);
        signalMax = max(signalMax, k);
        signalAvg += k;
    }

    signalAvg /= micSamples;

    return signalMax - signalMin;
}