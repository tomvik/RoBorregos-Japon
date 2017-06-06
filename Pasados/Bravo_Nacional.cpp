//giroscopio
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU9150Lib.h"
#include "CalLib.h"
#include <dmpKey.h>
#include <dmpmap.h>
#include <inv_mpu.h>
#include <inv_mpu_dmp_motion_driver.h>
#include <EEPROM.h>
#define  DEVICE_TO_USE          0
#define MPU_UPDATE_RATE         (20)
#define MAG_UPDATE_RATE         (10)
#define  MPU_MAG_MIX_GYRO_ONLY  0
#define MPU_LPF_RATE            40
MPU9150Lib MPU;

//microSD
#include <SD.h>
#include <SPI.h>

//pantalla LCD
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#define I2C_ADDR    0x27
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
LiquidCrystal_I2C lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);


//motores listos
#define motorRFF 8
#define motorRFB 9
#define motorRBF 6
#define motorRBB 7
#define motorLFF 12
#define motorLFB 4
#define motorLBF 11
#define motorLBB 10

//sharps
//#define sharp1  A11	//corregir pines
//#define sharp2  A12
//#define sharp3  A13

//ultrasonicos
#define trigger1  27 //IzquierdaAtras
#define echo1     29 
#define trigger2  39 //IzquierdaEnfrente
#define echo2     41  
#define trigger3  31 //FrenteIzquierda
#define echo3     33
#define trigger4  43 //FrenteCentro
#define echo4     45
#define trigger5  35 //FrenteDerecha
#define echo5     37
#define trigger6  47 //DerechaEnfrente
#define echo6     49
#define trigger7  23 //DerechaAtras
#define echo7     25

//color
/*#define s0  A2
#define s1  A3
#define OE  A4*/
#define s3  A5
#define s2  A6
#define out A7

//encoder
int eCount1 = 0; // global para el encoder
int eCount2 = 0;

//medidas de pista
const int piso1X = 5;
const int piso1Y = 3;
const int piso2X = 5;
const int piso2Y = 7;
const int pisoXmax = max(piso1X, piso2X);
const int pisoYmax = max(piso1Y, piso2Y);

//matriz pista
char tab[2*pisoYmax + 1][2*pisoXmax + 1][2];
//char tab[15][11][2];


//seleccion de piso
int iPiso = 0; // 0 o 1

//posicion
int posX[] = {1,1}; //pos x de nivel 1 y nivel 2    x1, x2
int posY[] = {1,1}; //pos y de nivel 1 y nivel 2    y1, y2

bool xMax[] = {false, false}; //pos x maxima de nivel 1 y 2
bool yMax[] = {false, false}; //pos y maxima de nivel 1 y 2

//orientation
char orientation = 'u'; //u,d,r,l (up, down, right, left)

bool b = true;

//int priority[3];

void crearMat() { 
	for(int z = 0; z < 2; z++)
		for (int i = 0; i < 15; i++) //puntos inexistentes
			for(int j = 0; j < 11; j++){
				if(i%2 == 0 && j%2 == 0)
					tab[i][j][z] = 'X';
				if(i%2 == 0 && j%2 == 1)
					tab[i][j][z] = '*';
				if(i%2 == 1 && j%2 == 0)
					tab[i][j][z] = '*';
				if(i%2 == 1 && j%2 == 1)
					tab[i][j][z] = '.';
			}
}

void printMat(){
	char c;
	for (int i = 0; i < 15; i++){
		for(int j = 0; j < 11; j++){
			c = tab[i][j][0];
			if(c == 's' && i%2 == 0) Serial.print("-");
			else if(c == 's') Serial.print("|");
			else if(c == 'n') Serial.print(" ");
			else if(c == 'v') Serial.print("v");
			else if(c == 'X') Serial.print(" ");
			else if(c == '.') Serial.print("∙");
			else if(c == '*') Serial.print("*");
		}
		Serial.println("");
	}

}
double ultrasonico(int trigger, int echo) {   //listo
  digitalWrite (trigger, LOW);
  delayMicroseconds (2);
  digitalWrite (trigger, HIGH);
  delayMicroseconds (5);
  digitalWrite (trigger, LOW);
  double dDistance = pulseIn (echo, HIGH);
  dDistance /= 58;
  return dDistance; 
}

