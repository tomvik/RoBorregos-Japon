#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *alfa = AFMS.getMotor(1);// derecha adelante
Adafruit_DCMotor *beta = AFMS.getMotor(2); // izquierda atras
Adafruit_DCMotor *ce = AFMS.getMotor(3); // izquierda adelante
Adafruit_DCMotor *de = AFMS.getMotor(4); // derecha atras


#define ENCODER_A 5
volatile int o = 0;
void x() {
	o++;
}

void setup() {
	Serial.begin(9600);
	Serial.println("Adafruit Motorshield v2 - DC Motor test!");
	AFMS.begin(); // create with the default frequency 1.6KHz
	//AFMS.begin(1000);  // OR with a different frequency, say 1KHz

	alfa->setSpeed(155);
	beta->setSpeed(155);
	ce->setSpeed(155);
	de->setSpeed(155);
	alfa->run(BACKWARD);
	delay(2000);
	beta->run(BACKWARD);
	delay(2000);
	ce->run(BACKWARD);
	delay(2000);
	de->run(FORWARD);
	attachInterrupt(ENCODER_A, x, RISING);
}

void loop() {
	Serial.println(o);
	delay(100);
}
