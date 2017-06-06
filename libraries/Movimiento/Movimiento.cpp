/*
IDEAS:
-nueva variable para posible victima, cada pared. (O usar datos que no he usado)
	-Sólo si no hay una victima fija ya ahí.
-Se descarta si no hay pared o no hay nada
-Victima heat rampa
*/
#include "Arduino.h"
#include "Movimiento.h"
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#define toleranciaBumper 10
/*
cDir (dirección)
n = norte
e = este
s = sur
w = oeste

sMov
d = derecha
e = enfrente
i = izquierda
a = atrás

*/
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotorRightF = AFMS.getMotor(3);//al revés
Adafruit_DCMotor *myMotorRightB = AFMS.getMotor(1);//al revés
Adafruit_DCMotor *myMotorLeftF = AFMS.getMotor(2);
Adafruit_DCMotor *myMotorLeftB = AFMS.getMotor(4);//al revés
Servo myservo;
int dir = 0;

#define lVictima 52
#define lack 18
#define pin_Servo 9
//1 Enfrente derecha
//2 Atras derecha
//3 Enfrente izquierda
//4 Atras izquierda
Movimiento::Movimiento(){
	AFMS.begin();
    myMotorLeftF->setSpeed(iPowI);
    myMotorLeftF->run(FORWARD);
    myMotorLeftF->run(RELEASE);
    myMotorLeftB->setSpeed(iPowI);
    myMotorLeftB->run(FORWARD);
    myMotorLeftB->run(RELEASE);
    myMotorRightF->setSpeed(iPowD);
    myMotorRightF->run(FORWARD);
    myMotorRightF->run(RELEASE);
    myMotorRightB->setSpeed(iPowD);
    myMotorRightB->run(FORWARD);
    myMotorRightB->run(RELEASE);
	iPowI = iPowD = 150;
    iTamano = 10;
    fRef = eCount1 = limit_Vision = 0;
    kp = 0.2;
    kpA = 4;
    iRampa = 17;
    probVisual = 'n';
    pos = 90;
    myservo.attach(9);
    myservo.write(pos);
    pinMode(lVictima,OUTPUT);
    alinear = false;
    kParedAlinear = 12;
    encoder30 = 1250;
	tVictima = 0;
	cVictima = 0;
}
//Puede que no sea necesaria
Movimiento::Movimiento(uint8_t iPowd, uint8_t iPowi, uint8_t iT, SensarRealidad *r){
	AFMS.begin();
	myMotorLeftF->setSpeed(iPowI);
    myMotorLeftF->run(FORWARD);
    myMotorLeftF->run(RELEASE);
    myMotorLeftB->setSpeed(iPowI);
    myMotorLeftB->run(FORWARD);
    myMotorLeftB->run(RELEASE);
    myMotorRightF->setSpeed(iPowD);
    myMotorRightF->run(FORWARD);
    myMotorRightF->run(RELEASE);
    myMotorRightB->setSpeed(iPowD);
    myMotorRightB->run(FORWARD);
    myMotorRightB->run(RELEASE);
	iPowI = iPowi;
	iPowD = iPowd;
	iTamano = iT;
	fRef = eCount1 = limit_Vision = 0;
	real = r;
	kp = 0.2;
	kpA = 4;
	iRampa = 17;
	probVisual = 'n';
    pos = 90;
    myservo.attach(pin_Servo);
    myservo.write(pos);
    pinMode(lVictima,OUTPUT);
    alinear = false;
    kParedAlinear = 12;
	encoder30 = 1250;
	tVictima = 0;
	cVictima = 0;
}
void Movimiento::Stop(){
   myMotorRightB->run(RELEASE);
   myMotorLeftB->run(RELEASE);
   myMotorRightF->run(RELEASE);
   myMotorLeftF->run(RELEASE);
   eCount1 = 0;
}
void Movimiento::Front(uint8_t PowD, uint8_t PowI){
   myMotorLeftF->setSpeed(PowI);
   myMotorRightF->setSpeed(PowD);
   myMotorLeftB->setSpeed(PowI);
   myMotorRightB->setSpeed(PowD);

   myMotorRightF->run(FORWARD);
   myMotorRightB->run(FORWARD);

   myMotorLeftF->run(FORWARD);
   myMotorLeftB->run(FORWARD);
   //while(digitalRead(lack) == 0){
   //	Stop();
   //}
}
void Movimiento::Back(uint8_t PowD, uint8_t PowI){
   myMotorLeftF->setSpeed(PowI);
   myMotorRightF->setSpeed(PowD);
   myMotorLeftB->setSpeed(PowI);
   myMotorRightB->setSpeed(PowD);

   myMotorRightF->run(BACKWARD);
   myMotorRightB->run(BACKWARD);

   myMotorLeftF->run(BACKWARD);
   myMotorLeftB->run(BACKWARD);
   //while(digitalRead(lack) == 0){
   //	Stop();
   //}
}
void Movimiento::Right(uint8_t PowD, uint8_t PowI){
   myMotorLeftF->setSpeed(PowI);
   myMotorRightF->setSpeed(PowD);
   myMotorLeftB->setSpeed(PowI);
   myMotorRightB->setSpeed(PowD);

   myMotorRightF->run(BACKWARD);
   myMotorRightB->run(BACKWARD);

   myMotorLeftF->run(FORWARD);
   myMotorLeftB->run(FORWARD);
   //while(digitalRead(lack) == 0){
   //	Stop();
   //}
}
void Movimiento::Left(uint8_t PowD, uint8_t PowI){
   myMotorLeftF->setSpeed(PowI);
   myMotorRightF->setSpeed(PowD);
   myMotorLeftB->setSpeed(PowI);
   myMotorRightB->setSpeed(PowD);

   myMotorRightF->run(FORWARD);
   myMotorRightB->run(FORWARD);

   myMotorLeftF->run(BACKWARD);
   myMotorLeftB->run(BACKWARD);
   //while(digitalRead(lack) == 0){
   //	Stop();
   //}
}
void Movimiento::SepararPared(){
	float fDeseado, grados = real->sensarOrientacion();
	unsigned long ahora = millis();
	uint8_t iActual = real->sensarEnfrentePared(), iPowDD;
	real->escribirLCD("SEPARA");
	while (iActual != 6 && ahora+2000 > millis()) {
	  iActual = real->sensarEnfrentePared();
	  if(iActual < 6){//Muy cerca
	    iPowDD = 80 + (6-iActual);
	    Back(iPowDD, iPowDD);
	  }
	  else if(iActual > 6){ //Muy lejos
	    iPowDD = 80 + (iActual-6);
	    Front(iPowDD, iPowDD);
	  }
	}
	if(ahora+2000 <= millis()){
		Back(80, 80);
		delay(500);
	}
	eCount1 = 0;
}
void Movimiento::AlineaPA(char cDir){
	if(alinear){
		Back(80, 80);
		delay(1200);
		Stop();
		fRef = 0;
		for(int i=0; i < 10; i++){
			fRef += real->sensarOrientacion();
		}
		fRef /= 10;
		switch(cDir)
		{
			case 'n':
				fRef = fRef;
				break;
			case 'e':
				fRef = fRef >= 90 ? fRef-90 : fRef + 270;
				break;
			case 's':
				fRef = fRef >= 180 ? fRef-180 : fRef + 180;
				break;
			case 'w':
				fRef = fRef >= 270 ? fRef-270 : fRef + 90;
				break;
		}
		Front(80, 80);
		delay(500);
		eCount1 = 0;
	}
	alinear = false;
}
void Movimiento::ErrorGradosVuelta(float fDeseado, float &grados){
	int iM;
	grados = grados == 360 ? 0 : grados;
	grados = grados - fDeseado;
	if (grados > 180){
		iM = grados;
		iM /= 180;
		grados = -( 180 - (grados - 180 * iM) );
	}
	else if (grados < -180){
		iM = -grados;
		iM /= 180;
		grados = ( 180 - (grados - 180 * iM) );
	}
}

