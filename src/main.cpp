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

int numMotors = 2; //this should get updated onMessageCallback but have it as something just in case?
int interval = 10; //default interval for updating motors
unsigned long lastUpdate = 0;
unsigned long lastUpdateClient = 0;
bool jitter = true;

void onMessageCallback(WebsocketsMessage message) {
    Serial.println(message.data());
    JsonDocument jsonMessage;
    deserializeJson(jsonMessage, message.data());

    if(jsonMessage["type"] == "motor_commands") {
        JsonArray motorArray = jsonMessage["motors"];
        
        numMotors = motorArray.size();
        for(JsonObject motorJson : motorArray) {
            int address = motorJson["motor_address"];
            // int interval = 10;
            if (motorJson.containsKey("angle")) {
                int angle = motorJson["angle"];
                motors[address].setTargetAngle(angle);
            }
            if (motorJson.containsKey("movement")) {
                bool move = motorJson["movement"];
                motors[address].setMovement(move);
            }
            if (motorJson.containsKey("stepDelay")) {
                interval = motorJson["stepDelay"]; // Extract stepDelay from JSON
                
            }
            motors[address].setInterval(interval);
        }
    }
}
 
void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connection Opened");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connection Closed");
        motors[1].setMovement(false);
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
    
    Serial.println("Connected to wifi");

    client.onMessage(onMessageCallback);
    client.onEvent(onEventsCallback);
    client.connect(CONNECTION_STRING);
    
    /* Send hello message on connection. */
    client.send(getHelloMessage()); 
    delay(1000);

    /* Set the I2C pins to the pins configured for the custom hardware */
    Wire.begin(SDA_PIN, SCL_PIN);
    pwm.begin();
    pwm.setPWMFreq(SERVO_PWM_FREQUENCY);
    Serial.println("done with pwm");

    //0,1,2,3,4,5,6,7,8,9
    //0,0,1,1,2,2,3,3,4,4
    //0,1,0,1,0,1,0,1,0,1

    motors.reserve(numMotors);
    for(int i = 0; i < numMotors/2; i+=2){
        //false for transparency motor and true for color motor
        
        motors.emplace_back(i, pwm, interval, false);
        motors.emplace_back(i+1, pwm, interval, true);

    }
}

void updateSensors() {
    if((millis() - lastUpdate) > SENSOR_INTERVAL) {
        String sensor_reading = ultrasoundSensor.getJsonSerializedReadings();
        client.send(sensor_reading);
        Serial.println(sensor_reading);
        String microphone_reading = microphone.getJsonSerializedReadings();
        client.send(microphone_reading);
        lastUpdate = millis();


        jitter = microphone.getLatest() > 500;
    }
}

void updateClientConnection() {
    if((millis() - lastUpdateClient) > CLIENT_INTERVAL){
        client.poll();
        lastUpdateClient = millis();
    }
}


void loop() {

    updateSensors();
    updateClientConnection();
    
    for(int i = 0; i < numMotors; i++){
        motors[i].setJitter(jitter);
        motors[i].update();
        String motor_reading = motors[i].getJsonAngle();
        client.send(motor_reading);
        Serial.println(motor_reading);
    }
}

