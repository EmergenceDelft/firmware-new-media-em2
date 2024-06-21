#include <main.h>
#include <UltrasoundSensor.h>
#include <Motor.h>
#include <ColorMotor.h>
#include <TransparencyMotor.h>
#include <Voxel.h>
#include <Microphone.h>
#include <cmath> 

WebsocketsClient client;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
UltrasoundSensor ultrasoundSensor(WiFi.macAddress() + "::ULTRASOUND", ULTRASOUND_TRIGGER_ECHO_PIN);
Microphone microphone(WiFi.macAddress() + "::MICROPHONE", NOISE_INPUT_PIN);


std::vector<Voxel*> voxels;       // Global vector for motors
std::vector<int> targetAngles;   // Global vector for target angles


int numVoxels = 2;
int numMotors = 4; //this should get updated onMessageCallback but have it as something just in case?
int interval = 10; //default interval for updating motors
unsigned long lastUpdate = 0;
unsigned long lastUpdateClient = 0;
unsigned long lastUpdateState = 0;
unsigned long lastUpdateMicrophone = 0;
unsigned long lastUpdateProximity = 0;
unsigned long lastUpdateMotors = 0;
bool jitter = true;
bool proximityNear = false;

int AUDIO_SAMPLE_INTERVAL = 500;
int AUDIO_SAMPLE_AMOUNT = 100;
int PROXIMITY_SAMPLE_INTERVAL = 300;
int PROXIMITY_SAMPLE_AMOUNT = 10;

int MOTOR_UPDATE_INTERVAL = 10;

/* Sensor tresholds*/
int MIN_AUDIO_JITTER_THRESHOLD = 500;
int MAX_AUDIO_JITTER_THRESHOLD = 5000;
int MIN_PROXIMITY_THRESHOLD = 1;
int MAX_PROXIMITY_THRESHOLD = 100;


enum State {
    UNMEASURED,
    MEASURED_ENTANGLED,
    MEASURED_OWN
};

State currentState = UNMEASURED;

void onMessageCallback(WebsocketsMessage message) {
    Serial.println("hello");

    
    Serial.println(message.data());
    JsonDocument jsonMessage;
    deserializeJson(jsonMessage, message.data());



    if(jsonMessage["type"] == "entangled_measured" && currentState == UNMEASURED) {
        Serial.println("going from UNMEASURED to MEASURED_ENTANGLED");
        int angle = jsonMessage["angle"];
        for(Voxel* v: voxels){
            v->turnMotorsToMeasured(angle);
        }
        currentState = MEASURED_ENTANGLED;
        return;
    }

    if (jsonMessage["type"] == "entangled_unmeasured" && currentState == MEASURED_ENTANGLED) {
        Serial.println("going from MEASURED_ENTANGLED to UNMEASURED");
        for(Voxel* v: voxels){
            v->turnMotorsToUnmeasured();
        }
        currentState = UNMEASURED;
        return;
    }

    if(jsonMessage["type"] == "parameters"){
        MIN_PROXIMITY_THRESHOLD = jsonMessage["minProximityThreshold"];
        MAX_PROXIMITY_THRESHOLD = jsonMessage["maxProximityThreshold"];
        MIN_AUDIO_JITTER_THRESHOLD = jsonMessage["minAudioJitterThreshold"];
        MAX_AUDIO_JITTER_THRESHOLD = jsonMessage["maxAudioJitterThreshold"];
        AUDIO_SAMPLE_AMOUNT = jsonMessage["audioSampleAmount"];
        AUDIO_SAMPLE_INTERVAL = jsonMessage["audioSampleInterval"];
        PROXIMITY_SAMPLE_AMOUNT = jsonMessage["proximitySampleAmount"];
        PROXIMITY_SAMPLE_INTERVAL = jsonMessage["proximitySampleInterval"];

        JsonArray voxelArray = jsonMessage["voxels"];
        for (int i=0; i< voxelArray.size(); i++) {
            JsonObject voxelObj = voxelArray[i];

            JsonObject colorObj = voxelObj["colourMotor"];

            voxels[i]->getColorMotor()->setMinAngle(colorObj["minAngle"]);
            voxels[i]->getColorMotor()->setMaxAngle(colorObj["maxAngle"]);
            voxels[i]->getColorMotor()->setRotationIncrement(colorObj["rotationIncrement"]);
            voxels[i]->getColorMotor()->setSnapIncrement(colorObj["snapIncrement"]);
            voxels[i]->getColorMotor()->setMinJitterIncrement(colorObj["minJitterIncrement"]);
            voxels[i]->getColorMotor()->setMaxJitterIncrement(colorObj["maxJitterIncrement"]);

            JsonObject transparencyObj = voxelObj["transparencyMotor"];

            voxels[i]->getTransparencyMotor()->setActiveAngle(colorObj["activeAngle"]);
            voxels[i]->getTransparencyMotor()->setInactiveAngle(colorObj["inactiveAngle"]);
            voxels[i]->getTransparencyMotor()->setSnapIncrement(colorObj["snapIncrement"]);

        }
    }
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
    client.connect(String(CONNECTION_STRING) + "?mac_address=" + WiFi.macAddress());

    /* Send hello message on connection. */
    client.send(getHelloMessage()); 
    delay(2000);

    /* Set the I2C pins to the pins configured for the custom hardware */
    Wire.begin(SDA_PIN, SCL_PIN);
    pwm.begin();
    pwm.setPWMFreq(SERVO_PWM_FREQUENCY);

    voxels.reserve(numVoxels);
    for(int i=0; i < numVoxels; i++) {
        ColorMotor* motor1 = new ColorMotor(2*i, pwm, interval);
        TransparencyMotor* motor2 = new TransparencyMotor(2*i+1, pwm, interval);
        Voxel* v = new Voxel(motor1, motor2);
        voxels.push_back(v);
    }

    currentState = UNMEASURED;
}