void Movimiento::VueltaGyro(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, float fDeseado, bool kit){
	int iPowDD;
	float grados = 10, kitActual;
	uint8_t iCase;
	unsigned long ahora = millis();
	while (grados < -6 || grados > 6) {
		grados = real->sensarOrientacion();
	  	ErrorGradosVuelta(fDeseado, grados);
	  	while(Serial2.available())
	    	cVictima = (char)Serial2.read();
		if(kit && cVictima&0b00000010 && !tMapa[iPiso][iRow][iCol].victima()){
			iCase = (cVictima&0b00000001) ? 1 : 2;
			real->escribirLCD("Victima");
			Stop();
		   	dejarKit(tMapa, cDir, iCol, iRow, iPiso, iCase, fDeseado);
		}
	  	if (grados < 0) {
	       	iPowDD = 120 + (grados * (-1) * kp);
		   	iPowDD = iPowDD > 180 ? 180 : iPowDD;
		   	Right(iPowDD, iPowDD);
		}
		else {
		   	iPowDD = 120 + (grados * kp);
		   	iPowDD = iPowDD > 180 ? 180 : iPowDD;
		   	Left(iPowDD, iPowDD);
		}
		if(millis()-ahora > 4000){
			Front(150, 150);
		  	delay(200);
		  	ahora = millis();
		}
	}
	eCount1 = 0;
}
void Movimiento::potenciasDerecho(float fDeseado, float &grados, int &iPowDD, int &iPowII){
	int pDeseado = 5, pDeseadoI = 5, pow_DD_Now, pow_II_Now, iError, pNow = real->sensarDerechaPared(), pNowI = real->sensarIzquierdaPared();
	if(pNow < 15){
		iError = pDeseado-pNow;
		pow_DD_Now = iPowD + iError * kParedAlinear;
		pow_II_Now = iPowI - iError * kParedAlinear;
	}
	else if(pNowI < 15){
		iError = pDeseadoI-pNowI;
		pow_DD_Now = iPowD - iError * kParedAlinear;
		pow_II_Now = iPowI + iError * kParedAlinear;
	}
	else{
		pow_DD_Now = pow_II_Now = 200;
	}
	grados = real->sensarOrientacion();
	ErrorGradosVuelta(fDeseado, grados);
	iPowDD = iPowD + (grados * kpA) + pow_DD_Now;
	iPowII = iPowI - (grados * kpA) + pow_II_Now;
    iPowDD/=2;
    iPowII/=2;
    iPowII = iPowII > 200 ? 200 : (iPowII < 0 ? 0 : iPowII);
    iPowDD = iPowDD > 200 ? 200 : (iPowDD < 0 ? 0 : iPowDD);
}
void Movimiento::pasaRampa(char cDir){
	while(Serial2.available())
		cVictima = (char)Serial2.read();
	real->escribirLCD("Rampa");
	unsigned long ahora = millis(), despues = millis();
	int iPowII, iPowDD, iCase;
	float fDeseado, grados;
	bool vR = true;
	switch(cDir)
	{
		case 'n':
			fDeseado = fRef;
			break;
		case 'e':
			fDeseado = fRef < 270 ? fRef+90 : fRef-270;
			break;
		case 's':
			fDeseado = fRef < 180 ? fRef+180 : fRef-180;
			break;
		case 'w':
			fDeseado = fRef < 90 ? fRef+270 : fRef-90;
			break;
	}
	while(real->sensarRampa() < -iRampa || real->sensarRampa() > iRampa){
		grados = real->sensarOrientacion();
		potenciasDerecho(fDeseado, grados, iPowDD, iPowII);
		Front(iPowDD, iPowII);
	}
	Front(100, 100);
	delay(800);
	Stop();
	if(real->sensarEnfrentePared() < iRampa){
   		SepararPared();
   	}
}
bool Movimiento::victimaPosible(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, uint8_t &iCase, bool bVuelta, bool bCalor, float fGrados){
	iCase = (cVictima&0b00000001) ? 1 : 2;
	cVictima = 0;
	if( (tVictima+1000) > millis() ) return false;
	real->escribirLCD("Posible?", "O no posible?");
	bool normal = true;
	if(!tMapa[iPiso][iRow][iCol].victima()){//Si no hay ninguna, directamente manda true
		return true;
	}
	if(iCase == 0){
		switch(cDir){
			case 'n':
				return !tMapa[iPiso][iRow][iCol].victimaArriba();
				break;
			case 'e':
				return !tMapa[iPiso][iRow][iCol].victimaDerecha();
				break;
			case 's':
				return !tMapa[iPiso][iRow][iCol].victimaAbajo();
				break;
			case 'w':
				return !tMapa[iPiso][iRow][iCol].victimaIzquierda();
				break;
		}
	}
	if(bVuelta){//Si está dando vuelta
		if(fGrados > 50 || fGrados < -50)//Sigue viendo la pared a que si estuviera normal
			normal = true;
		else if(fGrados < 0){//A la derecha ahora está la pared que estaba enfrente
			normal = false;
			switch(cDir){
				case 'n':
					return ( (iCase == 1 && !tMapa[iPiso][iRow][iCol].victimaArriba() ) || (iCase == 2 && !tMapa[iPiso][iRow][iCol].victimaAbajo() ) );
					break;
				case 'e':
					return ( (iCase == 1 && !tMapa[iPiso][iRow][iCol].victimaDerecha() ) || (iCase == 2 && !tMapa[iPiso][iRow][iCol].victimaIzquierda() ) );
					break;
				case 's':
					return ( (iCase == 1 && !tMapa[iPiso][iRow][iCol].victimaAbajo() ) || (iCase == 2 && !tMapa[iPiso][iRow][iCol].victimaArriba() ) );
					break;
				case 'w':
					return ( (iCase == 1 && !tMapa[iPiso][iRow][iCol].victimaIzquierda() ) || (iCase == 2 && !tMapa[iPiso][iRow][iCol].victimaDerecha() ) );
					break;
			}
		}
		else{//A la derecha ahora está la pared que estaba atras
			normal = false;
			switch(cDir){
				case 'n':
					return ( (iCase == 2 && !tMapa[iPiso][iRow][iCol].victimaArriba() ) || (iCase == 1 && !tMapa[iPiso][iRow][iCol].victimaAbajo() ) );
					break;
				case 'e':
					return ( (iCase == 2 && !tMapa[iPiso][iRow][iCol].victimaDerecha() ) || (iCase == 1 && !tMapa[iPiso][iRow][iCol].victimaIzquierda() ) );
					break;
				case 's':
					return ( (iCase == 2 && !tMapa[iPiso][iRow][iCol].victimaAbajo() ) || (iCase == 1 && !tMapa[iPiso][iRow][iCol].victimaArriba() ) );
					break;
				case 'w':
					return ( (iCase == 2 && !tMapa[iPiso][iRow][iCol].victimaIzquierda() ) || (iCase == 1 && !tMapa[iPiso][iRow][iCol].victimaDerecha() ) );
					break;
			}
		}
	}
	if(normal){//Si está andando normal
		switch(cDir){
			case 'n':
				return ( (iCase == 1 && !tMapa[iPiso][iRow][iCol].victimaDerecha() ) || (iCase == 2 && !tMapa[iPiso][iRow][iCol].victimaIzquierda() ) );
				break;
			case 'e':
				return ( (iCase == 1 && !tMapa[iPiso][iRow][iCol].victimaAbajo() ) || (iCase == 2 && !tMapa[iPiso][iRow][iCol].victimaArriba() ) );
				break;
			case 's':
				return ( (iCase == 1 && !tMapa[iPiso][iRow][iCol].victimaIzquierda() ) || (iCase == 2 && !tMapa[iPiso][iRow][iCol].victimaDerecha() ) );
				break;
			case 'w':
				return ( (iCase == 1 && !tMapa[iPiso][iRow][iCol].victimaArriba() ) || (iCase == 2 && !tMapa[iPiso][iRow][iCol].victimaAbajo() ) );
				break;
		}
	}
	tVictima = millis();
	return false;
}