double u10(int trigger, int echo){
  double d = 0;
  for(int i = 0; i < 10; i++){
    d += ultrasonico(trigger, echo);
  }
   d /= 10;
}

int colorSelection() { //identificar color     listo
    int frequency = pulseIn ( out , LOW );
    if ( 60 < frequency && frequency < 90 ) return 0; //blanco
    if ( 10 < frequency && frequency < 45 ) return 1; //plateado
    if ( 300 < frequency && frequency < 340 ) return 2; //negro
    return 3; //error
}

int giroscopioZ(){ //listo
  while(!MPU.read());
  int zAngle = round(MPU.readZAxis(MPU.m_fusedEulerPose));
  return zAngle + 180;
}

int giroscopioY(){ //listo
  while(!MPU.read());
  int yAngle = round(MPU.readYAxis(MPU.m_fusedEulerPose));
  return yAngle;
}

void moverMatAbajo () { //cuando el mov excede hacia arriba
  if(posY[iPiso] != 1) return; // si no es necesario mover la matriz hacia abajo

  for(int i = 2*pisoYmax - 2; i > -1; i--)
    for(int j = 0; j < 2*pisoXmax + 1; j++)
    	tab[i+2][j][iPiso] = tab[i][j][iPiso];
  
  for(int i = 0; i < 2; i++)
    for(int j = 0; j < 2*pisoXmax + 1; j++)
        tab[i][j][iPiso] = '.';

  for(int i = 0; i < 2*pisoXmax + 1; i+=2)
      tab[0][i][iPiso] = 'X';

  posY[iPiso] += 2;
  yMax[iPiso] += 2;
}

void moverMatDerecha () {  //cuando el mov excede hacia la izquierda
  if(posX[iPiso] != 1) return; // si no es necesario mover la matriz hacia la derecha

  for(int i = 2*pisoXmax - 2; i > -1; i--)
  	for(int j = 0; j < 2*pisoYmax + 1; j++)
        tab[j][i+2][iPiso] = tab[j][i][iPiso];
  
  for(int i = 0; i < 2; i++)
    for(int j = 0; j < 2*pisoYmax; j++)
        tab[j][i+2][iPiso] = ' ';

  for(int i = 0; i < 2*pisoYmax; i+=2)
      tab[i][0][iPiso] = 'X';

  posX[iPiso] -= 2;
  xMax[iPiso] += 2;
}

void powerOff(){
  analogWrite(motorRFF, 0);
  analogWrite(motorLFF, 0);
  analogWrite(motorRBF, 0);
  analogWrite(motorLBF, 0);
  analogWrite(motorRFB, 0);
  analogWrite(motorLFB, 0);
  analogWrite(motorRBB, 0);
  analogWrite(motorLBB, 0);
  delay(200);
}

