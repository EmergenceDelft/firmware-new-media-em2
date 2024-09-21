#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#define SSID "NMPRouter 2.0"
#define PASSWORD "keepitquantum"
#define CONNECTION_STRING "ws://192.168.0.101:5050/" 
#define BUTTON_PIN 22
#define LED_PIN 2

using namespace websockets;
WebsocketsClient client;

int BUTTON_DELAY = 5000;

void onMessageCallback(WebsocketsMessage message) {
    Serial.println("hello");

    Serial.println(message.data()); 
    JsonDocument jsonMessage;
    deserializeJson(jsonMessage, message.data());

    if(jsonMessage["type"] == "setup"){
        JsonObject content = jsonMessage["content"];
        BUTTON_DELAY = content["buttonDelay"];
    }
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

    
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if(digitalRead(BUTTON_PIN) == LOW) {
    Serial.println("Button pressed!");
    String str = getJsonButtonPressed();
    client.send(str);

    //Set led to ON
    digitalWrite(LED_BUILTIN, LOW);

    // Wait BUTTON_DELAY for next command
    delay(BUTTON_DELAY);

    //Set led to OFF
    digitalWrite(LED_BUILTIN, HIGH);
  }
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

String getJsonButtonPressed() {
    JsonDocument doc;

    doc["type"] = "button_pressed";
    doc["macAddress"] = WiFi.macAddress();

    String serializedDoc;
    serializeJson(doc, serializedDoc);
    return serializedDoc;
}
