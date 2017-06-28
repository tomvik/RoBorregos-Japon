#include "Arduino.h"
#define limit1 53
#define limit2 52

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);
	pinMode(limit1, INPUT);
	pinMode(limit2, INPUT);
}

void loop() {
	// put your main code here, to run repeatedly:
	Serial.print(digitalRead(limit1)); Serial.print(" "); Serial.println(digitalRead(limit2));
}
