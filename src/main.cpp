#include <main.h>
#include <UltrasoundSensor.h>
#include <Motor.h>
#include <Microphone.h>
#include <cmath> 

WebsocketsClient client;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
UltrasoundSensor ultrasoundSensor(WiFi.macAddress() + "::ULTRASOUND", ULTRASOUND_TRIGGER_ECHO_PIN);
Microphone microphone(WiFi.macAddress() + "::MICROPHONE", NOISE_INPUT_PIN);


std::vector<Motor> motors;       // Global vector for motors
std::vector<int> targetAngles;   // Global vector for target angles
int numMotors = 10; 
int interval = 15; 

void onMessageCallback(WebsocketsMessage message) {
    Serial.println(message.data());
    JsonDocument jsonMessage;
    deserializeJson(jsonMessage, message.data());

    if(jsonMessage["type"] == "motor_commands") {
        JsonArray motorArray = jsonMessage["motors"];
        
        numMotors = motorArray.size();
        for(JsonObject motorJson : motorArray) {
            int address = motorJson["motor_address"];
            int angle = motorJson["angle"];
            if (motorJson.containsKey("stepDelay")) {
                interval = motorJson["stepDelay"]; // Extract stepDelay from JSON
                motors[address].setInterval(interval);
            }
            motors[address].setTargetAngle(angle);
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
    sensorArray.add("MICROPHONE");

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
    
    /* Send hello message on connection. */
    client.send(getHelloMessage()); 

    //Setup pwm
    pwm.begin();
    pwm.setPWMFreq(SERVO_PWM_FREQUENCY);

    motors.reserve(numMotors);
    for(int i = 0; i < numMotors; i++){
        motors.emplace_back(i, pwm, interval);
    }
}

void loop() {
    client.poll();
    
    /* Get and send ultrasound sensor readings to server */
    String sensor_reading = ultrasoundSensor.getJsonSerializedReadings();
    Serial.println("------------");
    Serial.println(sensor_reading);
    client.send(sensor_reading);

    /* Get and send microphone sensor readings to server */
    String microphone_reading = microphone.getJsonSerializedReadings();
    Serial.println("-------------");
    Serial.println(microphone_reading);
    client.send(microphone_reading);

    for(int i = 0; i < numMotors; i++){
        motors[i].update();
    }
    delay(400);
}