void moveFront(){
	eCount1 = 0;
	eCount2 = 0;
	analogWrite (motorLFF, 185);
    analogWrite (motorLBF, 200);
    analogWrite (motorRFF, 200);
    analogWrite (motorRBF, 200);
	/*analogWrite (motorLFF, 140);
    analogWrite (motorLBF, 150);
    analogWrite (motorRFF, 150);
    analogWrite (motorRBF, 150);*/
    /*delay(100);
    analogWrite(motorRFF, 100);
    analogWrite(motorLFF, 93);
    analogWrite(motorRBF, 100);
    analogWrite(motorLBF, 100);*/
    while( (eCount1 + eCount2)/2 < 1440) {
    	Serial.print("");
	 //   lcd.setCursor(1,0);
	 //   lcd.print(ultrasonico(trigger4, echo4));
    }
    powerOff();
    if(ultrasonico(trigger4, echo4) < 4.5){
    	analogWrite (motorLFB, 63);
	    analogWrite (motorLBB, 70);
	    analogWrite (motorRFB, 70);
	    analogWrite (motorRBB, 70);
	}
    while(ultrasonico(trigger4, echo4) < 4.5){}
   	powerOff();
   	switch (orientation){
	     case 'u': moverMatAbajo(); posY[iPiso] -= 2; break;
	     case 'd': posY[iPiso] += 2; break;
	     case 'r': posX[iPiso] += 2; break;
	     case 'l': moverMatDerecha(); posX[iPiso] -= 2; break;
	}
	yMax[iPiso] = yMax[iPiso] || posY[iPiso] == pisoYmax - 2;
	xMax[iPiso] = xMax[iPiso] || posX[iPiso] == pisoXmax - 2;
}

void turnRight() {
    int posicionInicial = giroscopioZ();
    analogWrite (motorLFF, 140);
    analogWrite (motorLBF, 150);
    analogWrite (motorRFB, 150);
    analogWrite (motorRBB, 150);
    delay(100);
    analogWrite (motorLFF, 63);
    analogWrite (motorLBF, 80);
    analogWrite (motorRFB, 80);
    analogWrite (motorRBB, 80);
    //Serial.print("posicionInicial = ");
    Serial.println(posicionInicial);
    if (posicionInicial >= 270)
      while(giroscopioZ() >= posicionInicial || giroscopioZ() < posicionInicial - 278)
      {
        //Serial.println(giroscopioZ());
      }  
    else while(giroscopioZ() < posicionInicial + 82)
      {
        //Serial.println(giroscopioZ());
      }
    powerOff();
	switch (orientation){
      case 'u': orientation = 'r'; break;
      case 'd':	orientation = 'l'; break;
      case 'r': orientation = 'd'; break;
      case 'l': orientation = 'u'; break;
	}
}

void turnLeft() {
    int posicionInicial = giroscopioZ();
    analogWrite (motorLFB, 140);
    analogWrite (motorLBB, 150);
    analogWrite (motorRFF, 150);
    analogWrite (motorRBF, 150);
    delay(100);
    analogWrite (motorLFB, 68);
    analogWrite (motorLBB, 80);
    analogWrite (motorRFF, 80);
    analogWrite (motorRBF, 80);
    //Serial.print("posicionInicial = ");
    Serial.println(posicionInicial);
    if (posicionInicial <= 90)
      while(giroscopioZ() <= posicionInicial || giroscopioZ() > posicionInicial + 278)
      {
        //Serial.println(giroscopioZ());
      }  
    else while(giroscopioZ() > posicionInicial - 82)
      {
        //Serial.println(giroscopioZ());
      }
    powerOff();
    switch (orientation){
      case 'u': orientation = 'l'; break;
      case 'd':	orientation = 'r'; break;
      case 'r': orientation = 'u'; break;
      case 'l': orientation = 'd'; break;
	}
}

void moverDerecha(){ //align
	analogWrite(motorLFF,150);
	analogWrite(motorLBB,150);
	analogWrite(motorRFB,150);
	analogWrite(motorRBF,150);
	delay(30);
	powerOff(); 
}

void moverIzquierda(){ //align
	analogWrite(motorLFB,150);
	analogWrite(motorRFF,150);
	analogWrite(motorLBF,150);
	analogWrite(motorRBB,150);
	delay(30);
	powerOff();
}

