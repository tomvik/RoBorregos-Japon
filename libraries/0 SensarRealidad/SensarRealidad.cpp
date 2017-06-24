#include <Arduino.h>
#include <SensarRealidad.h>
#include <EEPROM.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <LiquidCrystal_I2C.h>

// VL53L0X
// enfrente, derecha, atras, izquerda
const uint8_t kCantVL53 = 4;
VL53L0X sensor[4];
const uint8_t kXSHUT[4] = {25, 27, 29, 23};

void SensarRealidad::inicializar(int x) {
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
     sensor[i].setAddress((uint8_t)(x + i));
     delay(200);
   	   sensor[i].setTimeout(500);
			 /*delay(200);
			 sensor[i].startContinuous(5);
			 delay(200);*/
   }
}

// LCD
#define I2C_ADDR  0x3F
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);

// IMU
Adafruit_BNO055 bno = Adafruit_BNO055(/*Adafruit_BNO055::OPERATION_MODE_GYRONLY*/);

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
  	//while(getIMUCalStatus() <= 0);
  	Serial.println("TERMINE");
	//Todos los pines declarados

	pinMode(switchIzquierda, INPUT);
	pinMode(switchDerecha, INPUT);
	pinMode(colorIn, INPUT);

	lcd.begin();// Indicamos medidas de LCD
	lcd.backlight();

	Wire.begin();

	inicializar(4);
}


byte SensarRealidad::getIMUCalStatus() {
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);
  return gyro;
}

bool SensarRealidad::sensarEnfrente() {
	int distancia = 0;
  for(int j = 0; j < 2; j++)
      distancia += sensor[0].readRangeSingleMillimeters();
  distancia /= 20;
	return distancia > 14;
}

bool SensarRealidad::sensarAtras() {
  int distancia = 0;
  for(int j = 0; j < 2; j++)
      distancia += sensor[2].readRangeSingleMillimeters();
  distancia /= 20;
	return distancia > 14;
}
uint8_t SensarRealidad::sensarEnfrentePared() {
  int distancia = 0;
  for(int j = 0; j < 2; j++)
      distancia += sensor[0].readRangeSingleMillimeters();
  distancia /= 20;
  return distancia;
}
bool SensarRealidad::sensarDerecha() {
  int distancia = 0;
  for(int j = 0; j < 2; j++)
      distancia += sensor[1].readRangeSingleMillimeters();
  distancia /= 20;
	return distancia > 14;
}
uint8_t SensarRealidad::sensarDerechaPared() {
  int distancia = 0;
  for(int j = 0; j < 2; j++)
      distancia += sensor[1].readRangeSingleMillimeters();
  distancia /= 20;
  return distancia;
}
bool SensarRealidad::sensarIzquierda() {
  int distancia = 0;
  for(int j = 0; j < 2; j++)
      distancia += sensor[3].readRangeSingleMillimeters();
  distancia /= 20;
	return distancia > 14;
}

uint8_t SensarRealidad::sensarIzquierdaPared() {
  int distancia = 0;
  for(int j = 0; j < 2; j++)
      distancia += sensor[3].readRangeSingleMillimeters();
  distancia /= 20;
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

float SensarRealidad::getAngulo() {
  sensors_event_t event;
  bno.getEvent(&event);
  return event.orientation.x;
}




void SensarRealidad::imu() {
	sensors_event_t event;
  bno.getEvent(&event);

  /* Board layout:
         +----------+
         |         *| RST   PITCH  ROLL  HEADING
     ADR |*        *| SCL
     INT |*        *| SDA     ^            /->
     PS1 |*        *| GND     |            |
     PS0 |*        *| 3VO     Y    Z-->    \-X
         |         *| VIN
         +----------+
  */

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

void SensarRealidad::apantallanteLCD(String sE1, String sE2){
	escribirLCD(sE1, sE2);
  for (size_t i = 0; i < 4; i++) {
    lcd.noBacklight();
    delay(40);
    lcd.backlight();
    delay(40);
  }
}