void Movimiento::mapearVictima(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, uint8_t iCase, bool bVuelta, float fGrados){
	bool normal = true;
	tMapa[iPiso][iRow][iCol].victima(true);
	if(iCase == 0){
		normal = false;
		switch(cDir){
			case 'n':
				tMapa[iPiso][iRow][iCol].victimaArriba(true);
				break;
			case 'e':
				tMapa[iPiso][iRow][iCol].victimaDerecha(true);
				break;
			case 's':
				tMapa[iPiso][iRow][iCol].victimaAbajo(true);
				break;
			case 'w':
				tMapa[iPiso][iRow][iCol].victimaIzquierda(true);
				break;
		}
	}
	else if(bVuelta){//Si está dando vuelta
		if(fGrados > 50 || fGrados < -50)//Sigue viendo la pared a que si estuviera normal
			normal = true;
		else if(fGrados < 0){//A la derecha ahora está la pared que estaba enfrente
			normal = false;
			switch(cDir){
				case 'n':
					switch(iCase){
						case 1:
							//real->escribirLCD(" ", "Arriba");
							tMapa[iPiso][iRow][iCol].victimaArriba(true);
							break;
						case 2:
							//real->escribirLCD(" ", "ABAJO");
							tMapa[iPiso][iRow][iCol].victimaAbajo(true);
							break;
					}
					break;
				case 'e':
					switch(iCase){
						case 1:
							//real->escribirLCD("DERECHA");
							tMapa[iPiso][iRow][iCol].victimaDerecha(true);
							break;
						case 2:
							//real->escribirLCD("IZQUIERDA");
							tMapa[iPiso][iRow][iCol].victimaIzquierda(true);
							break;
					}
					break;
				case 's':
					switch(iCase){
						case 1:
							//real->escribirLCD(" ", "ABAJO");
							tMapa[iPiso][iRow][iCol].victimaAbajo(true);
							break;
						case 2:
							//real->escribirLCD(" ", "Arriba");
							tMapa[iPiso][iRow][iCol].victimaArriba(true);
							break;
					}
					break;
				case 'w':
					switch(iCase){
						case 1:
							//real->escribirLCD("IZQUIERDA");
							tMapa[iPiso][iRow][iCol].victimaIzquierda(true);
							break;
						case 2:
							//real->escribirLCD("DERECHA");
							tMapa[iPiso][iRow][iCol].victimaDerecha(true);
							break;
					}
					break;
			}
		}
		else{//A la derecha ahora está la pared que estaba atras
			normal = false;
			switch(cDir){
				case 'n':
					switch(iCase){
						case 2:
							//real->escribirLCD(" ", "Arriba");
							tMapa[iPiso][iRow][iCol].victimaArriba(true);
							break;
						case 1:
							//real->escribirLCD(" ", "ABAJO");
							tMapa[iPiso][iRow][iCol].victimaAbajo(true);
							break;
					}
					break;
				case 'e':
					switch(iCase){
						case 2:
							//real->escribirLCD("DERECHA");
							tMapa[iPiso][iRow][iCol].victimaDerecha(true);
							break;
						case 1:
							//real->escribirLCD("IZQUIERDA");
							tMapa[iPiso][iRow][iCol].victimaIzquierda(true);
							break;
					}
					break;
				case 's':
					switch(iCase){
						case 2:
							//real->escribirLCD(" ", "ABAJO");
							tMapa[iPiso][iRow][iCol].victimaAbajo(true);
							break;
						case 1:
							//real->escribirLCD(" ", "Arriba");
							tMapa[iPiso][iRow][iCol].victimaArriba(true);
							break;
					}
					break;
				case 'w':
					switch(iCase){
						case 2:
							//real->escribirLCD("IZQUIERDA");
							tMapa[iPiso][iRow][iCol].victimaIzquierda(true);
							break;
						case 1:
							//real->escribirLCD("DERECHA");
							tMapa[iPiso][iRow][iCol].victimaDerecha(true);
							break;
					}
					break;
			}
		}
	}
	if(normal){//Si está andando normal
		switch(cDir){
				case 'n':
					switch(iCase){
						case 1:
							//real->escribirLCD("DERECHA");
							tMapa[iPiso][iRow][iCol].victimaDerecha(true);
							break;
						case 2:
							//real->escribirLCD("IZQUIERDA");
							tMapa[iPiso][iRow][iCol].victimaIzquierda(true);
							break;
					}
					break;
				case 'e':
					switch(iCase){
						case 1:
							//real->escribirLCD(" ", "ABAJO");
							tMapa[iPiso][iRow][iCol].victimaAbajo(true);
							break;
						case 2:
							//real->escribirLCD(" ", "Arriba");
							tMapa[iPiso][iRow][iCol].victimaArriba(true);
							break;
					}
					break;
				case 's':
					switch(iCase){
						case 1:
							//real->escribirLCD("IZQUIERDA");
							tMapa[iPiso][iRow][iCol].victimaIzquierda(true);
							break;
						case 2:
							//real->escribirLCD("DERECHA");
							tMapa[iPiso][iRow][iCol].victimaDerecha(true);
							break;
					}
					break;
				case 'w':
					switch(iCase){
						case 1:
							//real->escribirLCD(" ", "Arriba");
							tMapa[iPiso][iRow][iCol].victimaArriba(true);
							break;
						case 2:
							//real->escribirLCD(" ", "ABAJO");
							tMapa[iPiso][iRow][iCol].victimaAbajo(true);
							break;
					}
					break;
		}
	}
}