void PonteBienIzquierda() { //align
	double a = u10(trigger2, echo2);
	double b = u10(trigger1, echo1);
	//lcd.setCursor(1,1);
	//lcd.print(abs(a-b));
	if( abs(a-b) > 1 ){
	    if(a > b){
	      	while( ultrasonico(trigger2, echo2) > ultrasonico(trigger1, echo1)) {
				lcd.setCursor(1,0);
				lcd.print("2 es mayor      ");
				analogWrite (motorLFB, 93);
				analogWrite (motorLBB, 100);
				analogWrite (motorRFF, 100);
				analogWrite (motorRBF, 100);
				delay(20);
				powerOff();
				delay(200);
	        }
	    } else{
	    	while(ultrasonico(trigger1, echo1) > ultrasonico(trigger2, echo2)){
		        lcd.setCursor(1,0);
		        lcd.print("1 es mayor     ");
		        analogWrite (motorLFF, 93);
		        analogWrite (motorLBF, 100);
		        analogWrite (motorRFB, 100);
		        analogWrite (motorRBB, 100);
		        delay(20);
		        powerOff();
		        delay(200);
	    	}
	      //delay(500);
	    }
  	}

  	double d = ultrasonico(trigger2, echo2);
	if(d < 15){
    	if(d < 5.5)
      		while(ultrasonico(trigger2, echo2) < 5.5)
        		moverDerecha();
    	else
      		while(ultrasonico(trigger2, echo2) > 5.5)
        		moverIzquierda();
  	} else if(d < 45){
    	if(d < 35.5)
      		while(ultrasonico(trigger2, echo2) < 35.5)
        		moverDerecha();
	    else
	      	while(ultrasonico(trigger2, echo2) > 35.5)
	        	moverIzquierda();
	} else if(d < 75){
	    if(d < 65.5)
	      	while(ultrasonico(trigger2, echo2) < 65.5)
	        	moverDerecha();
	    else
	      	while(ultrasonico(trigger2, echo2) > 65.5)
	        	moverIzquierda();
	} else if(d < 105){
	    if(d < 95.5)
	      	while(ultrasonico(trigger2, echo2) < 95.5)
	        	moverDerecha();
	    else
	      	while(ultrasonico(trigger2, echo2) > 95.5)
	        	moverIzquierda();
	}
}

void PonteBienDerecha() { //align
	double a = u10(trigger7, echo7);
	double b = u10(trigger6, echo6);
	//lcd.setCursor(1,1);
	//lcd.print(abs(a-b));
	if( abs(a-b) > 1 ){
	    if(a > b){
	      	while( ultrasonico(trigger7, echo7) > ultrasonico(trigger6, echo6)) {
				lcd.setCursor(1,0);
				lcd.print("2 es mayor      ");
				analogWrite (motorLFB, 93);
				analogWrite (motorLBB, 100);
				analogWrite (motorRFF, 100);
				analogWrite (motorRBF, 100);
				delay(20);
				powerOff();
				delay(200);
	        }
	    } else{
	    	while(ultrasonico(trigger6, echo6) > ultrasonico(trigger7, echo7)){
		        lcd.setCursor(1,0);
		        lcd.print("1 es mayor     ");
		        analogWrite (motorLFF, 93);
		        analogWrite (motorLBF, 100);
		        analogWrite (motorRFB, 100);
		        analogWrite (motorRBB, 100);
		        delay(20);
		        powerOff();
		        delay(200);
	    	}
	      //delay(500);
	    }
  	}

  	double d = ultrasonico(trigger6, echo6);
	if(d < 15){
    	if(d < 5.5)
      		while(ultrasonico(trigger6, echo6) < 5.5)
        		moverIzquierda();
    	else
      		while(ultrasonico(trigger6, echo6) > 5.5)
        		moverDerecha();
  	} else if(d < 45){
    	if(d < 35.5)
      		while(ultrasonico(trigger6, echo6) < 35.5)
        		moverIzquierda();
	    else
	      	while(ultrasonico(trigger6, echo6) > 35.5)
	        	moverDerecha();
	} else if(d < 75){
	    if(d < 65.5)
	      	while(ultrasonico(trigger6, echo6) < 65.5)
	        	moverIzquierda();
	    else
	      	while(ultrasonico(trigger6, echo6) > 65.5)
	        	moverDerecha();
	} else if(d < 105){
	    if(d < 95.5)
	      	while(ultrasonico(trigger6, echo6) < 95.5)
	        	moverIzquierda();
	    else
	      	while(ultrasonico(trigger6, echo6) > 95.5)
	        	moverDerecha();
	}
}

