#include <main.h>
#include <UltrasoundSensor.h>
#include <Motor.h>
#include <ColourMotor.h>
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

//so we wanted to control the number of voxels/motors from the server side so you don't have to flash everytime
//right now, the constants for voxels and motors are set to the highest amount it could possibly be
//in the setup message from the server, we make sure to only send constants for only the actual amount of voxels
//IMPORTANT it's not a big deal if the addresses a voxel that does not actually exist, so good to keep this number high rather than low
//so make numVoxels > voxelArray.size() (from setup json message from the server)
int numVoxels = 2;

int interval = 10; //default interval for updating motors

//lastUpdates
unsigned long lastUpdate = 0;
unsigned long lastUpdateClient = 0;
unsigned long lastUpdateState = 0;
unsigned long lastUpdateMicrophone = 0;
unsigned long lastUpdateProximity = 0;
unsigned long lastUpdateMotors = 0;

long distanceReal[3];
long distanceFilter[3];

bool colourMotorJitter = true;
bool proximityNear = false;


//how often to sample 
//IMPORTANT the higher the sample amount/lower the sample interval, the more the program is blocking (no motor movement, no sensing)
int AUDIO_SAMPLE_INTERVAL = 500;
int AUDIO_SAMPLE_AMOUNT = 100;
int PROXIMITY_SAMPLE_INTERVAL = 100;
int PROXIMITY_SAMPLE_AMOUNT = 10;

int MOTOR_UPDATE_INTERVAL = 20;
int UNMEASURED_BLOCKING_STATE_INTERVAL = 2000;
int MEASURED_BLOCKING_STATE_INTERVAL = 2000;


/* Sensor tresholds*/
int MIN_AUDIO_JITTER_THRESHOLD = 200;
int MAX_AUDIO_JITTER_THRESHOLD = 5000;
int MIN_PROXIMITY_THRESHOLD = 1;
int MAX_PROXIMITY_THRESHOLD = 150;

/* Transparency motor jitter */
bool TRANSPARENCY_MOTOR_JITTER = true;


//three states the ESP can be in
//measured own means someone is activating the proximity sensor
//measured entangled means entangled esp is measured and server sent message to us to also be measured and stopped
enum State {
    UNMEASURED,
    MEASURED_ENTANGLED,
    MEASURED_OWN,
    JITTER
};

State currentState = UNMEASURED;

long LPF (long input){
 // float timestamp = _micros();
    if(input > 450){
        input = 450;
    }
  distanceReal[0] = input;
   float b[] = {0.06745527, 0.13491054, 0.06745527};
    float a[] = {1.14298050, -0.41280160};
  distanceFilter[0] = a[0]*distanceFilter[1] + a[1]*distanceFilter[2] + b[0]*distanceReal[0] + b[1]*distanceReal[1] + b[2]*distanceReal[2];

  for(int i = 1; i >= 0; i--){
    distanceReal[i+1] = distanceReal[i]; // store xi
    distanceFilter[i+1] = distanceFilter[i]; // store yi
  }
  Serial.println(distanceFilter[0]);
  return distanceFilter[0];
}

