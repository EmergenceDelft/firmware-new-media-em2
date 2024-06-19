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



Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
NewPing sensor(ULTRASOUND_TRIGGER_ECHO_PIN, ULTRASOUND_TRIGGER_ECHO_PIN);


void setup() {
  Serial.begin(115200);
  Serial.println("Begin");  
    
  Wire.begin(SDA_PIN, SCL_PIN);

  pwm.begin();
  pwm.setPWMFreq(50);  // This is the maximum PWM frequency

  

}

int colorMotorAddress = 1;
int colorMotorSpeed = 5;
int colorMotorMinAngle = 0;
int colorMotorMaxAngle = 120;
int colorMotorCurrentAngle = 0;

int transparencyMotorAddress = 0;
int transparencyMotorMinAngle = 0;
int transparecyMotorMaxAngle = 90;


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

  /* Handle color motor rotation */
  if(colorMotorCurrentAngle >= colorMotorMaxAngle) {
    colorMotorSpeed = -5;
  }
  if(colorMotorCurrentAngle <= colorMotorMinAngle) {
    colorMotorSpeed = 5;
  }
  colorMotorCurrentAngle += colorMotorSpeed;

  pwm.setPWM(colorMotorAddress, 0, convertAngleToPulseWidth(colorMotorCurrentAngle));


  delay(100);
}


long convertAngleToPulseWidth(int motorAngle) {
  return map(motorAngle, 0, 180, SERVO_MIN_PULSE_WIDTH, SERVO_MAX_PULSE_WIDTH);
}

int measureMicrophone() {
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
