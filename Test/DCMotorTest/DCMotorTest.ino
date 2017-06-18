#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);
Adafruit_DCMotor *mySecondMotor = AFMS.getMotor(4);


void setup() {
  Serial.begin(9600);
  Serial.println("Adafruit Motorshield v2 - DC Motor test!");
  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz

  myMotor->setSpeed(150);
  myMotor->run(FORWARD);
  myMotor->run(RELEASE);
}

void loop() {
  uint8_t i;

  Serial.print("tick");

  myMotor->run(FORWARD);
  for (i=0; i<255; i++) {
    myMotor->setSpeed(i);
    delay(10);
  }
  for (i=255; i!=0; i--) {
    myMotor->setSpeed(i);
    delay(10);
  }

  Serial.print("tock");

  myMotor->run(BACKWARD);
  for (i=0; i<255; i++) {
    myMotor->setSpeed(i);
    delay(10);
  }
  for (i=255; i!=0; i--) {
    myMotor->setSpeed(i);
    delay(10);
  }

  Serial.print("tech");
  myMotor->run(RELEASE);
  delay(1000);

  mySecondMotor->run(FORWARD);
  for (i=0; i<255; i++) {
    mySecondMotor->setSpeed(i);
    delay(10);
  }
  for (i=255; i!=0; i--) {
    mySecondMotor->setSpeed(i);
    delay(10);
  }

  Serial.print("tock");

  mySecondMotor->run(BACKWARD);
  for (i=0; i<255; i++) {
    mySecondMotor->setSpeed(i);
    delay(10);
  }
  for (i=255; i!=0; i--) {
    mySecondMotor->setSpeed(i);
    delay(10);
  }

  Serial.print("tech");
  mySecondMotor->run(RELEASE);
  delay(1000);
}