void loop() {

    
    
    if(millis() - lastUpdateMicrophone > AUDIO_SAMPLE_INTERVAL) {
        int noise = microphone.measureAnalog(AUDIO_SAMPLE_AMOUNT);
        jitter = noise > MIN_AUDIO_JITTER_THRESHOLD && noise < MAX_AUDIO_JITTER_THRESHOLD;
        lastUpdateMicrophone = millis();
    }

    if(millis() - lastUpdateProximity > PROXIMITY_SAMPLE_INTERVAL) {
        unsigned long distance = ultrasoundSensor.getValue();
        proximityNear = distance > MIN_PROXIMITY_THRESHOLD && distance < MAX_PROXIMITY_THRESHOLD;
        lastUpdateProximity = millis();

    }
    

    switch (currentState) {
        case UNMEASURED:
            if((millis() - lastUpdateState) > BLOCKING_STATE_INTERVAL && proximityNear) {
                Serial.println("Going from UNMEASURED to MEASURED");
                currentState = MEASURED_OWN;
                String str = getJsonMeasured();

                client.send(str);

                for(Voxel* v: voxels){
                    v->turnMotorsToMeasured();
                }
                lastUpdateState = millis();
            }
            break;
        case MEASURED_ENTANGLED:
            break;
        case MEASURED_OWN:
            if((millis() - lastUpdateState) > BLOCKING_STATE_INTERVAL && !proximityNear) {
                Serial.println("going from MEASURED to UNMEASURED");
                currentState = UNMEASURED;
                client.send(getJsonUnmeasured());

                for(Voxel* v: voxels){
                    v->turnMotorsToUnmeasured();
                }
                lastUpdateState = millis();
            }
            break;
    }

    client.poll();
    /* In each loop update Motors. */
    if(millis() - lastUpdateMotors > MOTOR_UPDATE_INTERVAL){
        for(Voxel* v: voxels){
            v->setJitter(jitter);
            v->update();
        }
        lastUpdateMotors = millis();
    }
}


/*
    Message utility functions.
*/

String getJsonMeasured() {
    JsonDocument doc;

    doc["type"] = "measured";
    doc["macAddress"] = WiFi.macAddress();
    doc["currentColourAngle"] = voxels[0]->getColorMotor()->getAngle();

    String serializedDoc;
    serializeJson(doc, serializedDoc);
    return serializedDoc;
}

String getJsonUnmeasured() {
    JsonDocument doc;

    doc["type"] = "unmeasured";
    doc["macAddress"] = WiFi.macAddress();
    
    String serializedDoc;
    serializeJson(doc, serializedDoc);
    return serializedDoc;
}

String getHelloMessage() {
    JsonDocument doc;

    doc["type"] = "hello";
    doc["macAddress"] = WiFi.macAddress();

    JsonDocument arrayDoc;
    JsonArray sensorArray = arrayDoc.to<JsonArray>();

    sensorArray.add("ULTRASOUND");
    sensorArray.add("MICROPHONE");

    doc["sensors"] = sensorArray;

    String serializedDoc;
    serializeJson(doc, serializedDoc);
    return serializedDoc;
}
