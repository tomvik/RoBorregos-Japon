/* Code made by the RoBorregos team in 2017 for the RoboCup JR. Rescue Maze category.
 * Tomás Lugo, Sebastián Esquer, Ernesto Cervantez, and Alexis Virgen.
 * "El Mariachi" Achieved a third place on the international RoboCup.
 *
 * This class has all the functions to check all the sensors and return the data in a way
 * it can be easily used. It also recieves the communication with the nano
 *
 * For more information, check the SensarRealidad.h file
 */
#include <Arduino.h>
#include <SensarRealidad.h>
#include <Wire.h>
#include <EEPROM.h>
#include <VL53L0X.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <LiquidCrystal_I2C.h>

// Quantity of VL53L0X sensors
const uint8_t kCantVL53 = 4;
//Array of the sensors
VL53L0X sensor[kCantVL53];
//Array of the xSHUT pin of each sensor
const uint8_t kXSHUT[kCantVL53] = {27, 25, 23, 29};
//Threshold to say if there's a wall
const int kMEDIDA_PARED_MM = 150;

//Don't remember what where these for. Unused variables apparently.
/*
const int kDistanciaMinimaVertical = 40;
const int kDistanciaMinimaLados = 60;
*/
//Pin of the button
#define BOTON_B 31

// LCD Adress and initialization
#define I2C_ADDR  0x3F
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);

// IMU Initialization with it's operational mode
Adafruit_BNO055 bno = Adafruit_BNO055(Adafruit_BNO055::OPERATION_MODE_NDOF_FMC_OFF);

//Tolerance for the IMU
#define toleranciaSwitchIMU 5

//Physical switch pins
#define switchIzquierda 39
#define switchDerecha 41

SensarRealidad::SensarRealidad() {
	if(!bno.begin())
		lcd.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
	bno.setExtCrystalUse(true);
	//Here is where I think we could calibrate the IMU until it's good enough, or a certain time has passed.
	Serial.println("A CALIBRAR");
	while(getIMUCalibrationStatus() <= 0) ;
	Serial.println("TERMINE");

	pinMode(switchIzquierda, INPUT);
	pinMode(switchDerecha, INPUT);

	lcd.begin();
	lcd.backlight();
	Wire.begin();
	escribirLCD("LCD lista");
	//Sometimes it would crash in here... I2C issues.
	inicializarSensoresDistancia(4);
	lastAngle = 0.0;
	malo = false;
}

//One thing we did to try and solve the i2c problem was to put a small delay between each function that used it.
//Because one possibility was that there was a problem of syncronization. Didn't work at all.
void SensarRealidad::escribirLCDabajo(String sE1) {
	//delay(1);
	lcd.setCursor(0, 1);
	//delay(1);
	lcd.print(sE1);
}

void SensarRealidad::escribirLCD(String sE1, String sE2) {
	//delay(1);
	lcd.clear();
	//delay(1);
	lcd.print(sE1);
	//delay(1);
	lcd.setCursor(0, 1);
	//delay(1);
	lcd.print(sE2);
}

void SensarRealidad::apantallanteLCD(String sE1, String sE2) {
	escribirLCD(sE1, sE2);
	//delay(1);
	for (size_t i = 0; i < 4; i++) {
		lcd.noBacklight();
		delay(30);
		lcd.backlight();
		delay(30);
	}
}

void SensarRealidad::inicializarSensoresDistancia(const uint8_t kINICIO_I2C) {
	for (int i = 0; i < kCantVL53; i++)
		pinMode(kXSHUT[i], OUTPUT);

	for (int i = 0; i < kCantVL53; i++)
		digitalWrite(kXSHUT[i], LOW);

	delay(70);

	for (int i = 0; i < kCantVL53; i++) {
		//We put it in '1', so it's enabled.
		//*IMPORTANT NOTE* we cannot send a HIGH signal, because we would burn it. So, what we do is put a really high impedance on it. Because by default, that pin is connected to HIGH.
		pinMode(kXSHUT[i], INPUT);
		//Must wait this time for it to actually be enabled
		delay(70);

		//Initialize the sensor
		sensor[i].init(true);

		//Wait for it to be initialized
		delay(70);
		//Set a new address
		//*IMPORTANT NOTE* we have to check that the addresses doesn't match with those of other sensors
		sensor[i].setAddress((uint8_t)(kINICIO_I2C + i));
		delay(70);
		sensor[i].setTimeout(200);
		delay(70);
		sensor[i].startContinuous();
	}
}
//We return a -1 when it's out of reach (around 100cm), because at farther distances weird things happened.
//This sensors are really tricky
int SensarRealidad::getDistanciaEnfrente() {
	//delay(2);
	int distancia = sensor[0].readRangeContinuousMillimeters();
	return distancia > 1000 ? -1 : (distancia > 20 ? distancia - 20 : 0);
}

int SensarRealidad::getDistanciaDerecha() {
	//delay(2);
	int distancia = sensor[1].readRangeContinuousMillimeters();
	return distancia > 1000 ? -1 : (distancia > 50 ? distancia - 50 : 0);
}

int SensarRealidad::getDistanciaAtras() {
	//delay(2);
	int distancia = sensor[2].readRangeContinuousMillimeters();
	return distancia > 1000 ? -1 : (distancia > 30 ? distancia - 30 : 0);

}

int SensarRealidad::getDistanciaIzquierda() {
	//delay(2);
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
	delay(2);
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
	delay(2);
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
    	escribirLCD("NO HAY NADA 2");
    	delay(2);
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
	delay(650);
	while(digitalRead(BOTON_B)) {
		escribirLCD(String(getDistanciaDerecha()) + "    " + String(getDistanciaAtras()) + "    " + String(getDistanciaIzquierda()), "      " + String(getDistanciaEnfrente()));
		delay(35);
	}
	escribirLCD("      IMU");
	delay(650);
	while(digitalRead(BOTON_B)) {
		getAngulo(angles);
		escribirLCD("      " + String(angles), "      " + String(sensarRampa()));
		delay(35);
	}

	escribirLCD("     COLOR");
	delay(650);
	while(digitalRead(BOTON_B)) {
		escribirLCD(String(color()));
    delay(35);
	}

	escribirLCD("     SWITCH");
	delay(650);
	while(digitalRead(BOTON_B)) {
		escribirLCD(String(switches()));
    delay(35);
	}

	escribirLCD("     VICTIMAS");
	delay(650);
	char cVictima;
	uint8_t mo = 0;
	while(digitalRead(BOTON_B)) {
		while(Serial2.available()) {
			cVictima = (char)Serial2.read();
		}
		if(cVictima&0b00000010)
    		uint8_t mo = (cVictima&0b00000001) ? 1 : 2;
		escribirLCD(String(mo));
    delay(100);
	}
}

void escribirEEPROM(int dir, int val) {
	byte lowByte = ((val >> 0) & 0xFF);
	byte highByte = ((val >> 8) & 0xFF);

	EEPROM.write(dir, lowByte);
	EEPROM.write(dir + 1, highByte);
}

bool SensarRealidad::getMalo(){
	return malo;
}

int leerEEPROM(int dir) {
	byte lowByte = EEPROM.read(dir);
	byte highByte = EEPROM.read(dir + 1);

	return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}