/*
 * Microphone.cpp - Library for Microphone MAX4466.
 */

#include "Microphone.h"
#include <algorithm>

Microphone::Microphone(String id, int AMP_PIN)
{
    _id = id;
    _AMP_PIN = AMP_PIN;
}

String Microphone::getJsonSerializedReadings() {
    JsonDocument doc;

    float sensorValue = measureAnalog();
    _latest = sensorValue;
    Serial.println(sensorValue);

    doc["type"] = "sensor_reading";
    doc["sensor_type"] = "MICROPHONE";
    doc["sensor_id"] = _id;
    doc["value"] = sensorValue;

    String serializedDoc;

    serializeJson(doc, serializedDoc);
    return serializedDoc;
}

int Microphone::getLatest(){
    return _latest;
}

int Microphone::measureAnalog()
{
    int micSamples = 10;
    int signalAvg = 0, signalMax = 0, signalMin = 65530;
    for (int i = 0; i < micSamples; i++)
    {
        
        int k = analogRead(_AMP_PIN);
        signalMin = min(signalMin, k);
        signalMax = max(signalMax, k);
        signalAvg += k;
    }

    return signalMax - signalMin;
}