void Movimiento::dejarKit(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, uint8_t iCase, float fDeseado){
	tMapa[iPiso][iR][iC].victima(true);
	real->escribirLCD("DEJAR KIT");
	digitalWrite(lVictima, HIGH);
	int fDeseadoT;
	if(real->sensarRampa() < iRampa && real->sensarRampa() > -iRampa){
		switch(iCase){
			case 2: //izq
				fDeseadoT = fDeseado < 90 ? fDeseado + 270 : fDeseado - 90;
				break;
			case 1: //der
				fDeseadoT = fDeseado > 270 ? fDeseado - 270 : fDeseado + 90;
				break;
		}
		VueltaGyro(tMapa, cDir, iCol, iRow, iPiso, fDeseadoT, false);
	}
	Stop();
    for (pos = 90; pos <= 165; pos += 1) { // goes from 180 degrees to 0 degrees
    	myservo.write(pos);                // tell servo to go to position in variable 'pos'
    	delay(15);                         // waits 15ms for the servo to reach the position
  	}
  	for (pos = 165; pos >= 90; pos -= 1) { // goes from 0 degrees to 180 degrees
    	myservo.write(pos);                // tell servo to go to position in variable 'pos'
    	delay(15);                         // waits 15ms for the servo to reach the position
  	}
    VueltaGyro(tMapa, cDir, iCol, iRow, iPiso, fDeseado, false);
    Stop();
    digitalWrite(lVictima, LOW);
    while(Serial2.available())
    	cVictima = (char)Serial2.read();
    tVictima = millis();
}

