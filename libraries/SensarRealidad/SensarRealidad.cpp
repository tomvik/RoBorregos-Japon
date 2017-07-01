#include <Arduino.h>
#include <SensarRealidad.h>
#include <Wire.h>
#include <EEPROM.h>
#include <VL53L0X.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <LiquidCrystal_I2C.h>

// VL53L0X
const uint8_t kCantVL53 = 4;

VL53L0X sensor[kCantVL53];
const uint8_t kXSHUT[kCantVL53] = {29, 27, 23, 25};
const int kMEDIDA_PARED_MM = 150;

// LCD
#define I2C_ADDR  0x3F
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);

// IMU
Adafruit_BNO055 bno = Adafruit_BNO055();

#define toleranciaSwitchIMU 5

// SWITCH
#define switchIzquierda 53
#define switchDerecha 35

// COLOR
#define colorIn 2
/*
   out 2
   s0 3
   s1 4
   s2 5
   s3 6

   nano serial 3
   master serial 2*/

SensarRealidad::SensarRealidad() {
	if(!bno.begin())
		lcd.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
	bno.setExtCrystalUse(true);
	Serial.println("A CALIBRAR");
	//while(getIMUCalibrationStatus() <= 0);
	Serial.println("TERMINE");
	//Todos los pines declarados

	pinMode(switchIzquierda, INPUT);
	pinMode(switchDerecha, INPUT);
	pinMode(colorIn, INPUT);

	lcd.begin();
	lcd.backlight();
	Wire.begin();
	inicializarSensoresDistancia(4);
}

void SensarRealidad::escribirLCD(String sE1, String sE2) {
	lcd.clear();
	lcd.print(sE1);
	lcd.setCursor(0, 1);
	lcd.print(sE2);
}

void SensarRealidad::apantallanteLCD(String sE1, String sE2) {
	escribirLCD(sE1, sE2);
	for (size_t i = 0; i < 4; i++) {
		lcd.noBacklight();
		delay(40);
		lcd.backlight();
		delay(40);
	}
}

void SensarRealidad::inicializarSensoresDistancia(const uint8_t kINICIO_I2C) {
	for (int i = 0; i < kCantVL53; i++)
		pinMode(kXSHUT[i], OUTPUT);

	for (int i = 0; i < kCantVL53; i++)
		digitalWrite(kXSHUT[i], LOW);

	delay(1000);

	for (int i = 0; i < kCantVL53; i++) {
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
		sensor[i].setAddress((uint8_t)(kINICIO_I2C + i));
		delay(200);
		sensor[i].setTimeout(200);
		/*delay(200);
		   sensor[i].startContinuous(5);
		   delay(200);*/
	}
}

int SensarRealidad::getDistanciaEnfrente() {
	int distancia = 0;
	for(int j = 0; j < 3; j++)
		distancia += sensor[0].readRangeSingleMillimeters();
	return distancia / 3;
}

int SensarRealidad::getDistanciaDerecha() {
	int distancia = 0;
	for(int j = 0; j < 3; j++)
		distancia += sensor[1].readRangeSingleMillimeters();
	return distancia / 3;
}

int SensarRealidad::getDistanciaAtras() {
	int distancia = 0;
	for(int j = 0; j < 3; j++)
		distancia += sensor[2].readRangeSingleMillimeters();
	return distancia / 3;
}

int SensarRealidad::getDistanciaIzquierda() {
	int distancia = 0;
	for(int j = 0; j < 3; j++)
		distancia += sensor[3].readRangeSingleMillimeters();
	return distancia / 3;
}

bool SensarRealidad::caminoEnfrente() {
	return (getDistanciaEnfrente()) > kMEDIDA_PARED_MM;
}

bool SensarRealidad::caminoDerecha() {
	return (getDistanciaDerecha()) > kMEDIDA_PARED_MM;
}

bool SensarRealidad::caminoAtras() {
	return (getDistanciaAtras()) > kMEDIDA_PARED_MM;
}

bool SensarRealidad::caminoIzquierda() {
	return (getDistanciaIzquierda()) > kMEDIDA_PARED_MM;
}

uint8_t SensarRealidad::getIMUCalibrationStatus() {
	uint8_t system, gyro, accel, mag;
	system = gyro = accel = mag = 0;
	bno.getCalibration(&system, &gyro, &accel, &mag);
	return gyro;
}

void SensarRealidad::calibracionIMU() {
	sensors_event_t event;
	bno.getEvent(&event);

	/* The processing sketch expects data as roll, pitch, heading */
	Serial.print(F("Orientation: "));
	Serial.print((float)event.orientation.x);
	Serial.print(F(" "));
	Serial.print((float)event.orientation.y);
	Serial.print(F(" "));
	Serial.print((float)event.orientation.z);
	Serial.println(F(""));

	/* Also send calibration data for each sensor. */
	uint8_t sys, gyro, accel, mag = 0;
	bno.getCalibration(&sys, &gyro, &accel, &mag);
	Serial.print(F("Calibration: "));
	Serial.print(sys, DEC);
	Serial.print(F(" "));
	Serial.print(gyro, DEC);
	Serial.print(F(" "));
	Serial.print(accel, DEC);
	Serial.print(F(" "));
	Serial.println(mag, DEC);

	delay(100);
}

double SensarRealidad::getAngulo() {
	sensors_event_t event;
	bno.getEvent(&event);
	return event.orientation.x;
}

double SensarRealidad::sensarRampa() {
	sensors_event_t event;
	bno.getEvent(&event);
	return event.orientation.y;
}

uint8_t SensarRealidad::switchesIMU(double fDeseado, double grados) {
	if(abs(fDeseado - grados) <= toleranciaSwitchIMU)
		return 0;
	return fDeseado > grados ? 1 : 2;
}

//No falta cuando ambos estan presionados?
uint8_t SensarRealidad::switches() {
	if(digitalRead(switchIzquierda) == 1)
		return 1;
	if(digitalRead(switchDerecha) == 1)
		return 2;
	return 0;
}

//1 = negro 0 = blanco
bool SensarRealidad::color() {
	char cc = 0;
	while(Serial2.available())
		cc = (char) Serial2.read();
	return(cc & 0b00001000);
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