void Align(){
	if( (ultrasonico(trigger1, echo1)+ultrasonico(trigger1, echo1))/2 < (ultrasonico(trigger6, echo6)+ultrasonico(trigger7, echo7))/2 ) PonteBienIzquierda();
	else PonteBienDerecha();
}

void mapTile(){
	/*if(giroscopioY() > 20) { //subir
		analogWrite(motorRFF, 150);
	    analogWrite(motorLFF, 150);
	    analogWrite(motorRBF, 150);
	    analogWrite(motorLBF, 150);
		while(giroscopioY() > 3);
		analogWrite(motorRFF, 80);
	    analogWrite(motorLFF, 80);
	    analogWrite(motorRBF, 80);
	    analogWrite(motorLBF, 80);
	    while(ultrasonico(trigger1, echo1) > 5.5);
		powerOff();
		iPiso = iPiso + 1 %2;
		movSensores();
		movSensores();
		return;
	}

	if(giroscopioY() < -20) {//bajar
		analogWrite(motorRFF, 50);
	    analogWrite(motorLFF, 50);
	    analogWrite(motorRBF, 50);
	    analogWrite(motorLBF, 50);
		while(giroscopioY() < -3);
		analogWrite(motorRFF, 80);
	    analogWrite(motorLFF, 80);
	    analogWrite(motorRBF, 80);
	    analogWrite(motorLBF, 80);
	    while(ultrasonico(trigger1, echo1) > 5.5);
		powerOff();
		iPiso = iPiso + 1 %2;
		movSensores(); //gira
		movSensores(); //entra
		return;
	}*/
}

