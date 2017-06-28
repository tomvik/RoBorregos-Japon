#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotorRightF = AFMS.getMotor(3);//al revés
Adafruit_DCMotor *myMotorRightB = AFMS.getMotor(1);//al revés
Adafruit_DCMotor *myMotorLeftF = AFMS.getMotor(2);
Adafruit_DCMotor *myMotorLeftB = AFMS.getMotor(4);//al revés

long iC = 0;


void Stop(){
   myMotorRightB->run(RELEASE);
   myMotorLeftB->run(RELEASE);
   myMotorRightF->run(RELEASE);
   myMotorLeftF->run(RELEASE);
}

void Front(uint8_t PowD, uint8_t PowI){
   myMotorLeftF->setSpeed(PowI);
   myMotorRightF->setSpeed(PowD);
   myMotorLeftB->setSpeed(PowI);
   myMotorRightB->setSpeed(PowD);

   myMotorRightF->run(FORWARD);
   myMotorRightB->run(FORWARD);

   myMotorLeftF->run(FORWARD);
   myMotorLeftB->run(FORWARD);
   //while(digitalRead(lack) == 0){
   //  Stop();
   //}
}

void setup() {
  // put your setup code here, to run once:
  attachInterrupt(5, encoder1, RISING);
  Serial.begin(9600);
  
  AFMS.begin();
  myMotorLeftF->setSpeed(10);
  myMotorLeftF->run(FORWARD);
  myMotorLeftF->run(RELEASE);
  myMotorLeftB->setSpeed(10);
  myMotorLeftB->run(FORWARD);
  myMotorLeftB->run(RELEASE);
  myMotorRightF->setSpeed(10);
  myMotorRightF->run(FORWARD);
  myMotorRightF->run(RELEASE);
  myMotorRightB->setSpeed(10);
  myMotorRightB->run(FORWARD);
  myMotorRightB->run(RELEASE);

  Stop();
  delay(1000);

  Serial.print("BEGIN "); Serial.println(++iC);
  Front(100, 100);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(iC);
}

void encoder1() {
  iC++;
}
