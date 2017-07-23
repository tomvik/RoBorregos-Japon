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
const uint8_t kXSHUT[kCantVL53] = {27, 25, 23, 29};
const int kMEDIDA_PARED_MM = 150;
const int kDistanciaMinimaVertical = 40;
const int kDistanciaMinimaLados = 60;

#define BOTON_B 31

// LCD
#define I2C_ADDR  0x3F
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);

// IMU
Adafruit_BNO055 bno = Adafruit_BNO055(Adafruit_BNO055::OPERATION_MODE_NDOF_FMC_OFF);

#define toleranciaSwitchIMU 5

// SWITCH
#define switchIzquierda 39
#define switchDerecha 41

// COLOR
#define colorIn 2
/*
   out 2
   s0 3
   s1 4
   s2 5
   s3 6 */

SensarRealidad::SensarRealidad() {
	if(!bno.begin())
		lcd.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
	bno.setExtCrystalUse(true);
	Serial.println("A CALIBRAR");
	while(getIMUCalibrationStatus() <= 0) ;
	Serial.println("TERMINE");
	//Todos los pines declarados

	pinMode(switchIzquierda, INPUT);
	pinMode(switchDerecha, INPUT);
	pinMode(colorIn, INPUT);

	lcd.begin();
	lcd.backlight();
	Wire.begin();
	inicializarSensoresDistancia(4);
	lastAngle = 0.0;
	malo = false;
}

void SensarRealidad::escribirLCDabajo(String sE1) {
	lcd.setCursor(0, 1);
	lcd.print(sE1);
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
		delay(35);
		lcd.backlight();
		delay(35);
	}
}

void SensarRealidad::inicializarSensoresDistancia(const uint8_t kINICIO_I2C) {
	for (int i = 0; i < kCantVL53; i++)
		pinMode(kXSHUT[i], OUTPUT);

	for (int i = 0; i < kCantVL53; i++)
		digitalWrite(kXSHUT[i], LOW);

	delay(75);

	for (int i = 0; i < kCantVL53; i++) {
		//We put it in '1', so it's enabled.
		//*IMPORTANT NOTE* we cannot send a HIGH signal, because we would burn it. So, what we do is put a really high impedance on it. Because by default, that pin is connected to HIGH.
		pinMode(kXSHUT[i], INPUT);
		//Must wait this time for it to actually be enabled
		delay(75);

		//Initialize the sensor
		sensor[i].init(true);

		//Wait for it to be initialized
		delay(75);
		//Set a new address
		//*IMPORTANT NOTE* we have to check that the addresses doesn't match with those of other sensors
		sensor[i].setAddress((uint8_t)(kINICIO_I2C + i));
		delay(75);
		sensor[i].setTimeout(200);
		delay(75);
		sensor[i].startContinuous();
	}
}

int SensarRealidad::getDistanciaEnfrente() {
	int distancia = sensor[0].readRangeContinuousMillimeters();
	return distancia > 1000 ? -1 : (distancia > 20 ? distancia - 20 : 0);
}

int SensarRealidad::getDistanciaDerecha() {
	int distancia = sensor[1].readRangeContinuousMillimeters();
	return distancia > 1000 ? -1 : (distancia > 50 ? distancia - 50 : 0);
}

int SensarRealidad::getDistanciaAtras() {
	int distancia = sensor[2].readRangeContinuousMillimeters();
	return distancia > 1000 ? -1 : (distancia > 30 ? distancia - 30 : 0);
}

int SensarRealidad::getDistanciaIzquierda() {
	int distancia = sensor[3].readRangeContinuousMillimeters();
	return distancia > 1000 ? -1 : (distancia > 55 ? distancia - 55 : 0);
}

bool SensarRealidad::caminoEnfrente() {
	int distancia = getDistanciaEnfrente();
	return distancia > kMEDIDA_PARED_MM || distancia == -1;
}

bool SensarRealidad::caminoDerecha() {
	int distancia = getDistanciaDerecha();
	return distancia > kMEDIDA_PARED_MM || distancia == -1;
}

bool SensarRealidad::caminoAtras() {
	int distancia = getDistanciaAtras();
	return distancia > kMEDIDA_PARED_MM || distancia == -1;
}

bool SensarRealidad::caminoIzquierda() {
	int distancia = getDistanciaIzquierda();
	return distancia > kMEDIDA_PARED_MM || distancia == -1;
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

bool SensarRealidad::getAngulo(double &angle) {
	double temp = lastAngle;
	sensors_event_t event;
	bno.getEvent(&event);
	angle = event.orientation.x;

	if(angle < 35 && lastAngle > 325) {
		temp -= 360;
	} else if(lastAngle < 35 && angle > 325) {
		temp += 360;
	}
	lastAngle = angle;
	return !(abs(temp - angle) > 35);
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

//2 = checkpoint 1 = negro 0 = blanco
uint8_t SensarRealidad::color() {
	char cc = 0;
  uint8_t iR = 0;

  Serial2.print("M");
	unsigned long inicio = millis();
  while(!Serial2.available() && !malo){
    escribirLCD("NO HAY NADA");
    delay(1);

		if(inicio + 1000 < millis())
			malo = true;
  }
  while(Serial2.available())
    cc = (char)Serial2.read();

  if(cc&0b00001000)
    iR = 1;
  else if(cc&0b00010000)
    iR = 2;

  return iR;
}

bool SensarRealidad::visual(){
  char cc = 0;
  while(Serial2.available())
    cc = (char)Serial2.read();
  return cc&0b00100000;
}

void SensarRealidad::test() {
	double angles;
	escribirLCD("   DISTANCIAS");
	delay(500);
	while(digitalRead(BOTON_B))
		escribirLCD(String(getDistanciaDerecha()) + "    " + String(getDistanciaAtras()) + "    " + String(getDistanciaIzquierda()), "      " + String(getDistanciaEnfrente()));

	escribirLCD("      IMU");
	delay(500);
	while(digitalRead(BOTON_B)) {
		getAngulo(angles);
		escribirLCD("      " + String(angles), "      " + String(sensarRampa()));
	}

	escribirLCD("     COLOR");
	delay(500);
	while(digitalRead(BOTON_B)) {
		escribirLCD(String(color()));
    delay(40);
	}

	// IMU RAMPA
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