void map(){
	tab [posY[iPiso]] [posX[iPiso]] [iPiso] = 'v';


	if(ultrasonico(trigger4, echo4) < 20)
		switch(orientation){
	      case 'u': tab[posY[iPiso]][posX[iPiso] - 1][iPiso] = 's'; if(tab[posY[iPiso]][posX[iPiso] - 2][iPiso] != 'v') tab[posY[iPiso]][posX[iPiso] - 2][iPiso] = 'p'; break;
	      case 'd':	tab[posY[iPiso]][posX[iPiso] + 1][iPiso] = 's'; if(tab[posY[iPiso]][posX[iPiso] + 2][iPiso] != 'v') tab[posY[iPiso]][posX[iPiso] + 2][iPiso] = 'p'; break;
	      case 'r': tab[posY[iPiso] + 1][posX[iPiso]][iPiso] = 's'; if(tab[posY[iPiso] + 2][posX[iPiso]][iPiso] != 'v') tab[posY[iPiso] + 2][posX[iPiso]][iPiso] = 'p'; break;
	      case 'l': tab[posY[iPiso] - 1][posX[iPiso]][iPiso] = 's'; if(tab[posY[iPiso] - 2][posX[iPiso]][iPiso] != 'v') tab[posY[iPiso] - 2][posX[iPiso]][iPiso] = 'p'; break;
		}
	else
		switch (orientation){
	      case 'u': tab[posY[iPiso]][posX[iPiso] - 1][iPiso] = 'n'; if(tab[posY[iPiso]][posX[iPiso] - 2][iPiso] != 'v') tab[posY[iPiso]][posX[iPiso] - 2][iPiso] = 'p'; break;
	      case 'd':	tab[posY[iPiso]][posX[iPiso] + 1][iPiso] = 'n'; if(tab[posY[iPiso]][posX[iPiso] + 2][iPiso] != 'v') tab[posY[iPiso]][posX[iPiso] + 2][iPiso] = 'p'; break;
	      case 'r': tab[posY[iPiso] + 1][posX[iPiso]][iPiso] = 'n'; if(tab[posY[iPiso] + 2][posX[iPiso]][iPiso] != 'v') tab[posY[iPiso] + 2][posX[iPiso]][iPiso] = 'p'; break;
	      case 'l': tab[posY[iPiso] - 1][posX[iPiso]][iPiso] = 'n'; if(tab[posY[iPiso] - 2][posX[iPiso]][iPiso] != 'v') tab[posY[iPiso] - 2][posX[iPiso]][iPiso] = 'p'; break;			
		}
	if( (ultrasonico(trigger6, echo6) + ultrasonico(trigger7, echo7))/2 < 20)
		switch(orientation){
	      case 'u': tab[posY[iPiso] + 1][posX[iPiso]][iPiso] = 's'; if(tab[posY[iPiso] + 2][posX[iPiso]][iPiso] != 'v') tab[posY[iPiso] + 2][posX[iPiso]][iPiso] = 'p'; break;
	      case 'd':	tab[posY[iPiso - 1]][posX[iPiso]][iPiso] = 's'; if(tab[posY[iPiso] - 2][posX[iPiso]][iPiso] != 'v') tab[posY[iPiso] - 2][posX[iPiso]][iPiso] = 'p'; break;
	      case 'r': tab[posY[iPiso]][posX[iPiso] + 1][iPiso] = 's'; if(tab[posY[iPiso]][posX[iPiso] + 2][iPiso] != 'v') tab[posY[iPiso]][posX[iPiso] + 2][iPiso] = 'p'; break;
	      case 'l': tab[posY[iPiso]][posX[iPiso] - 1][iPiso] = 's'; if(tab[posY[iPiso]][posX[iPiso] - 2][iPiso] != 'v') tab[posY[iPiso]][posX[iPiso] - 2][iPiso] = 'p'; break;
		}
	else
		switch (orientation){
	      case 'u': tab[posY[iPiso] + 1][posX[iPiso]][iPiso] = 'n'; if(tab[posY[iPiso] + 2][posX[iPiso]][iPiso] != 'v') tab[posY[iPiso] + 2][posX[iPiso]][iPiso] = 'p'; break;
	      case 'd':	tab[posY[iPiso - 1]][posX[iPiso]][iPiso] = 'n'; if(tab[posY[iPiso] - 2][posX[iPiso]][iPiso] != 'v') tab[posY[iPiso] - 2][posX[iPiso]][iPiso] = 'p'; break;
	      case 'r': tab[posY[iPiso]][posX[iPiso] + 1][iPiso] = 'n'; if(tab[posY[iPiso]][posX[iPiso] + 2][iPiso] != 'v') tab[posY[iPiso]][posX[iPiso] + 2][iPiso] = 'p'; break;
	      case 'l': tab[posY[iPiso]][posX[iPiso] - 1][iPiso] = 'n'; if(tab[posY[iPiso]][posX[iPiso] - 2][iPiso] != 'v') tab[posY[iPiso]][posX[iPiso] - 2][iPiso] = 'p'; break;
	  	}
	if((ultrasonico(trigger1, echo1) + ultrasonico(trigger2, echo2))/2 < 20)
		switch(orientation){
	      case 'u': tab[posY[iPiso] - 1][posX[iPiso]][iPiso] = 's'; if(tab[posY[iPiso] - 2][posX[iPiso]][iPiso] != 'v') tab[posY[iPiso] - 2][posX[iPiso]][iPiso] = 'p'; break;
	      case 'd':	tab[posY[iPiso] + 1][posX[iPiso]][iPiso] = 's'; if(tab[posY[iPiso] + 2][posX[iPiso]][iPiso] != 'v') tab[posY[iPiso] + 2][posX[iPiso]][iPiso] = 'p'; break;
	      case 'r': tab[posY[iPiso]][posX[iPiso] - 1][iPiso] = 's'; if(tab[posY[iPiso]][posX[iPiso] - 2][iPiso] != 'v') tab[posY[iPiso]][posX[iPiso] - 2][iPiso] = 'p'; break;
	      case 'l': tab[posY[iPiso]][posX[iPiso] + 1][iPiso] = 's'; if(tab[posY[iPiso]][posX[iPiso] + 2][iPiso] != 'v') tab[posY[iPiso]][posX[iPiso] + 2][iPiso] = 'p'; break;
		}
	else
		switch (orientation){
	      case 'u': tab[posY[iPiso] - 1][posY[iPiso]][iPiso] = 'n'; if(tab[posY[iPiso] - 2][posY[iPiso]][iPiso] != 'v') tab[posY[iPiso] - 2][posY[iPiso]][iPiso] = 'p'; break;
	      case 'd':	tab[posY[iPiso] + 1][posY[iPiso]][iPiso] = 'n'; if(tab[posY[iPiso] + 2][posY[iPiso]][iPiso] != 'v') tab[posY[iPiso] + 2][posY[iPiso]][iPiso] = 'p'; break;
	      case 'r': tab[posY[iPiso]][posY[iPiso] - 1][iPiso] = 'n'; if(tab[posY[iPiso]][posY[iPiso] - 2][iPiso] != 'v') tab[posY[iPiso]][posY[iPiso] - 2][iPiso] = 'p'; break;
	      case 'l': tab[posY[iPiso]][posY[iPiso] + 1][iPiso] = 'n'; if(tab[posY[iPiso]][posY[iPiso] + 2][iPiso] != 'v') tab[posY[iPiso]][posY[iPiso] + 2][iPiso] = 'p'; break;
		}
}

