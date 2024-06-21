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

int Microphone::measureAnalog(int samples)
{
    int signalAvg = 0, signalMax = 0, signalMin = 65530, k;
    for (int i = 0; i < samples; i++)
    {
        k = analogRead(_AMP_PIN);
        if (k < signalMin){
            signalMin = k;
        }
        if (k > signalMax){
            signalMax = k;
        }
    }

    return signalMax - signalMin;
}
