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
bool jitter = true;
bool near = false;

int AUDIO_JITTER_THRESHOLD = 500;
int PROXIMITY_THRESHOLD = 50;

int ACTIVE_TRANSPARENCY_ANGLE = 0;
int INACTIVE_TRANSPARENCY_ANGLE = 90;

String state;  

enum State {
    UNMEASURED,
    MEASURED_ENTANGLED,
    MEASURED_OWN
};

State currentState = UNMEASURED;

void onMessageCallback(WebsocketsMessage message) {

    
    Serial.println(message.data());
    JsonDocument jsonMessage;
    deserializeJson(jsonMessage, message.data());



    if(jsonMessage["type"] == "entangled_measure" && currentState == UNMEASURED) {
        int angle = jsonMessage["angle"];
        for(Voxel* v: voxels){
            v->turnMotorsToMeasured(angle);
        }
        currentState = MEASURED_ENTANGLED;
        return;
    }

    if (jsonMessage["type"] == "entangled_unmeasure" && currentState == MEASURED_ENTANGLED) {
        for(Voxel* v: voxels){
            v->turnMotorsToUnmeasured();
        }
        currentState = UNMEASURED;
        return;
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
    
    Serial.println("Connected to wifi");

    client.onMessage(onMessageCallback);
    client.onEvent(onEventsCallback);
    client.connect(CONNECTION_STRING);
    
    /* Send hello message on connection. */
    client.send(getHelloMessage()); 
    delay(2000);

    /* Set the I2C pins to the pins configured for the custom hardware */
    Wire.begin(SDA_PIN, SCL_PIN);
    pwm.begin();
    pwm.setPWMFreq(SERVO_PWM_FREQUENCY);
    Serial.println("done with pwm");


    voxels.reserve(numVoxels);

    for(int i=0; i < numVoxels; i++) {
        ColorMotor* motor1 = new ColorMotor(2*i, pwm, interval);
        TransparencyMotor* motor2 = new TransparencyMotor(2*i+1, pwm, interval);
        Voxel* v = new Voxel(motor1, motor2);
        voxels.push_back(v);
    }

    currentState = UNMEASURED;
}

void updateSensors() {
    if((millis() - lastUpdate) > SENSOR_INTERVAL) {
        String sensor_reading = ultrasoundSensor.getJsonSerializedReadings();
        //client.send(sensor_reading);
        Serial.println(sensor_reading);
        String microphone_reading = microphone.getJsonSerializedReadings();
        //client.send(microphone_reading);
        Serial.println(microphone_reading);
        lastUpdate = millis();


        near = ultrasoundSensor.getValue() < PROXIMITY_THRESHOLD;
        jitter = microphone.getLatest() > AUDIO_JITTER_THRESHOLD;

        if(near && currentState == UNMEASURED) {
            currentState = MEASURED_OWN;
            client.send(getJsonMeasured());

            for(Voxel* v: voxels){
                v->turnMotorsToMeasured();
            }
        }
        if(!near && currentState == MEASURED_OWN) {
            currentState = UNMEASURED;
            client.send(getJsonUnmeasured());

            for(Voxel* v: voxels){
                v->turnMotorsToUnmeasured();
            }
        }
    }
}

void updateClientConnection() {
    if((millis() - lastUpdateClient) > CLIENT_INTERVAL){
        client.poll();
        lastUpdateClient = millis();
    }
}


void loop() {

    switch (currentState) {
        case UNMEASURED:
            updateSensors();
            updateClientConnection();
            for(Voxel* v: voxels){
                v->setJitter(jitter);
                v->update();
            }
            //
            break;
        case MEASURED_ENTANGLED:
            updateClientConnection();
            //update Motors here
            break;
        case MEASURED_OWN:
            updateSensors();
            updateClientConnection();
            for(Voxel* v: voxels){
                v->setJitter(jitter);
                v->update();
            }
            //
            break;
    }
}


String getJsonMeasured() {
    JsonDocument doc;

    doc["type"] = "measured";
    doc["angle"] = voxels[0]->getColorMotor()->getAngle();

    String serializedDoc;
    serializeJson(doc, serializedDoc);
    return serializedDoc;
}
String getJsonUnmeasured() {
    JsonDocument doc;

    doc["type"] = "unmeasured";
    
    String serializedDoc;
    serializeJson(doc, serializedDoc);
    return serializedDoc;
}

