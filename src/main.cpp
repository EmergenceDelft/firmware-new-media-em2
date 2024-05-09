#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <SRF05.h>

#define TRIGGER_PIN 4
#define ECHO_PIN 16


const char* ssid = "VlietlaanVlinders"; //Enter SSID
const char* password = "Braktouba45"; //Enter Password
const char* websockets_server = "ws://192.168.2.5:3000/echo"; //server adress and port

using namespace websockets;
WebsocketsClient client;
SRF05 SRF(TRIGGER_PIN, ECHO_PIN);
TaskHandle_t pollingTaskHandle;



void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());
}
 
void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connection Opened");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connection Closed");
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

String getHelloMessage() {
    JsonDocument doc;

    doc["type"] = "hello";
    doc["mac_address"] = WiFi.macAddress();

    String serializedDoc;

    serializeJson(doc, serializedDoc);
    return serializedDoc;
}

void pollingTask( void * pvParameters ){
    for(;;) {
        client.poll();
    }
}

void setup() {
    Serial.begin(115200);
    // Connect to wifi
    WiFi.begin(ssid, password);

    // Wait some time to connect to wifi
    for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
    }
    
    Serial.print("Connected to wifi");

    // Setup Callbacks
    client.onMessage(onMessageCallback);
    client.onEvent(onEventsCallback);
    
    // Connect to server
    client.connect(websockets_server);

    // Send a message
    client.send(getHelloMessage());
    // Send a ping
    client.ping();

    //All polling code is executed on a different thread to keep the main thread open for reading sensor data.
    xTaskCreatePinnedToCore(
             pollingTask,  /* Task function. */
             "Sensor task",    /* name of task. */
             10000,      /* Stack size of task */
             NULL,       /* parameter of the task */
             1,          /* priority of the task */
             &pollingTaskHandle,     /* Task handle to keep track of created task */
             0);         /* pin task to core 0 */
}

void loop() {
    Serial.println(SRF.getCentimeter(), 1);
    delay(1000);
}