void Movimiento::identificaVictima(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, uint8_t iCase, float fDeseado){
	real->escribirLCD("IDENTIFICA");
	float fDeseadoT;
	switch(iCase){
		case 2:
			fDeseadoT = fDeseado < 90 ? fDeseado + 270 : fDeseado - 90;
			break;
		case 1:
			fDeseadoT = fDeseado > 270 ? fDeseado - 270 : fDeseado + 90;
			break;
		case 0:
			fDeseadoT = fDeseado;
	}
	VueltaGyro(tMapa, cDir, iCol, iRow, iPiso, fDeseadoT, false);
	Stop();
	VueltaGyro(tMapa, cDir, iCol, iRow, iPiso, fDeseadoT, false);
	Stop();
	unsigned long ahora = millis(), despues = millis();
	uint8_t iActual = real->sensarEnfrentePared();
	uint8_t iPowDD;
	while (iActual != 11 && ahora+1500 > despues) {
	  iActual = real->sensarEnfrentePared();
	  if(iActual < 11){//Muy cerca
	    iPowDD = 80 + (11-iActual);
	    Back(iPowDD, iPowDD);
	  }
	  else if(iActual > 11){ //Muy lejos
	    iPowDD = 80 + (iActual-11);
	    Front(iPowDD, iPowDD);
	  }
	  despues = millis();
	}
	if(ahora+1500 < despues){
		Back(80, 80);
		delay(500);
	}
	Stop();
	char letra = 'p';
	real->escribirLCD("CUAL?");
	Serial2.println("Identifica");
	ahora = millis(), despues = millis();
	while(letra != 'H' && letra != 'S' && letra != 'U' && letra != 'L' && letra != 'N' && (ahora+1800) > despues){
		while(!Serial2.available() && (ahora+1800) > despues){
			delay(1);
			despues = millis();
		}
		if((ahora+1800) > despues){
			letra = (char)Serial2.read();
			if(letra != 'H' && letra != 'S' && letra != 'U' && letra != 'N' && letra != 'L'){
				Serial2.println("Identifica");
			}
		}
		despues = millis();
	}
	real->escribirLCD("YA ESTA");
	int i = 2;
	SepararPared();
	Stop();
	Serial2.println("Avanza");
	switch(letra){
		//2 kits
		case 'H':
			probVisual = probVisual2 = 'n';
			//real->escribirLCD("HHHHHHHHHHHHHHHH");
			real->escribirLCD("Visual");
			digitalWrite(lVictima, HIGH);
			mapearVictima(tMapa, cDir, iCol, iRow, iPiso, iCase, false, 0);
			i = 0;
			break;
		//1 kit
		case 'S':
			i = 1;
			probVisual = probVisual2 = 'n';
			//real->escribirLCD("SSSSSSSSSSSSSSSS");
			real->escribirLCD("Visual");
			digitalWrite(lVictima, HIGH);
			mapearVictima(tMapa, cDir, iCol, iRow, iPiso, iCase, false, 0);
			break;
		//0 kits
		case 'U':
			probVisual = probVisual2 = 'n';
			//real->escribirLCD("UUUUUUUUUUUUUUUU");
			real->escribirLCD("Visual");
			digitalWrite(lVictima, HIGH);
			mapearVictima(tMapa, cDir, iCol, iRow, iPiso, iCase, false, 0);
			i = 1;
			break;
		case 'N':
			//real->escribirLCD("NNNNNNNNNNNNNNNN");
			real->escribirLCD("Falso");
			break;
		case 'L':
			//real->escribirLCD("LLLLLLLLLLLLLLLL");
			real->escribirLCD("Visual");
			digitalWrite(lVictima, HIGH);
			mapearVictima(tMapa, cDir, iCol, iRow, iPiso, iCase, false, 0);
			i = 1;
			break;

	}
	for(i; i < 2; i++){
		for (pos = 90; pos <= 165; pos += 1) {
	    	myservo.write(pos);
	    	delay(15);
	  	}
	  	for (pos = 165; pos >= 90; pos -= 1) {
	    	myservo.write(pos);
	    	delay(15);
	  	}
	}
	
	digitalWrite(lVictima, LOW);
	VueltaGyro(tMapa, cDir, iCol, iRow, iPiso, fDeseado, false);
}

void Movimiento::retroceder(Tile tMapa[3][10][10],  char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso){
	real->escribirLCD("NEGRO");
	float fDeseado, grados;
	int iPowDD, iPowII;
	switch(cDir)
	{
		case 'n':
			fDeseado = fRef;
			iRow++;
			break;
		case 'e':
			fDeseado = fRef < 270 ? fRef+90 : fRef-270;
			iCol--;
			break;
		case 's':
			fDeseado = fRef < 180 ? fRef+180 : fRef-180;
			iRow--;
			break;
		case 'w':
			fDeseado = fRef < 90 ? fRef+270 : fRef-90;
			iCol++;
			break;
	}
	while(eCount1 < encoder30){
		grados = real->sensarOrientacion();
		potenciasDerecho(fDeseado, grados, iPowDD, iPowII);
	    Back(iPowII, iPowDD);
	}
	eCount1 = 0;
	probVisual = 'n';
}

