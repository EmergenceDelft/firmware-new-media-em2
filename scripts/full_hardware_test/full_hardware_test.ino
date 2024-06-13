#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <NewPing.h>
#include <WiFi.h>


#define SERVO_MIN_PULSE_WIDTH 150 // Minimum pulse width for the servo
#define SERVO_MAX_PULSE_WIDTH 500 // Maximum pulse width for the servo

#define SCL_PIN 14
#define SDA_PIN 27
#define AMP_PIN 34
#define ULTRASOUND_TRIGGER_ECHO_PIN 22

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
NewPing sensor(ULTRASOUND_TRIGGER_ECHO_PIN, ULTRASOUND_TRIGGER_ECHO_PIN);


void setup() {
  Serial.begin(115200);
  Serial.println("Begin");  
    
  Wire.begin(SDA_PIN, SCL_PIN);

  pwm.begin();
  pwm.setPWMFreq(50);  // This is the maximum PWM frequency
}

void loop() {
    Serial.println(WiFi.macAddress());
    delay(1000);

    /* Servo 0 actuation */
    Serial.println("Servo number 0: 0 degrees");
    pwm.setPWM(0, 0, SERVO_MIN_PULSE_WIDTH);
    delay(1000);
    Serial.println("Servo number 0: 90 degrees");
    pwm.setPWM(0, 0, SERVO_MAX_PULSE_WIDTH);
    delay(1000);

    /* Servo 1 actuation */
    Serial.println("Servo number 1: 0 degrees");
    pwm.setPWM(1, 0, SERVO_MIN_PULSE_WIDTH);
    delay(1000);
    Serial.println("Servo number 1: 90 degrees");
    pwm.setPWM(1, 0, SERVO_MAX_PULSE_WIDTH);
    delay(1000);

    Serial.print("Microphone output: ");
    Serial.println(measureAnalog());

    Serial.print("Ultrasound output: ");
    Serial.println(sensor.ping_cm());
}

int measureAnalog() {
    int micSamples = 100;
    int signalAvg = 0, signalMax = 0, signalMin = 50024;
    for (int i = 0; i < micSamples; i++)
    {
        int k = analogRead(AMP_PIN);
        
        signalMin = min(signalMin, k);
        signalMax = max(signalMax, k);
        signalAvg += k;
    }

    signalAvg /= micSamples;

    return signalMax - signalMin;
}
