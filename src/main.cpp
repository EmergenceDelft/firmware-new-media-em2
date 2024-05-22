#include <main.h>
#include <UltrasoundSensor.h>
#include <Motor.h>


const char* ssid = "NPRouter"; //Enter SSID
const char* password = "keepitquantum"; //Enter Password
const char* websockets_server = "ws://192.168.4.5:3000/echo"; //server adress and port

WebsocketsClient client;
TaskHandle_t pollingTaskHandle;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
UltrasoundSensor ultrasoundSensor(WiFi.macAddress() + "//ULTRASOUND");
Motor motor1(WiFi.macAddress() + "//" + 0, 0, pwm);

void onMessageCallback(WebsocketsMessage message) {
    Serial.println(message.data());
    JsonDocument jsonMessage;
    deserializeJson(jsonMessage, message.data());

    if(jsonMessage.containsKey("angle")) {
        int angle = jsonMessage["angle"];
        if(angle == 90) {
          pwm.setPWM(0, 0, 550);
        } else {
          pwm.setPWM(0, 0, 125);
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
    doc["motor_amount"] = 5;


    String serializedDoc;
    serializeJson(doc, serializedDoc);
    return serializedDoc;
}


// void pollingTask( void * pvParameters ){
//     for(;;) {
//         client.poll();
//     }
// }

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

    //Setup pwm
    pwm.begin();
    pwm.setPWMFreq(SERVO_PWM_FREQUENCY);

    //All polling code is executed on a different thread to keep the main thread open for reading sensor data.
    // xTaskCreatePinnedToCore(pollingTask, "Sensor task", 10000, NULL, 1, &pollingTaskHandle, CORE_0);         
}

void loop() {
    client.poll();
    client.send(ultrasoundSensor.getJsonSerializedReadings());
    delay(1000);
}
