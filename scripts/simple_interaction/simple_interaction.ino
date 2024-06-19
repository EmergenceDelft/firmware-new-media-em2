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
#define ULTRASOUND_MAX_DISTANCE 100
#define ULTRASOUND_MIN_DISTANCE 2

#define AUDIO_JITTER_THRESHOLD 500

int colorMotorAddress = 1;
int colorMotorSpeed = 5;
int colorMotorMinAngle = 0;
int colorMotorMaxAngle = 120;
int colorMotorCurrentAngle = 0;
int colorMotorSetupAngle = 60;

int transparencyMotorAddress = 0;
int transparencyMotorMinAngle = 175;
int transparecyMotorMaxAngle = 70;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
NewPing sensor(ULTRASOUND_TRIGGER_ECHO_PIN, ULTRASOUND_TRIGGER_ECHO_PIN);

int generateRandomBetween(int a, int b) {
    int random_integer = rand() % (b+1);
    return random_integer + a;
}

void setup() {
    Serial.begin(115200);
    Serial.println("Begin");  
    
    Wire.begin(SDA_PIN, SCL_PIN);

    pwm.begin();
    pwm.setPWMFreq(50);  // This is the maximum PWM frequency

  /* Initialize color motor at 60 degrees for 30 seconds to fix the color filter. */
  pwm.setPWM(colorMotorAddress, 0, convertAngleToPulseWidth(colorMotorSetupAngle));
  for(int i = 10; i > 0; i--) {
    Serial.print("Waiting for another ");
    Serial.print(i);
    Serial.println(" seconds.");
    delay(1000);
  }
}


void loop() {

    int currenUltrasonicDistance = sensor.ping_cm();
    Serial.print("Current ultrasonic distance: ");
    Serial.println(currenUltrasonicDistance);


    if(currenUltrasonicDistance < ULTRASOUND_MAX_DISTANCE && currenUltrasonicDistance > ULTRASOUND_MIN_DISTANCE) {
        /* Flip transparency filter to active state. */
        pwm.setPWM(transparencyMotorAddress, 0, convertAngleToPulseWidth(transparencyMotorMinAngle));
        /* Adding a delay after measurement occurs gives more rest */
        delay(2000);
        return;
    }

    /* Flip transparency filter to inactive state. */
    pwm.setPWM(transparencyMotorAddress, 0, convertAngleToPulseWidth(transparecyMotorMaxAngle));

    bool jitter = measureMicrophone() > AUDIO_JITTER_THRESHOLD;
  
    if(colorMotorCurrentAngle >= colorMotorMaxAngle){
        movingTowards180 = false;
    }
    if(colorMotorCurrentAngle <= colorMotorMinAngle){
        movingTowards180 = true;
    }

    if(jitter) {
        int random_nr = generateRandomBetween(-2,5);
        if(movingTowards180) {
            increment = random_nr;
        }else {
            increment = -random_nr;
        }
    }else{
        if(movingTowards180) {
            increment = 1;
        }else {
            increment = -1;
        } 
    }
    colorMotorCurrentAngle += increment;

    pwm.setPWM(colorMotorAddress, 0, convertAngleToPulseWidth(colorMotorCurrentAngle));


    delay(100);
}


long convertAngleToPulseWidth(int motorAngle) {
  return map(motorAngle, 0, 180, SERVO_MIN_PULSE_WIDTH, SERVO_MAX_PULSE_WIDTH);
}

int measureMicrophone() {
    int micSamples = 10;
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
