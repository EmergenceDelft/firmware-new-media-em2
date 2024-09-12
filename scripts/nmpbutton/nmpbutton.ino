#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <ArduinoJson.h>
WebsocketsClient client;

void onMessageCallback(WebsocketsMessage message) {
    Serial.println("hello");

    Serial.println(message.data()); 
}

void setup() {
    Serial.begin(115200);

    WiFi.begin(SSID, PASSWORD);

    //wait for connections, since it's not usually instant
    //also if you turn all the esps on and off the server takes a bit longer
    for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("Connected to wifi");

    client.onMessage(onMessageCallback);
    client.connect(String(CONNECTION_STRING) + "?mac_address=" + WiFi.macAddress());

    /* Send hello message on connection. */
    client.send(getHelloMessage());
}

void loop() {

}


/*
    Message utility functions.
*/

String getHelloMessage() {
    JsonDocument doc;

    doc["type"] = "hello";
    doc["macAddress"] = WiFi.macAddress();

    JsonDocument arrayDoc;
    JsonArray sensorArray = arrayDoc.to<JsonArray>();

    sensorArray.add("BUTTON");

    //but then you'd have to somehow query your existing sensor or reflash it if you have different sensorArrays
    doc["sensors"] = sensorArray;

    String serializedDoc;
    serializeJson(doc, serializedDoc);
    return serializedDoc;
}
