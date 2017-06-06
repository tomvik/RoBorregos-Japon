#include "Arduino.h"
#include "SensarRealidad.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <EEPROM.h>
/*
#define sIFtrig 30
#define sIFecho 31
#define sIBtrig 32
#define sIBecho 33
*/
//Izquierda adelanto
#define TIzqAde 51
#define EIzqAde 49
//Adelante izquierda
#define TAdeIzq 45
#define EAdeIzq 47
//Adelante derecha
#define TAdeDer 41
#define EAdeDer 43
//Derecha Adelante
#define TDerAde 39
#define EDerAde 37
//Atras
#define TAtr 22
#define EAtr 24

Adafruit_BNO055 bno = Adafruit_BNO055();

#define switchIzquierda 53
#define switchDerecha 35

#define colorIn 23

#define toleranciaSwitchIMU 5

SensarRealidad::SensarRealidad() {
	if(!bno.begin())
 	{
    	/* There was a problem detecting the BNO055 ... check your connections */
    	Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    	//while(1);
  	}
  	bno.setExtCrystalUse(true);
  	Serial.println("A CALIBRAR");
  	//while(getIMUCalStatus() <= 0);
  	Serial.println("TERMINE");
	//Todos los pines declarados
	pinMode(TIzqAde,OUTPUT);
	pinMode(EIzqAde,INPUT);
	pinMode(TAdeIzq,OUTPUT);
	pinMode(EAdeIzq,INPUT);
	pinMode(TAdeDer,OUTPUT);
	pinMode(EAdeDer,INPUT);
	pinMode(TDerAde,OUTPUT);
	pinMode(EDerAde,INPUT);
	pinMode(TAtr, OUTPUT);
	pinMode(EAtr, INPUT);

  	pinMode(switchIzquierda, INPUT);
    pinMode(switchDerecha, INPUT);
    pinMode(colorIn, INPUT);
}
byte SensarRealidad::getIMUCalStatus()
{
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);
  return gyro;
}

uint8_t SensarRealidad::calcDistanciaUS(uint8_t trigger, uint8_t echo) {
   digitalWrite(trigger,LOW);
   delayMicroseconds(5);
   digitalWrite(trigger,HIGH);
   delayMicroseconds(10);
   long tiempo = pulseIn(echo,HIGH);
   return uint8_t(0.017 * tiempo);
}

uint8_t SensarRealidad::calcDistanciaSharp(uint8_t sensor) {
   uint8_t lectura, cm;
   float volts = analogRead(sensor)*0.0048828125;  // value from sensor * (5/1024)
   uint8_t distance = 13*pow(volts, -1); // worked out from datasheet graph
   return distance;
}

bool SensarRealidad::sensarEnfrente() {
	//Con sharp o ultrasonico
	int iD = 0;
	for(uint8_t i = 0; i < 2; i++) {
		iD += calcDistanciaUS(TAdeIzq, EAdeIzq);
		delay(10);
		iD += calcDistanciaUS(TAdeDer, EAdeDer);
		delay(10);
		/*iD += calcDistanciaSharp(ShFront);
		delay(10);*/
	}
	iD /= 4;
	//iD2 /= 2;
	//iD /= 20;
	return iD > 14;
}
bool SensarRealidad::sensarAtras() {
	int iD = 0;
	for(uint8_t i = 0; i < 2; i++) {
		iD += calcDistanciaUS(TAtr, EAtr);
		delay(10);
	}
	iD /= 2;
	//Serial.println(iD);
	return iD > 14;
}
uint8_t SensarRealidad::sensarEnfrentePared() {
	int iD = 0;
	for(uint8_t i = 0; i < 2; i++) {
		iD += calcDistanciaUS(TAdeIzq, EAdeIzq);
		delay(10);
		iD += calcDistanciaUS(TAdeDer, EAdeDer);
		delay(10);
		/*iD += calcDistanciaSharp(ShFront);
		delay(10);*/
	}
	iD /= 4;
	return iD;
}
bool SensarRealidad::sensarDerecha() {
	int iD = 0;
	for(uint8_t i = 0; i < 4; i++) {
		iD += calcDistanciaUS(TDerAde, EDerAde);
		delay(10);
		/*iD += calcDistanciaSharp(ShRight);
		delay(10);*/
	}
	iD /= 4;
	//Serial.print("Derecha: "); Serial.println(iD);
	return iD > 14;
}
uint8_t SensarRealidad::sensarDerechaPared() {
	int iD = 0;
	for(uint8_t i = 0; i < 2; i++) {
		iD += calcDistanciaUS(TDerAde, EDerAde);
		delay(10);
	}
	iD /= 2;
	return iD;
}
bool SensarRealidad::sensarIzquierda() {
	int iD = 0;
	for(int i = 0; i < 4; i++) {
		/*iD += calcDistanciaUS(TIzqAtr, EIzqAtr);
		delay(10);*/
		iD += calcDistanciaUS(TIzqAde, EIzqAde);
		delay(10);
		/*iD += calcDistanciaSharp(ShRight);
		delay(10);*/
	}
	iD /= 4;
	//Serial.print("Izquierda:"); Serial.println(iD);
	return iD > 14;
}

uint8_t SensarRealidad::sensarIzquierdaPared() {
	int iD = 0;
	for(int i = 0; i < 2; i++) {
		iD += calcDistanciaUS(TIzqAde, EIzqAde);
		delay(10);
	}
	iD /= 2;
	return iD;
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

uint8_t SensarRealidad::color() {
	return(digitalRead(colorIn));
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
