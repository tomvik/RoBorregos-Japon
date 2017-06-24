<<<<<<< HEAD
/* 
This is a test sketch for the Adafruit assembled Motor Shield for Arduino v2
It won't work with v1.x motor shields! Only for the v2's with built in PWM
control

For use with the Adafruit Motor Shield v2 
---->	http://www.adafruit.com/products/1438
*/
//1 der atras     bien
//4 der adelante  bien
//2 izq adelante  bien
//3 izq atras     alreves

=======
>>>>>>> 3a0e0bc20bf8457de483a46aa0ff21ac8f76a99c
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

<<<<<<< HEAD
// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *myMotor = AFMS.getMotor(2);
Adafruit_DCMotor *mySecondMotor = AFMS.getMotor(3);
// You can also make another motor on port M2
//Adafruit_DCMotor *myOtherMotor = AFMS.getMotor(2);
=======
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);
Adafruit_DCMotor *mySecondMotor = AFMS.getMotor(4);

>>>>>>> 3a0e0bc20bf8457de483a46aa0ff21ac8f76a99c

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