bool pChar(int a, int b){
	int pY = posY[iPiso] + a;
	int pX = posX[iPiso] + b;

	if(pY < 0 || pY > pisoYmax)
		return false;
	if(pX < 0 || pX > pisoXmax)
		return false;

	if(tab[pY][pX][iPiso] == 'p')
		return true;

	return false;
}

void closestP(){
}

void algo(){
	map();
	switch(orientation){
		case 'u':
			if(pChar(0,2)){ lcd.setCursor(1,0); lcd.print("..right.."); turnRight(); moveFront(); Align();}
			else if(pChar(-2,0)){ lcd.setCursor(1,0); lcd.print("..front.."); moveFront(); Align();}
			else if(pChar(0,-2)){ lcd.setCursor(1,0); lcd.print("..left.."); turnLeft(); moveFront(); Align();}
			else if(pChar(2,0)){ lcd.setCursor(1,0); lcd.print("..back.."); turnRight(); turnRight(); moveFront(); Align();}
			else closestP();
			break;
		case 'r':
			if(pChar(2,0)){	lcd.setCursor(1,0); lcd.print("..right.."); turnRight(); moveFront(); Align();}
			else if(pChar(0,2)){ lcd.setCursor(1,0); lcd.print("..front.."); moveFront(); Align();}
			else if(pChar(-2,0)){ lcd.setCursor(1,0); lcd.print("..left.."); turnLeft(); moveFront(); Align();}
			else if(pChar(0,-2)){ lcd.setCursor(1,0); lcd.print("..back.."); turnRight(); turnRight(); moveFront(); Align();}
			else closestP();
			break;
		case 'd':
			if(pChar(0,-2)){ lcd.setCursor(1,0); lcd.print("..right.."); turnRight(); moveFront(); Align();}
			else if(pChar(2,0)){ lcd.setCursor(1,0); lcd.print("..front.."); moveFront(); Align();}
			else if(pChar(0,2)){ lcd.setCursor(1,0); lcd.print("..left.."); turnLeft(); moveFront(); Align();}
			else if(pChar(-2,0)){ lcd.setCursor(1,0); lcd.print("..back.."); turnRight(); turnRight(); moveFront(); Align();}
			else closestP();
			break;
		case 'l':
			if(pChar(-2,0)){ lcd.setCursor(1,0); lcd.print("..right.."); turnRight(); moveFront(); Align();}
			else if(pChar(0,-2)){ lcd.setCursor(1,0); lcd.print("..front.."); moveFront(); Align();}
			else if(pChar(2,0)){ lcd.setCursor(1,0); lcd.print("..left.."); turnLeft(); moveFront(); Align();}
			else if(pChar(0,2)){ lcd.setCursor(1,0); lcd.print("..back.."); turnRight(); turnRight(); moveFront(); Align();}
			else closestP();back
			break;
	}
}