void Movimiento::acomodaChoque(uint8_t switchCase){
	real->escribirLCD("AUCH");
	int encoderTemp = eCount1;
	Stop();
	switch(switchCase){
		case 1:
			Back(0, 255);
			delay(400);
			Back(255, 0);
			delay(400);
			break;
		case 2:
			Back(255, 0);
			delay(400);
			Back(0, 255);
			delay(400);
			break;
	}
	encoderTemp -= eCount1;
	eCount1 = encoderTemp;
}

void Movimiento::avanzar(Tile tMapa[3][10][10], char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso){						//Modificarse en realidad
	float fDeseado, grados = real->sensarOrientacion(), pasado, bumperMax = 0.0, bumperMin = 0.0;
	int iPowDD, iPowII;
	uint8_t switchCase, iCase;
	real->escribirLCD("FRONT");
	while(Serial2.available())
    	cVictima = (char)Serial2.read();
	AlineaPA(cDir);
	switch(cDir){
		case 'n':
			fDeseado = fRef;
			break;
		case 'e':
			fDeseado = fRef < 270 ? fRef+90 : fRef-270;
			break;
		case 's':
			fDeseado = fRef < 180 ? fRef+180 : fRef-180;
			break;
		case 'w':
			fDeseado = fRef < 90 ? fRef+270 : fRef-90;
			break;
	}
	while(eCount1 < encoder30 && real->sensarEnfrentePared() > 7){
		potenciasDerecho(fDeseado, grados, iPowDD, iPowII);
	    Front(iPowDD, iPowII);
	    //Calor
	    while(Serial2.available())
	    	cVictima = (char)Serial2.read();
	    if(cVictima&0b00000010){
	    	int countT = eCount1;
	    	uint8_t iC = iCol, iR = iRow;
	    	if(countT > (encoder30/2)){
	    		switch(cDir){
					case 'n':
						iR--;
						break;
					case 'e':
						iC++;
						break;
					case 's':
						iR++;
						break;
					case 'w':
						iC--;
						break;
				}
	    	}
	    	if(!tMapa[iPiso][iR][iC].victima()){
	    		iCase = (cVictima&0b00000001) ? 1 : 2;
				real->escribirLCD("Victima");
	    		Stop();
		    	dejarKit(tMapa, cDir, iC, iR, iPiso, iCase, fDeseado);
	    	}
	    	eCount1 = countT;
	    }
		switchCase = real->switches();
	    if(switchCase > 0 && real->sensarEnfrente()){
	    	acomodaChoque(switchCase);
	    }
		//bumper
		bumperMin = real->sensarRampaFloat() < bumperMin ? real->sensarRampaFloat() : bumperMin;
		bumperMax = real->sensarRampaFloat() > bumperMax ? real->sensarRampaFloat() : bumperMax;
		if (bumperMax - bumperMin >= toleranciaBumper)
			tMapa[iPiso][iRow][iCol].bumper(true);
		cVictima = 0;
    }
	real->escribirLCD(String(bumperMin)+" "+String(bumperMax)+" "+String(bumperMax - bumperMin));
    VueltaGyro(tMapa, cDir, iCol, iRow, iPiso, fDeseado, true);
   	eCount1 = 0;
   	switch(cDir){
		case 'n':
			iRow--;
			break;
		case 'e':
			iCol++;
			break;
		case 's':
			iRow++;
			break;
		case 'w':
			iCol--;
			break;
	}
   	if( !real->color() && real->sensarRampa() < iRampa && real->sensarRampa() > -iRampa && real->sensarEnfrentePared() < 20){
   		real->escribirLCD("Separar");
   		Back(80, 80);
   		delay(100);
   		SepararPared();
   		iCase = 0;
   		real->escribirLCD("Acabe");
   	}
}
//Aquí obviamente hay que poner las cosas de moverse con los motores, hasta ahorita es solamente modificar mapa
void Movimiento::derecha(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso){														//Modificarse en realidad
	real->escribirLCD("DER");
	alinear = !(real->sensarIzquierda());
	float fDeseado, fActual = real->sensarOrientacion();
	uint8_t iCase;
	while(Serial2.available())
	    cVictima = (char)Serial2.read();
	if(cVictima&0b00000010 && !tMapa[iPiso][iRow][iCol].victima()){
		iCase = (cVictima&0b00000001) ? 1 : 2;
		real->escribirLCD("Victima");
		Stop();
	   	dejarKit(tMapa, cDir, iCol, iRow, iPiso, iCase, fDeseado);
	}
    switch(cDir){
		case 'n':
			fDeseado = fRef < 270 ? fRef+90 : fRef-270;
			cDir='e';
			break;
		case 'e':
			fDeseado = fRef < 180 ? fRef+180 : fRef-180;
			cDir = 's';
			break;
		case 's':
			fDeseado = fRef < 90 ? fRef+270 : fRef-90;
			cDir = 'w';
			break;
		case 'w':
			fDeseado = fRef;
			cDir = 'n';
			break;
	}
    VueltaGyro(tMapa, cDir, iCol, iRow, iPiso, fDeseado, true);
    VueltaGyro(tMapa, cDir, iCol, iRow, iPiso, fDeseado, true);
}
//Aquí obviamente hay que poner las cosas de moverse con los motores, hasta ahorita es solamente modificar mapa
void Movimiento::izquierda(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso){
	probVisual = probVisual2 = 'n';
	real->escribirLCD("IZQ");
	alinear = !(real->sensarDerecha());
    float fDeseado, fActual = real->sensarOrientacion();
    uint8_t iCase;
    while(Serial2.available())
	    cVictima = (char)Serial2.read();
	if(cVictima&0b00000010 && !tMapa[iPiso][iRow][iCol].victima()){
		iCase = (cVictima&0b00000001) ? 1 : 2;
		real->escribirLCD("Victima");
		Stop();
	   	dejarKit(tMapa, cDir, iCol, iRow, iPiso, iCase, fDeseado);
	}
    switch(cDir){
		case 'n':
			fDeseado = fRef < 90 ? fRef+270 : fRef-90;
			cDir = 'w';
			break;
		case 'e':
			fDeseado = fRef;
			cDir = 'n';
			break;
		case 's':
			fDeseado = fRef < 270 ? fRef+90 : fRef-270;
			cDir = 'e';
			break;
		case 'w':
			fDeseado = fRef < 180 ? fRef+180 : fRef-180;
			cDir = 's';
			break;
	}
    VueltaGyro(tMapa, cDir, iCol, iRow, iPiso, fDeseado, true);
    VueltaGyro(tMapa, cDir, iCol, iRow, iPiso, fDeseado, true);
}
//Recibe el string de a dónde moverse y ejecuta las acciones llamando a las funciones de arriba
void Movimiento::hacerInstrucciones(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, String sMov){
	real->escribirLCD("PATH");
	///////////////////////////TEST/////////////////////////////////
	/*for(int i = sMov.length()-1; i >= 0; i--){
		Serial.print(sMov[i]);
	}
	Serial.println(" ");
	delay(5000);
	cDir = (char)Serial.read();*/
	for(int i = sMov.length()-1; i >= 0; i--){
		switch(sMov[i]){
			case 'r':
				derecha(tMapa, cDir, iCol, iRow, iPiso);
				avanzar(tMapa, cDir, iCol, iRow, iPiso);
				break;
			case 'u':
				avanzar(tMapa, cDir, iCol, iRow, iPiso);
				break;
			case 'l':
				izquierda(tMapa, cDir, iCol, iRow, iPiso);
				avanzar(tMapa, cDir, iCol, iRow, iPiso);
				break;
			case 'd':
				derecha(tMapa, cDir, iCol, iRow, iPiso);
				derecha(tMapa, cDir, iCol, iRow, iPiso);
				avanzar(tMapa, cDir, iCol, iRow, iPiso);
				break;	
		}
	}
	Stop();
}

