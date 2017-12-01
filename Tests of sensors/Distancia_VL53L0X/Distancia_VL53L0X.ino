#include <Wire.h>
#include <VL53L0X.h>

const int kCANTIDAD_SENSORES = 4;
const int kXSHUT[kCANTIDAD_SENSORES] = {25, 23, 27, 29};

// 23 ATRAS 37
// 25 DERECHA 51
// 27 ADELANTE 39
// 29 IZQUIERDA 58

VL53L0X sensor[kCANTIDAD_SENSORES];

void setup() {
	//These pins are going connected to the XSHUT of each sensor
	for (int i = 0; i < kCANTIDAD_SENSORES; i++)
		pinMode(kXSHUT[i], OUTPUT);

	//First we put them in '0', so they are disabled
	for (int i = 0; i < kCANTIDAD_SENSORES; i++)
		digitalWrite(kXSHUT[i], LOW);

	//We wait a little bit until they are actually disabled and start the I2C communication
	delay(1000);
	Wire.begin();
	Serial.begin (9600);

	for (int i = 0; i < kCANTIDAD_SENSORES; i++) {
		//We put it in '1', so it's enabled.
		//*IMPORTANT NOTE* we cannot send a HIGH signal, because we would burn it. So, what we do is put a really high impedance on it. Because by default, that pin is connected to HIGH.
		pinMode(kXSHUT[i], INPUT);
		//Must wait this time for it to actually be enabled
		delay(200);
		Serial.println(2 * i);
		//Initialize the sensor
		sensor[i].init(true);
		Serial.println(2 * i + 1);
		//Wait for it to be initialized
		delay(200);
		//Set a new address
		//*IMPORTANT NOTE* we have to check that the addresses doesn't match with those of other sensors
		sensor[i].setAddress((uint8_t)(10 + i));
		Serial.println(2 * i);
		delay(200);
	}

	Serial.println("\naddresses set\n\n");
	//Set the time out of the sensor
	for (int i = 0; i < kCANTIDAD_SENSORES; i++)
		sensor[i].setTimeout(200);
}
// 3 back
// 2 left
// 1 right
// 0 front
void loop() {
	/*Serial.println("__________________________________________________________________\n");
	   Serial.println("=================================");
	   Serial.println ("I2C scanner. Scanning ...");
	   byte count = 0;

	   for (byte i = 1; i < 120; i++) {
	      Wire.beginTransmission (i);
	      if (Wire.endTransmission () == 0) {
	        Serial.print ("Found address: ");
	        Serial.print (i, DEC);
	        Serial.print (" (0x");
	        Serial.print (i, HEX);
	        Serial.println (")");
	        count++;
	        delay (1);  // maybe unneeded?
	      } // end of good response
	   } // end of for loop
	   Serial.println ("Done.");
	   Serial.print ("Found ");
	   Serial.print (count, DEC);
	   Serial.println (" device(s).");
	   Serial.println("=================================");*/

	int distance;
	for (int i = 0; i < kCANTIDAD_SENSORES; i++) {
		distance = 0;
		for(int j = 0; j < 10; j++) {
			distance += sensor[i].readRangeSingleMillimeters();
			//delay(1);
		}
		Serial.print(i);
		Serial.print(":  ");
		//Serial.println("SENSOR #" + i);
		if (sensor[i].timeoutOccurred())
			Serial.println(" TIMEOUT");
		else
			Serial.println(distance/10);
	}
	//delay(1000);//can change to a lower time like 100
	Serial.println("");
}