void setup() {  // put your setup code here, to run once:
  Serial.begin(9600); 

  attachInterrupt(0, encoder1, RISING);
  attachInterrupt(1, encoder2, RISING);

  //motores
  pinMode(motorRFF, OUTPUT);
  pinMode(motorLFF, OUTPUT);
  pinMode(motorRBF, OUTPUT);
  pinMode(motorLBF, OUTPUT);
  pinMode(motorRFB, OUTPUT);
  pinMode(motorLFB, OUTPUT);
  pinMode(motorRBB, OUTPUT);
  pinMode(motorLBB, OUTPUT);

  //ultrasonicos
  pinMode (trigger1, OUTPUT);
  pinMode (echo1, INPUT);
  pinMode (trigger2, OUTPUT);
  pinMode (echo2, INPUT);
  pinMode (trigger3, OUTPUT);
  pinMode (echo3, INPUT);
  pinMode (trigger4, OUTPUT);
  pinMode (echo4, INPUT);
  pinMode (trigger5, OUTPUT);
  pinMode (echo5, INPUT);
  pinMode(trigger6, OUTPUT);
  pinMode(echo6, INPUT);
  pinMode(trigger7, OUTPUT);
  pinMode(echo7, INPUT);

  //color
  pinMode (s2, OUTPUT);
  pinMode (s3, OUTPUT);
  pinMode (out , INPUT);
  digitalWrite (s2, LOW);
  digitalWrite (s3, LOW);

  //giroscopio
  Wire.begin();
  MPU.selectDevice(DEVICE_TO_USE);
  MPU.init(MPU_UPDATE_RATE, MPU_MAG_MIX_GYRO_ONLY, MAG_UPDATE_RATE, MPU_LPF_RATE);

  //pantalla lcd
  lcd.begin(16,2);// Indicamos medidas de LCD   
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(HIGH);

  crearMat();

  delay (10000);
}

void loop() {
	if(b){
	    analogWrite (motorLFF, 140);
	    analogWrite (motorLBF, 150);
	    analogWrite (motorRFB, 150);
	    analogWrite (motorRBB, 150);
	    delay(100);
	    powerOff();
	    analogWrite (motorLFB, 140);
	    analogWrite (motorLBB, 150);
	    analogWrite (motorRFF, 150);
	    analogWrite (motorRBF, 150);
	    delay(100);
	    powerOff();
	    while(giroscopioZ() < 150);
	    delay(10);
	    b = false;
	    
	    turnRight();
	    if( (ultrasonico(trigger6, echo6) + ultrasonico(trigger7, echo7))/2 < 20)
	    	tab [posX[iPiso]] [posY[iPiso]+1] [iPiso] = 's';
		else
			tab [posX[iPiso]] [posY[iPiso]+1] [iPiso] = 'n';
		turnLeft();
	}
		algo();
}

void encoder1(){ //Contador
  eCount1++;
}

void encoder2(){ //Contador
  eCount2++;
}