//Busca dónde está el no visitado usando funciones de aquí y de sensar mapa.
//Aquí imprime el mapa de int
//La funcion comparaMapa se podría modificar para depender si quiero ir al inicio o a un cuadro no visitado
bool Movimiento::goToVisitado(Tile tMapa[3][10][10], char &cDir, char cD, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso){
	//Declara un mapa de int, debe ser del tamaño que el otro mapa. Será mejor declararlo desde un principio del código?
	uint8_t iMapa[10][10];
	char cMapa[10][10];
	//Llena el mapa de 0
	for (uint8_t i = 0; i < iTamano; i++)
		for(uint8_t j = 0; j < iTamano; j++){
			iMapa[i][j] = 0;
			cMapa[i][j] = 'n';
		}
	//Pone 1 en donde vamos a iniciar
	iMapa[iRow][iCol] = 1;
	cMapa[iRow][iCol] = 'i';
	//LA FUNCION RECURSIVA
	mapa.llenaMapa(iMapa, cMapa, tMapa, cDir, iCol, iRow, iPiso);
	///////////////Imprime el mapa//////////////////////////////
	/*for (uint8_t i = 0; i < iTamano; ++i){
		for(uint8_t j=0; j<iTamano; j++){
			Serial.print(iMapa[i][j]); Serial.print(" ");
		}
		Serial.println();
	}
	Serial.println();
	Serial.println();
	for (uint8_t i = 0; i < iTamano; ++i){
		for(uint8_t j=0; j<iTamano; j++){
			Serial.print(cMapa[i][j]); Serial.print(" ");
		}
		Serial.println();
	}
	delay(5000);*/
	//Nuevas coordenadas a dónde moverse
	uint8_t iNCol = 100, iNRow = 100;
	//Compara las distancias para escoger la más pequeña
	if(mapa.comparaMapa(iMapa, tMapa, cD, iCol, iRow, iNCol, iNRow, iPiso)){//Hace las instrucciones que recibe de la función en forma de string
		hacerInstrucciones(tMapa, cDir, iCol, iRow, iPiso, mapa.getInstrucciones(iMapa, cMapa, tMapa, iNCol, iNRow, iPiso));
		//hacerInstrucciones(cMapa, tMapa, cDir, iCol, iRow, iPiso);
		iCol = iNCol, iRow = iNRow;
		return true;
	}
	return false;
}