void onMessageCallback(WebsocketsMessage message) {
    Serial.println("hello");

    
    Serial.println(message.data());
    JsonDocument jsonMessage;
    deserializeJson(jsonMessage, message.data());

    //when receiving entangled measured message, snap colour motor to the angle of the entangled colour motor
    if(jsonMessage["type"] == "entangled_measured" && currentState == JITTER) {
        Serial.println("going from " + String(currentState) + " to MEASURED_ENTANGLED");
        JsonObject content = jsonMessage["content"];
        int angle = content["currentColourAngle"];
        Serial.println(angle);

        //IDEA with more voxels, one can do more interesting things
        for(Voxel* v: voxels){
            v->turnMotorsToMeasured(angle);
            v->setTransparencyMotorJitter(false);
        }

        //switch state to measured entangled, only way to go out of this state is an unmeasure entangled message
        currentState = MEASURED_ENTANGLED;
        return;
    }

    //similarly go from entangled measured to unmeasuerd
    if (jsonMessage["type"] == "entangled_unmeasured" && currentState == MEASURED_ENTANGLED) {
        Serial.println("going from MEASURED_ENTANGLED to UNMEASURED");
        for(Voxel* v: voxels){
            v->turnMotorsToUnmeasured();
        }
        currentState = UNMEASURED;
        return;
    }

    if (jsonMessage["type"] == "button_pressed" && currentState == UNMEASURED) {
        Serial.println("going from " + String(currentState) + " to JITTER");
        for(Voxel* v: voxels){
            v->turnMotorsToJitter();
            v->setTransparencyMotorJitter(true);
        }
        currentState = JITTER;
        return;
    }

    //server messages us a bunch of setup constants
    //this is so that when wanting to make a small change, one does not need to reflash the esp
    //particularly useful for testing/exhibiting
    if(jsonMessage["type"] == "setup"){
        JsonObject content = jsonMessage["content"];

        MIN_PROXIMITY_THRESHOLD = content["minProximityThreshold"];
        MAX_PROXIMITY_THRESHOLD = content["maxProximityThreshold"];
        MIN_AUDIO_JITTER_THRESHOLD = content["minAudioJitterThreshold"];
        MAX_AUDIO_JITTER_THRESHOLD = content["maxAudioJitterThreshold"];
        AUDIO_SAMPLE_AMOUNT = content["audioSampleAmount"];
        AUDIO_SAMPLE_INTERVAL = content["audioSampleInterval"];
        PROXIMITY_SAMPLE_AMOUNT = content["proximitySampleAmount"];
        PROXIMITY_SAMPLE_INTERVAL = content["proximitySampleInterval"];
        TRANSPARENCY_MOTOR_JITTER = content["transparencyMotorJitter"];
        UNMEASURED_BLOCKING_STATE_INTERVAL = content["unmeasuredBlockingStateInterval"];
        MEASURED_BLOCKING_STATE_INTERVAL = content["measuredBlockingStateInterval"];


        JsonArray voxelArray = content["voxels"];

        //be careful to receive a voxelArray that is not bigger than the numVoxels we have
        for (int i=0; i< voxelArray.size(); i++) {
            JsonObject voxelObj = voxelArray[i];

            //british english to american english
            JsonObject colourObj = voxelObj["colourMotor"];

            voxels[i]->getColourMotor()->setMinAngle(colourObj["minAngle"]);
            voxels[i]->getColourMotor()->setMaxAngle(colourObj["maxAngle"]);
            voxels[i]->getColourMotor()->setRotationIncrement(colourObj["rotationIncrement"]);
            voxels[i]->getColourMotor()->setSnapIncrement(colourObj["snapIncrement"]);
            voxels[i]->getColourMotor()->setMinJitterIncrement(colourObj["minJitterIncrement"]);
            voxels[i]->getColourMotor()->setMaxJitterIncrement(colourObj["maxJitterIncrement"]);

            JsonObject transparencyObj = voxelObj["transparencyMotor"];

            voxels[i]->getTransparencyMotor()->setActiveAngle(transparencyObj["activeAngle"]);
            voxels[i]->getTransparencyMotor()->setInactiveAngle(transparencyObj["inactiveAngle"]);
            voxels[i]->getTransparencyMotor()->setSnapIncrement(transparencyObj["snapIncrement"]);

        }
        //useful for making sure the server sends the right stuff
        //there are a surprising amount of errors/typos that happen when sending json messages
        Serial.print("MIN_PROXIMITY_THRESHOLD: ");
        Serial.println(MIN_PROXIMITY_THRESHOLD);
        Serial.print("MAX_PROXIMITY_THRESHOLD: ");
        Serial.println(MAX_PROXIMITY_THRESHOLD);
        Serial.print("MIN_AUDIO_JITTER_THRESHOLD: ");
        Serial.println(MIN_AUDIO_JITTER_THRESHOLD);
        Serial.print("MAX_AUDIO_JITTER_THRESHOLD: ");
        Serial.println(MAX_AUDIO_JITTER_THRESHOLD);
        
        Serial.print("AUDIO_SAMPLE_AMOUNT: ");
        Serial.println(AUDIO_SAMPLE_AMOUNT);
        Serial.print("AUDIO_SAMPLE_INTERVAL: ");
        Serial.println(AUDIO_SAMPLE_INTERVAL);
        Serial.print("PROXIMITY_SAMPLE_AMOUNT: ");
        Serial.println(PROXIMITY_SAMPLE_AMOUNT);
        Serial.print("PROXIMITY_SAMPLE_AMOUNT: ");
        Serial.println(PROXIMITY_SAMPLE_INTERVAL);
        Serial.print("TRANSPARENCY_MOTOR_JITTER: ");
        Serial.println(TRANSPARENCY_MOTOR_JITTER);        
        Serial.print("UNMEASURED_BLOCKING_STATE_INTERVAL: ");
        Serial.println(UNMEASURED_BLOCKING_STATE_INTERVAL);
        Serial.print("MEASURED_BLOCKING_STATE_INTERVAL: ");
        Serial.println(MEASURED_BLOCKING_STATE_INTERVAL);
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

    /* Set the I2C pins to the pins configured for the custom hardware */
    Wire.begin(SDA_PIN, SCL_PIN);
    pwm.begin();
    pwm.setPWMFreq(SERVO_PWM_FREQUENCY);

    voxels.reserve(numVoxels);
    for(int i=0; i < numVoxels; i++) {
        TransparencyMotor* motor2 = new TransparencyMotor(2*i, pwm, interval);
        ColourMotor* motor1 = new ColourMotor(2*i + 1, pwm, interval);

        //IMPORTANT!!!!
        //for aligning colour motor in the back, we want to set it to the center of its virtual position
        //and then align the physical gear (so that it is in the middle)
        motor1->setAngle(90);
        Voxel* v = new Voxel(motor1, motor2);
        voxels.push_back(v);
    }

    //IMPORTANT
    //huge delay so that one is able to align the colour motor as described previously
    //one can screw in the gear rod into the colour motor in these 10 seconds
    //easier way is to turn off the power after the gear went to the middle in the previous step
    //then screw it in while making sure that you are not moving the motor
    delay(10000);

    //default state is unmeasured
    currentState = UNMEASURED;
    for(Voxel* v: voxels){
        v->turnMotorsToUnmeasured();
    }
}

void loop() {

    if(millis() - lastUpdateMicrophone > AUDIO_SAMPLE_INTERVAL) {
        //this is blocking, don't make the AUDIO_SAMPLE_AMOUNT too big
        int noise = microphone.measureAnalog(AUDIO_SAMPLE_AMOUNT);
        //basic way to determine if the movement should be jittery or not, advanced way would be to scale it linearly or something
        colourMotorJitter = noise > MIN_AUDIO_JITTER_THRESHOLD && noise < MAX_AUDIO_JITTER_THRESHOLD;
        lastUpdateMicrophone = millis();
    }

    if(millis() - lastUpdateProximity > PROXIMITY_SAMPLE_INTERVAL) {
        //keep in mind the ultrasoundSensor only pings the distance once
        //this could be a bit unreliable

        //keep in mind that if the ultrasoundSensor does not detect something in its MAX_RANGE (450 i think)
        //the returned distance is 0
        //IMPORTANT filter out the 0 values so that it's properly understood as out of range and not 0 cm away
        long distance = ultrasoundSensor.getValue(10)/57;
        Serial.println(distance);
        // long filtered_dist = LPF(real_dist);
        // unsigned long distance = LPF(ultrasoundSensor.getValue());
        proximityNear = distance > MIN_PROXIMITY_THRESHOLD && distance < MAX_PROXIMITY_THRESHOLD;
        
        lastUpdateProximity = millis();

    }
    

    switch (currentState) {
        case JITTER:
            if((millis() - lastUpdateState) > UNMEASURED_BLOCKING_STATE_INTERVAL && proximityNear) {
                Serial.println("Going from JITTER to MEASURED_OWN");
                currentState = MEASURED_OWN;

                //send to server own colour angle, so that server can send to all entangled esps which angle to go to
                String str = getJsonMeasured();

                client.send(str);

                for(Voxel* v: voxels){
                    v->setTransparencyMotorJitter(false);
                    v->turnMotorsToMeasured();
                }
                lastUpdateState = millis();
            }
            break;
        case UNMEASURED:

            //When the device is unmeasured and the JITTER_TRANSPARENCY_FILTER_IS_SET
            //Make sure the transparency filter jitters

            if((millis() - lastUpdateState) > UNMEASURED_BLOCKING_STATE_INTERVAL && proximityNear) {
                Serial.println("Going from UNMEASURED to MEASURED");
                currentState = MEASURED_OWN;

                //send to server own colour angle, so that server can send to all entangled esps which angle to go to
                String str = getJsonMeasured();

                client.send(str);

                for(Voxel* v: voxels){
                    v->setTransparencyMotorJitter(false);
                    v->turnMotorsToMeasured();
                }
                lastUpdateState = millis();
            }
            break;
        case MEASURED_ENTANGLED:
            //doing nothing, just waiting for message from server to go from measured entangled to unmeasured
            break;
        case MEASURED_OWN:
            if((millis() - lastUpdateState) > MEASURED_BLOCKING_STATE_INTERVAL && !proximityNear) {
                Serial.println("going from MEASURED_OWN to UNMEASURED");
                currentState = UNMEASURED;

                //send server unmeasure message, so it can notify all entangled ESPs to go to unmeasured too
                client.send(getJsonUnmeasured());

                for(Voxel* v: voxels){
                    v->setTransparencyMotorJitter(false);
                    v->turnMotorsToUnmeasured();
                }
                lastUpdateState = millis();
            }
            break;
    }

    //poll for messages from websockets, if you don't call this you will not notice messages from the server
    client.poll();
    /* In each loop update Motors. */
    if(millis() - lastUpdateMotors > MOTOR_UPDATE_INTERVAL){
        for(Voxel* v: voxels){
            v->setColourMotorJitter(colourMotorJitter);
            //IMPORTANT
            //this updates all motors, they will all move by a set amount of steps to where they need to
            //if this is not called or called too rarely you will notice
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
    //for communicating current angle to entangled esps, we only get the angle from the first voxel colour motor
    //IDEA send different angles, not just the first one. have different voxels entangled with other voxels
    doc["currentColourAngle"] = voxels[0]->getColourMotor()->getAngle();

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

    //send sensors to server, this is not really used anymore :((
    //idea was that each esp has different sensors and sends it to the server
    //but then you'd have to somehow query your existing sensor or reflash it if you have different sensorArrays
    doc["sensors"] = sensorArray;

    String serializedDoc;
    serializeJson(doc, serializedDoc);
    return serializedDoc;
}
