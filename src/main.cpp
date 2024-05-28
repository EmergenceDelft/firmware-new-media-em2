#include <main.h>
#include <UltrasoundSensor.h>
#include <Motor.h>
#include <cmath> 

WebsocketsClient client;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
UltrasoundSensor ultrasoundSensor(WiFi.macAddress() + "::ULTRASOUND");


void onMessageCallback(WebsocketsMessage message) {
    Serial.println(message.data());
    JsonDocument jsonMessage;
    deserializeJson(jsonMessage, message.data());

    if(jsonMessage["type"] == "motor_commands") {
        JsonArray motorArray = jsonMessage["motors"];

        int numMotors = motorArray.size();

        std::vector<Motor> motors;       // Use a vector for motors
        std::vector<int> targetAngles;   // Use a vector for target angles
        std::vector<int> currentAngles;

        motors.reserve(numMotors);       // Reserve space for efficiency
        targetAngles.reserve(numMotors); // Reserve space for efficiency
        currentAngles.reserve(numMotors);

        int stepSize = 1;
        int stepDelay = 10;
        for(JsonObject motorJson : motorArray) {
            int address = motorJson["motor_address"];
            int angle = motorJson["angle"];
            if (motorJson.containsKey("stepDelay")) {
                stepDelay = motorJson["stepDelay"]; // Extract stepDelay from JSON
            }

            motors.emplace_back(address, pwm); // passes addres and pwm, and vector will create a new motor at end automatically
            targetAngles.push_back(angle);     // Add target angle to the vector
            currentAngles.push_back(abs(angle - 90)); // Initialize currentAngles
            // !!!!!!!!
            //this will need fixing, once we have other angles that are not just 0 or 90
            
        }

        

        bool movementCompleted = false;
        while (!movementCompleted) {
            movementCompleted = true;
            for (int i = 0; i < numMotors; i++) {
                if (currentAngles[i] < targetAngles[i]) {
                    Serial.println(currentAngles[i]);
                    currentAngles[i] += stepSize;
                    if (currentAngles[i] > targetAngles[i]) currentAngles[i] = targetAngles[i]; //this is how we stop
                    movementCompleted = false;
                } else if (currentAngles[i] > targetAngles[i]) {
                    currentAngles[i] -= stepSize;
                    if (currentAngles[i] < targetAngles[i]) currentAngles[i] = targetAngles[i]; //this is how we stop
                    movementCompleted = false;
                }
                motors[i].setAngle(currentAngles[i]);
                delay(stepDelay); 

            }   
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
    String sensor_reading = ultrasoundSensor.getJsonSerializedReadings();
    Serial.println("------------");
    Serial.println(sensor_reading);
    client.send(sensor_reading);
    //delay(1000);
}