//La hice bool para que de una forma estuviera como condición de un loop, pero aún no se me ocurre cómo
bool Movimiento::decidir(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso){
	if(tMapa[iPiso][iRow][iCol].cuadroNegro()){
		retroceder(tMapa, cDir, iCol, iRow, iPiso);
	}
	if(real->sensarEnfrentePared() < 20){
   		SepararPared();
   	}
   	float fDeseado;
	switch(cDir)
	{
		case 'n':
			fDeseado = fRef;
			break;
		case 'e':
			fDeseado = fRef < 270 ? fRef+90 : fRef-270;
			break;
		case 's':
			fDeseado = fRef < 180 ? fRef+180 : fRef-180;
			break;
		case 'w':
			fDeseado = fRef < 90 ? fRef+270 : fRef-90;
			break;
	}
	uint8_t iCase;
	while(Serial2.available())
	    cVictima = (char)Serial2.read();
	if(cVictima&0b00000010 && !tMapa[iPiso][iRow][iCol].victima()){
		iCase = (cVictima&0b00000001) ? 1 : 2;
		real->escribirLCD("Victima");
		Stop();
	   	dejarKit(tMapa, cDir, iCol, iRow, iPiso, iCase, fDeseado);
	}
	//Esto ya no debe de ser necesario con la clase Mapear y SensarRealidad
	tMapa[iPiso][iRow][iCol].existe(true);
	//Esto, no sé si sea mejor tenerlo aquí o en la clase Mapear
	tMapa[iPiso][iRow][iCol].visitado(true);
	//Todos estos sensados los hace con el mapa virtual, por eso dependemos en que el robot sea preciso.
	//Si no hay pared a la derecha Y no está visitado, muevete hacia allá
	if(mapa.sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'r') && mapa.sensaVisitado(tMapa, cDir, iCol, iRow, iPiso, 'r')){
		derecha(tMapa, cDir, iCol, iRow, iPiso);
		avanzar(tMapa, cDir, iCol, iRow, iPiso);
		Stop();
		return true;
	}
	//Si no hay pared enfrente Y no está visitado, muevete hacia allá
	else if(mapa.sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'u') && mapa.sensaVisitado(tMapa, cDir, iCol, iRow, iPiso, 'u')){
		avanzar(tMapa, cDir, iCol, iRow, iPiso);
		Stop();
		return true;
	}
	//Si no hay pared a la izquierda y no está visitado, muevete hacia allá
	else if(mapa.sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'l') && mapa.sensaVisitado(tMapa, cDir, iCol, iRow, iPiso, 'l')){
		izquierda(tMapa, cDir, iCol, iRow, iPiso);
		avanzar(tMapa, cDir, iCol, iRow, iPiso);
		Stop();
		return true;
	}
	//Si no hay pared atrás y no está visitado, muevete hacia allá
	//Sólo entraría a este caso en el primer movimiento de la ronda (si queda mirando a un deadend)
	else if(mapa.sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'd') && mapa.sensaVisitado(tMapa, cDir, iCol, iRow, iPiso, 'd')){
		derecha(tMapa, cDir, iCol, iRow, iPiso);
		derecha(tMapa, cDir, iCol, iRow, iPiso);
		avanzar(tMapa, cDir, iCol, iRow, iPiso);
		Stop();
		return true;
	}
	//Aquí es cuando entra a lo recursivo
	else{
		//Llama la función recursiva
		return goToVisitado(tMapa, cDir, 'n', iCol, iRow, iPiso);
	}
}

bool Movimiento::decidir_Prueba(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso){
	//Esto ya no debe de ser necesario con la clase Mapear y SensarRealidad
	tMapa[iPiso][iRow][iCol].existe(true);
	//Esto, no sé si sea mejor tenerlo aquí o en la clase Mapear
	tMapa[iPiso][iRow][iCol].visitado(true);
	//Todos estos sensados los hace con el mapa virtual, por eso dependemos en que el robot sea preciso.
	//Si no hay pared a la derecha Y no está visitado, muevete hacia allá
	if(mapa.sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'r') && mapa.sensaVisitado(tMapa, cDir, iCol, iRow, iPiso, 'r')){
		Serial.println("DER");
		switch(cDir){
			case 'n':
				iCol++;
				cDir = 'e';
				break;
			case 'e':
				iRow++;
				cDir = 's';
				break;
			case 's':
				iCol--;
				cDir = 'w';
				break;
			case 'w':
				iRow--;
				cDir = 'n';
				break;
		}
		return true;
	}
	//Si no hay pared enfrente Y no está visitado, muevete hacia allá
	else if(mapa.sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'u') && mapa.sensaVisitado(tMapa, cDir, iCol, iRow, iPiso, 'u')){
		Serial.println("ENF");
		switch(cDir){
			case 'n':
				iRow--;
				cDir = 'n';
				break;
			case 'e':
				iCol++;
				cDir = 'e';
				break;
			case 's':
				iRow++;
				cDir = 's';
				break;
			case 'w':
				iCol--;
				cDir = 'w';
				break;
		}
		return true;
	}
	//Si no hay pared a la izquierda y no está visitado, muevete hacia allá
	else if(mapa.sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'l') && mapa.sensaVisitado(tMapa, cDir, iCol, iRow, iPiso, 'l')){
		Serial.println("IZQ");
		switch(cDir){
			case 'n':
				iCol--;
				cDir = 'w';
				break;
			case 'e':
				iRow--;
				cDir = 'n';
				break;
			case 's':
				iCol++;
				cDir = 'e';
				break;
			case 'w':
				iRow++;
				cDir = 's';
				break;
		}
		return true;
	}
	//Si no hay pared atrás y no está visitado, muevete hacia allá
	//Sólo entraría a este caso en el primer movimiento de la ronda (si queda mirando a un deadend)
	else if(mapa.sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'd') && mapa.sensaVisitado(tMapa, cDir, iCol, iRow, iPiso, 'd')){
		Serial.println("ATR");
		switch(cDir){
			case 'n':
				iRow++;
				cDir = 's';
				break;
			case 'e':
				iCol--;
				cDir = 'w';
				break;
			case 's':
				iRow--;
				cDir = 'n';
				break;
			case 'w':
				iCol++;
				cDir = 'e';
				break;
		}
		return true;
	}
	//Aquí es cuando entra a lo recursivo
	else{
		//Llama la función recursiva
		return goToVisitado(tMapa, cDir, 'n', iCol, iRow, iPiso);
	}
}

void Movimiento::encoder(){
    eCount1++;
}
//FUNCION PARA QUE EL ROBOT NO SE DETENGA