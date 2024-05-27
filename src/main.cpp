#include <main.h>
#include <UltrasoundSensor.h>
#include <Motor.h>

WebsocketsClient client;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
UltrasoundSensor ultrasoundSensor(WiFi.macAddress() + "::ULTRASOUND");


void onMessageCallback(WebsocketsMessage message) {
    Serial.println(message.data());
    JsonDocument jsonMessage;
    deserializeJson(jsonMessage, message.data());

    if(jsonMessage["type"] == "motor_commands") {
        JsonArray motorArray = jsonMessage["motors"];
        for(JsonObject motorJson : motorArray) {
            int address = motorJson["motor_address"];
            int angle = motorJson["angle"];

            Motor motor(address, pwm);
            motor.setAngle(angle);
        }
    }
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


void setup() {
    Serial.begin(115200);

    WiFi.begin(SSID, PASSWORD);
    for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
    }
    
    Serial.print("Connected to wifi");

    client.onMessage(onMessageCallback);
    client.onEvent(onEventsCallback);
    client.connect(CONNECTION_STRING);
    
    //Send hello message on connection.
    client.send(getHelloMessage());

    //Setup pwm
    pwm.begin();
    pwm.setPWMFreq(SERVO_PWM_FREQUENCY);       
}

void loop() {
    client.poll();
    Serial.println(ultrasoundSensor.getJsonSerializedReadings());
    client.send(ultrasoundSensor.getJsonSerializedReadings());
    delay(1000);
}
