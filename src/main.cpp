#include <main.h>
#include <UltrasoundSensor.h>

const char* ssid = "NPRouter"; //Enter SSID
const char* password = "keepitquantum"; //Enter Password
const char* websockets_server = "ws://192.168.4.2:3000/echo"; //server adress and port

WebsocketsClient client;
TaskHandle_t pollingTaskHandle;
UltrasoundSensor ultrasoundSensor;


void onMessageCallback(WebsocketsMessage message) {
    JsonDocument jsonMessage;
    deserializeJson(jsonMessage, message.data());

    String msgType = jsonMessage["type"];
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

    JsonDocument arrayDoc;
    JsonArray sensorArray = arrayDoc.to<JsonArray>();

    sensorArray.add("ULTRASOUND");

    doc["sensors"] = sensorArray;

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

    WiFi.begin(ssid, password);
    for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
    }
    
    Serial.print("Connected to wifi");

    client.onMessage(onMessageCallback);
    client.onEvent(onEventsCallback);
    client.connect(websockets_server);
    
    //Send hello message on connection.
    client.send(getHelloMessage());

    //Setup sensors
    ultrasoundSensor.setup(WiFi.macAddress() + ":ULTRASOUND");


    //All polling code is executed on a different thread to keep the main thread open for reading sensor data.
    // xTaskCreatePinnedToCore(pollingTask, "Sensor task", 10000, NULL, 1, &pollingTaskHandle, CORE_0);         
}

void loop() {
    client.poll();
    client.send(ultrasoundSensor.getJsonSerializedReadings());
    delay(1000);
}
