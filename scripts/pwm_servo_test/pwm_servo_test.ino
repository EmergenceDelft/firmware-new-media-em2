#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVO_MIN_PULSE_WIDTH 150 // Minimum pulse width for the servo
#define SERVO_MAX_PULSE_WIDTH 500 // Maximum pulse width for the servo

#define SDA_PIN 27
#define SCL_PIN 14

void setup() {
  Serial.begin(115200);
  Serial.println("16 channel PWM test!");

  Wire.begin(SDA_PIN, SCL_PIN);
  pwm.begin();
  pwm.setPWMFreq(50);
}

void loop() {
    for(int i = 0; i < 15; i++ ) {
      Serial.print("Sending PWM signal to pin: ");
      Serial.println(i);
      Serial.print("Pulse width: ");
      Serial.println(SERVO_MIN_PULSE_WIDTH);
      pwm.setPWM(i, 0, SERVO_MIN_PULSE_WIDTH);
      delay(500);

      Serial.print("Pulse width: ");
      Serial.println(SERVO_MAX_PULSE_WIDTH);
      pwm.setPWM(i, 0, SERVO_MAX_PULSE_WIDTH);
      delay(500);
    }
}
