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

int colourMotorAddress = 1;
int colourMotorSpeed = 5;
int colourMotorMinAngle = 0;
int colourMotorMaxAngle = 100;
int colourMotorCurrentAngle = 20;
int colourMotorSetupAngle = 60;
bool colourMotorIsMovingPositive = false;

int transparencyMotorAddress = 0;
int transparencyMotorMinAngle = 90;
int transparecyMotorMaxAngle = 195;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
NewPing sensor(ULTRASOUND_TRIGGER_ECHO_PIN, ULTRASOUND_TRIGGER_ECHO_PIN);

void setup() {
    Serial.begin(115200);
    Serial.println("Begin");  
    
    Wire.begin(SDA_PIN, SCL_PIN);

    pwm.begin();
    pwm.setPWMFreq(50);  // This is the maximum PWM frequency

  /* Initialize colour motor at 60 degrees for 30 seconds to fix the colour filter. */
  pwm.setPWM(colourMotorAddress, 0, convertAngleToPulseWidth(colourMotorSetupAngle));
  for(int i = 3; i > 0; i--) {
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
        pwm.setPWM(transparencyMotorAddress, 0, convertAngleToPulseWidth(transparecyMotorMaxAngle));
        /* Adding a delay after measurement occurs gives more rest */
        delay(2000);
        return;
    }

    /* Flip transparency filter to inactive state. */
    pwm.setPWM(transparencyMotorAddress, 0, convertAngleToPulseWidth(transparencyMotorMinAngle));

    bool jitter = measureMicrophone() > AUDIO_JITTER_THRESHOLD;
  
    if(colourMotorCurrentAngle >= colourMotorMaxAngle){
        colourMotorIsMovingPositive = false;
    }
    if(colourMotorCurrentAngle <= colourMotorMinAngle){
        colourMotorIsMovingPositive = true;
    }

    if(jitter) {
        int random_nr = generateRandomBetween(-2,5);
        if(colourMotorIsMovingPositive) {
            colourMotorSpeed = random_nr;
        }else {
            colourMotorSpeed = -random_nr;
        }
    }else{
        if(colourMotorIsMovingPositive) {
            colourMotorSpeed = 3;
        }else {
            colourMotorSpeed = -3;
        } 
    }

    colourMotorCurrentAngle += colourMotorSpeed;
    Serial.print("Colour filter turning to: ");
    Serial.println(colourMotorCurrentAngle);

    pwm.setPWM(colourMotorAddress, 0, convertAngleToPulseWidth(colourMotorCurrentAngle));
    delay(50);
}

/*
  Utility Functions
*/

int generateRandomBetween(int a, int b) {
    int random_integer = rand() % (b+1);
    return random_integer + a;
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
