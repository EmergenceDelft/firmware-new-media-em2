/*
 * Microphone.cpp - Library for Ultrasonic Sensor.
 */

#include "Microphone.h"
#include <algorithm>

#define BACKGROUND_AMPLTITUDE 0

//SRF05 sensor(ULTRASONIC_TRIGGER_PIN, ULTRASONIC_ECHO_PIN);

Microphone::Microphone(String id, int AMP_PIN)
{
    _id = id;
    _AMP_PIN = AMP_PIN;


}

String Microphone::getJsonSerializedReadings() {
    JsonDocument doc;

    doc["type"] = "sensor_reading";
    doc["sensor_type"] = "MICROPHONE";
    doc["sensor_id"] = _id;
    doc["value"] = measureAnalog()- BACKGROUND_AMPLTITUDE ;

    String serializedDoc;

    serializeJson(doc, serializedDoc);
    return serializedDoc;
}

unsigned long Microphone::measureAnalog()
{
    int micSamples = 2;
    long signalAvg = 0, signalMax = 0, signalMin = 50024;
    for (int i = 0; i < micSamples; i++)
    {
        long k = analogRead(_AMP_PIN);
        

        // signalMin = min(signalMin, k);
        // signalMax = max(signalMax, k);
        signalAvg += k;
    }
    // Serial.println("max min");
    // Serial.println(signalMax);
    // Serial.println(signalMin);


    signalAvg /= micSamples;
    //Serial.println("microphone signal avg");
    //Serial.println(signalAvg);
    return signalAvg;
}

// void update() {
//     unsigned long startMillis = millis(); // Start of sample window
//     int peakToPeak = 0;   // peak-to-peak level
//     int signalMax = 0;
//     int signalMin = 1024;

//   // collect data for 50 mS and then plot data
//   while (millis() - startMillis < sampleWindow)
//   {
//     sample = analogRead(AMP_PIN);
//     if (sample < 1024)  // toss out spurious readings
//     {
//       if (sample > signalMax)
//       {
//         signalMax = sample;  // save just the max levels
//       }
//       else if (sample < signalMin)
//       {
//         signalMin = sample;  // save just the min levels
//       }
//     }
//   }
//   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
//   Serial.println(peakToPeak);
//   //double volts = (peakToPeak * 5.0) / 1024;  // convert to volts
//   //Serial.println(volts);
// }




