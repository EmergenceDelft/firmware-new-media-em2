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

    //ignore outside of signal min and signal max

    //IMPORTANT
    //this is not very well done
    //look into how to translate data from the microphone into proper loudness levels
    //internet suggested measuring the amplitude and measuring for as long as possible
    //but we don't want that so we are measuring for not very long (100)
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

    //amplitude is better than the avg (sources needed from sound/phsyics people)
    return signalMax - signalMin;
}
