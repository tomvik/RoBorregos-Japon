#include "Arduino.h"
#include "SensarRealidad.h"
#include <Wire.h>
#include <VL53L0X.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <EEPROM.h>
//LED
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR  0x3F
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);

Adafruit_BNO055 bno = Adafruit_BNO055();

#define switchIzquierda 53
#define switchDerecha 35

#define toleranciaSwitchIMU 5

const int kCANTIDAD_SENSORES = 4;
const int kXSHUT[kCANTIDAD_SENSORES] = {23, 25, 27, 29};
/*
0 = derecha
1 = enfrente
2 = izquierda
3 = atras
*/

VL53L0X sensor[kCANTIDAD_SENSORES];


SensarRealidad::SensarRealidad() {
	for (int i = 0; i < kCANTIDAD_SENSORES; i++){
		pinMode(kXSHUT[i], OUTPUT);
		digitalWrite(kXSHUT[i], LOW);
	}
	delay(1000);
    Wire.begin();
    for (int i = 0; i < kCANTIDAD_SENSORES; i++) {
     //We put it in '1', so it's enabled.
     //*IMPORTANT NOTE* we cannot send a HIGH signal, because we would burn it. So, what we do is put a really high impedance on it. Because by default, that pin is connected to HIGH.
     pinMode(kXSHUT[i], INPUT);
     //Must wait this time for it to actually be enabled
     delay(200);
     //Initialize the sensor
     sensor[i].init(true);
     //Wait for it to be initialized
     delay(200);
     //Set a new address
     //*IMPORTANT NOTE* we have to check that the addresses doesn't match with those of other sensors
     sensor[i].setAddress((uint8_t)(10 + i));
     Serial.println(2 * i);
     delay(200);
   }
   for (int i = 0; i < kCANTIDAD_SENSORES; i++)
 	   sensor[i].setTimeout(200);
	if(!bno.begin())
 	{
    	/* There was a problem detecting the BNO055 ... check your connections */
    	Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    	//while(1);
  	}
  	bno.setExtCrystalUse(true);
  	Serial.println("A CALIBRAR");
  	while(getIMUCalStatus() <= 0);
  	Serial.println("TERMINE");

	pinMode(switchIzquierda, INPUT);
	pinMode(switchDerecha, INPUT);

	lcd.begin();// Indicamos medidas de LCD
	lcd.backlight();
}


byte SensarRealidad::getIMUCalStatus()
{
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);
  return gyro;
}

/*uint8_t SensarRealidad::calcDistanciaUS(uint8_t trigger, uint8_t echo) {
   digitalWrite(trigger, LOW);
   delayMicroseconds(5);
   digitalWrite(trigger, HIGH);
   delayMicroseconds(10);
   long tiempo = pulseIn(echo, HIGH);
   return uint8_t(0.017 * tiempo);
}*/

/*uint8_t SensarRealidad::calcDistanciaSharp(uint8_t sensor) {
   float volts = analogRead(sensor)*0.0048828125;  // value from sensor * (5/1024)
   uint8_t distance = 13*pow(volts, -1); // worked out from datasheet graph
   return distance;
}*/

bool SensarRealidad::sensarEnfrente() {
	int distancia = 0;
	for(int j = 0; j < 2; j++)
		distancia += sensor[1].readRangeSingleMillimeters();
	distancia /= 20;
	escribirLCD(String(distancia));
	return distancia > 14;
}
bool SensarRealidad::sensarDerecha() {
	int distancia = 0;
	for(int j = 0; j < 2; j++)
		distancia += sensor[0].readRangeSingleMillimeters();
	distancia /= 20;
	escribirLCD(String(distancia));
	return distancia > 14;
}
bool SensarRealidad::sensarAtras() {
	int distancia = 0;
	for(int j = 0; j < 2; j++)
		distancia += sensor[3].readRangeSingleMillimeters();
	distancia /= 20;
	escribirLCD(String(distancia));
	return distancia > 14;
}
bool SensarRealidad::sensarIzquierda() {
	int distancia = 0;
	for(int j = 0; j < 2; j++)
		distancia += sensor[2].readRangeSingleMillimeters();
	distancia /= 20;
	escribirLCD(String(distancia));
	return distancia > 14;
}
uint8_t SensarRealidad::sensarEnfrentePared() {
	int distancia = 0;
	for(int j = 0; j < 2; j++)
		distancia += sensor[1].readRangeSingleMillimeters();
	distancia /= 20;
	escribirLCD(String(distancia));
	return distancia;
}
uint8_t SensarRealidad::sensarDerechaPared() {
	int distancia = 0;
	for(int j = 0; j < 2; j++)
		distancia += sensor[0].readRangeSingleMillimeters();
	distancia /= 20;
	escribirLCD(String(distancia));
	return distancia;
}
uint8_t SensarRealidad::sensarIzquierdaPared() {
	int distancia = 0;
	for(int j = 0; j < 2; j++)
		distancia += sensor[2].readRangeSingleMillimeters();
	distancia /= 20;
	escribirLCD(String(distancia));
	return distancia;
}

int SensarRealidad::sensarRampa() {
  sensors_event_t event;
  bno.getEvent(&event);
  return int(event.orientation.y);
}

float SensarRealidad::sensarRampaFloat() {
  sensors_event_t event;
  bno.getEvent(&event);
  return event.orientation.y;
}

float SensarRealidad::sensarOrientacion() {
  sensors_event_t event;
  bno.getEvent(&event);
  return float(event.orientation.x);
}


//No falta cuando ambos estan presionados?
uint8_t SensarRealidad::switches() {
	if(digitalRead(switchIzquierda) == 1)
		return 1;
	if(digitalRead(switchDerecha) == 1)
		return 2;
	return 0;
}

uint8_t SensarRealidad::switchesIMU(float fDeseado, float grados) {
	if(abs(fDeseado - grados) <= toleranciaSwitchIMU)
		return 0;
	return fDeseado > grados ? 1 : 2;
}
//1 = negro 0 = blanco
bool SensarRealidad::color() {
	char cc = 0;
	while(Serial2.available())
		cc = (char)Serial2.read();
	return(cc&0b00001000);
}

void escribirEEPROM(int dir, int val) {
	byte lowByte = ((val >> 0) & 0xFF);
	byte highByte = ((val >> 8) & 0xFF);

	EEPROM.write(dir, lowByte);
	EEPROM.write(dir + 1, highByte);
}

int leerEEPROM(int dir) {
	byte lowByte = EEPROM.read(dir);
	byte highByte = EEPROM.read(dir + 1);

	return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}

void SensarRealidad::escribirLCD(String sE1, String sE2){
	lcd.clear();
	lcd.print(sE1);
	lcd.setCursor(0, 1);
    lcd.print(sE2);
}
