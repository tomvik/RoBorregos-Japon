#include "Arduino.h"
#include "Movimiento.h"
#include <string.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <PID_v1.h>
#define toleranciaBumper 10
const float kPRECISION_IMU = 5;

double inIzq, outIzq, inDer, outDer, fSet;

PID izqPID(&inIzq, &outIzq, &fSet, 15, 0, 0, DIRECT);
PID derPID(&inDer, &outDer, &fSet, 15, 0, 0, REVERSE);
/*
   cDir (dirección)
   n = norteder
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
Adafruit_DCMotor *myMotorRightF = AFMS.getMotor(4);
Adafruit_DCMotor *myMotorRightB = AFMS.getMotor(1);
Adafruit_DCMotor *myMotorLeftF = AFMS.getMotor(2);
Adafruit_DCMotor *myMotorLeftB = AFMS.getMotor(3);//al revés
Servo myservo;


#define lVictima 52
#define lack 18
#define pin_Servo 9
//1 Enfrente derecha
//2 Atras derecha
//3 Enfrente izquierda
//4 Atras izquierda
Movimiento::Movimiento(uint8_t iPowd, uint8_t iPowi, uint8_t iT, SensarRealidad *r){
  AFMS.begin();
  myMotorLeftF->setSpeed(iPowI);
  myMotorLeftF->run(BACKWARD);
  myMotorLeftF->run(RELEASE);
  myMotorLeftB->setSpeed(iPowI);
  myMotorLeftB->run(FORWARD);
  myMotorLeftB->run(RELEASE);
  myMotorRightF->setSpeed(iPowD);
  myMotorRightF->run(BACKWARD);
  myMotorRightF->run(RELEASE);
  myMotorRightB->setSpeed(iPowD);
  myMotorRightB->run(BACKWARD);
  myMotorRightB->run(RELEASE);
  iPowI = iPowi;
  iPowD = iPowd;
  iTamano = iT;
  fRef = fDeseado = eCount1 = cVictima = cParedes = 0;
  real = r;
  kp = 1;
  ki = 0;
  kpA = 0;//4
  iRampa = 17;
  pos = 90;
  myservo.attach(pin_Servo);
  myservo.write(pos);
  pinMode(lVictima,OUTPUT);
  alinear = false;
  kParedAlinear = 10;
  encoder30 = 1200;
  SampleTime = 35;
  ITerm = 0;
  vueltasDadas = 0;
  fSetPoint = fSet = 0;
}

void Movimiento::velocidad(int PowI, int PowD) {
  myMotorLeftF->setSpeed(PowI);
  myMotorRightF->setSpeed(PowD);
  myMotorLeftB->setSpeed(PowI);
  myMotorRightB->setSpeed(PowD);
}


void Movimiento::Stop(){
  myMotorRightB->run(RELEASE);
  myMotorLeftB->run(RELEASE);
  myMotorRightF->run(RELEASE);
  myMotorLeftF->run(RELEASE);
  eCount1 = 0;
}
void Movimiento::StopX(){
  myMotorRightB->run(BREAK);
  myMotorLeftB->run(BREAK);
  myMotorRightF->run(BREAK);
  myMotorLeftF->run(BREAK);
}
void Movimiento::Front(uint8_t PowD, uint8_t PowI){
  myMotorLeftF->setSpeed(PowI);
  myMotorRightF->setSpeed(PowD);
  myMotorLeftB->setSpeed(PowI);
  myMotorRightB->setSpeed(PowD);

  myMotorRightF->run(FORWARD);
  myMotorRightB->run(FORWARD);

  myMotorLeftF->run(FORWARD);
  myMotorLeftB->run(BACKWARD);
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
  myMotorLeftB->run(FORWARD);
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
  myMotorLeftB->run(BACKWARD);
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
  myMotorLeftB->run(FORWARD);
  //while(digitalRead(lack) == 0){
  //	Stop();
  //}
}
void Movimiento::SepararPared(){
  unsigned long ahora = millis();
  uint8_t iActual = real->sensarEnfrentePared(), iPowDD;
  real->escribirLCD("SEPARA");
  while (iActual != 6 && ahora+2000 > millis()) {
	iActual = real->sensarEnfrentePared();
	if(iActual < 6) {     //Muy cerca
	  iPowDD = 80 + (6-iActual);
	  Back(iPowDD, iPowDD);
	}
	else if(iActual > 6) {      //Muy lejos
	  iPowDD = 80 + (iActual-6);
	  Front(iPowDD, iPowDD);
	}
  }
  if(ahora+2000 <= millis()) {
	Back(80, 80);
	delay(500);
  }
  eCount1 = 0;
}
void Movimiento::AlineaPA(char cDir){
  if(alinear) {
	Back(80, 80);
	delay(1200);
	Stop();
	fRef = 0;
	for(int i=0; i < 10; i++) {
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

void Movimiento::vueltaIzq(char &cDir) {
  real->escribirLCD("Vuelta IZQ");
  vueltasDadas++;
  float posInicial, limInf, limSup;
  posInicial = real->getAngulo();

  switch(cDir) {
    case 'n':
    	cDir = 'w';
    	break;
    case 'e':
    	cDir = 'n';
    	break;
    case 's':
    	cDir = 'e';
    	break;
    case 'w':
    	cDir = 's';
    	break;
  }



  fSetPoint -= 90;
  if(fSetPoint < 0)
    fSetPoint += 360;
  fSet = fSetPoint;

  real->escribirLCD(String(posInicial), String(fSet));

  limInf = fSetPoint - kPRECISION_IMU;
  if(limInf < 0)
    limInf += 360;

  limSup = fSetPoint + kPRECISION_IMU;
  if(limSup >= 360.0)
    limSup -= 360;

  unsigned long inicio = millis();
  Left(140, 140);

  if(limSup > limInf) {
      while(posInicial < limInf || posInicial > limSup) {
          posInicial = real->getAngulo();
          if (millis() >= inicio + 10000) {
              velocidad(160, 160);
          } else if (millis() >= inicio + 5000) {
              velocidad(170, 170);
          }
      }
  } else {
      while(posInicial < limInf && posInicial > limSup) {
          posInicial = real->getAngulo();
          if (millis() >= inicio + 10000) {
            velocidad(160, 160);
          } else if (millis() >= inicio + 5000) {
              velocidad(170, 170);
          }
      }
  }
  Stop();
  velocidad(180, 180);
}

void Movimiento::vueltaDer(char &cDir) {
  real->escribirLCD("Vuelta DER");
  vueltasDadas++;
  float posInicial, limInf, limSup;
  posInicial = real->getAngulo();

  switch(cDir) {
    case 'n':
    	cDir = 'e';
    	break;
    case 'e':
    	cDir = 's';
    	break;
    case 's':
    	cDir = 'w';
    	break;
    case 'w':
    	cDir = 'n';
    	break;
  }

  fSetPoint += 90;
  if(fSetPoint >= 360.0)
    fSetPoint -= 360;
  fSet = fSetPoint;

  limInf = fSetPoint - kPRECISION_IMU;
  if(limInf < 0)
    limInf += 360;

  limSup = fSetPoint + kPRECISION_IMU;
  if(limSup >= 360.0)
    limSup -= 360;

  unsigned long inicio = millis();
  Right(140, 140);

  if(limSup > limInf) {
      while(posInicial < limInf || posInicial > limSup) {
          posInicial = real->getAngulo();
          real->escribirLCD(String(posInicial), String(fSet));
          if (millis() >= inicio + 10000) {
              velocidad(160, 160);
          } else if (millis() >= inicio + 5000) {
              velocidad(170, 170);
          }
      }
  } else {
      while(posInicial < limInf && posInicial > limSup) {
          posInicial = real->getAngulo();
          real->escribirLCD(String(posInicial), String(fSet));
          if (millis() >= inicio + 10000) {
            velocidad(160, 160);
          } else if (millis() >= inicio + 5000) {
              velocidad(170, 170);
          }
      }
  }
  real->escribirLCD("Aqui estoy: " + String(real->getAngulo()), String(fSet));
  StopX();
  delay(5);
  Stop();
  velocidad(180, 180);
}

void Movimiento::ErrorGradosVuelta(float &grados) {
	  grados = real->sensarOrientacion();
    int iM;
    grados = (grados >= 360) ? 0 - fDeseado : grados - fDeseado;
    if(grados > 180){
      iM = grados/180;
      grados = -( 180 - (grados - (iM + 180)));
    }else if(grados < -180){
      grados *= -1;
      iM = grados/180;
      grados = ( 180 - (grados - (iM + 180)));
    }
}

void Movimiento::VueltaGyro(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso) {
  ITerm = 0;
  int iPowDD;
  float error = 10;
  uint8_t iCase;
  unsigned long ahora = millis();
  while (error < -5 || error > 5) {
	ErrorGradosVuelta(error);
	/*while(Serial2.available())
	  cVictima = (char)Serial2.read();
	if(cVictima&0b00000010 && !tMapa[iPiso][iRow][iCol].victima()) {
	  iCase = (cVictima&0b00000001) ? 1 : 2;
	  real->escribirLCD("Victima");
	  Stop();
	  dejarKit(tMapa, cDir, iCol, iRow, iPiso, iCase);
	}*/
	if (error < 0) {
	  iPowDD = 100 + (error * (-1) * kp);
	  iPowDD = iPowDD > 180 ? 180 : iPowDD;
	  Right(iPowDD, iPowDD);
	}
	else {
	  iPowDD = 100 + (error * kp);
	  iPowDD = iPowDD > 180 ? 180 : iPowDD;
	  Left(iPowDD, iPowDD);
	}
	if(millis()-ahora > 4000) {
	  Front(150, 150);
	  delay(200);
	  ahora = millis();
	}
  }
}
void Movimiento::potenciasDerecho(int &potenciaDer, int &potenciaIzq) {
/*
	unsigned long now = millis();

  if((now - lastTime) >= SampleTime) {
	int pDeseadoIzq = 5, pDeseadoDer = 5, potenciaIzq_Act, potenciaDer_Act, iError;

	int distanciaIzq = real->sensarIzquierdaPared(), distanciaDer = real->sensarDerechaPared();
	float error;


	if(distanciaIzq < 15 && distanciaDer < 15) {
	  iError = distanciaDer - distanciaIzq;
    ITerm += iError;
    if(ITerm > 50) ITerm = 50;
    else if(ITerm < -50) ITerm = -50;
	  contadorIzq++;
	  contadorDer++;
	  if(iError <= -2) {
		// Se tiene que mover a la izquierda
		potenciaIzq_Act = iPowI - iError * kParedAlinear - (ITerm * ki);
		potenciaDer_Act = iPowD + iError * kParedAlinear + (ITerm * ki);
	  } else if(iError >= 2) {
		// Se tiene que mover a la derecha
		potenciaIzq_Act = iPowI + iError * kParedAlinear + (ITerm * ki);
		potenciaDer_Act = iPowD - iError * kParedAlinear - (ITerm * ki);
	  } else {
		// Alinearse con las dos paredes
		// TODO
	  }
	} else if(distanciaIzq < 15) {
	  contadorIzq++;
	  iError = pDeseadoIzq - distanciaIzq;
    ITerm += iError;
    if(ITerm > 50) ITerm= 50;
    else if(ITerm < -50) ITerm= -50;
	  potenciaDer_Act = iPowD - iError * kParedAlinear - (ITerm * ki);
	  potenciaIzq_Act = iPowI + iError * kParedAlinear + (ITerm * ki);
	} else if(distanciaDer < 15) {
	  contadorDer++;
	  iError = pDeseadoDer - distanciaDer;
    ITerm += iError;
    if(ITerm > 50) ITerm= 50;
    else if(ITerm < -50) ITerm= -50;
	  potenciaDer_Act = iPowD + iError * kParedAlinear + (ITerm * ki);
	  potenciaIzq_Act = iPowI - iError * kParedAlinear - (ITerm * ki);
	} else{
    ITerm = 0;
	  potenciaDer_Act = potenciaIzq_Act = 200;
	}

	//ErrorGradosVuelta(error);
	potenciaDer = iPowD + (error * kpA) + potenciaDer_Act;
	potenciaIzq = iPowI - (error * kpA) + potenciaIzq_Act;
	potenciaDer/=2;
	potenciaIzq/=2;
	potenciaIzq = potenciaIzq > 250 ? 250 : (potenciaIzq < 0 ? 0 : potenciaIzq);
	potenciaDer = potenciaDer > 250 ? 250 : (potenciaDer < 0 ? 0 : potenciaDer);

	lastTime = now;
}*/

float angle = real->getAngulo();
if(angle > 320) {
    inIzq = - (360 - angle);
    inDer = - (360 - angle);
} else {
    inIzq = angle;
    inDer = angle;
}

izqPID.Compute();
derPID.Compute();
potenciaIzq = iPowI + outIzq;
potenciaDer = iPowD + outDer;



}

void Movimiento::pasaRampa(char cDir){
	while(Serial2.available())
		cVictima = (char)Serial2.read();
	real->escribirLCD("Rampa");
	int iPowII, iPowDD;
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
		potenciasDerecho(iPowDD, iPowII);
		Front(iPowDD, iPowII);
	}
	cParedes = 0;
	Front(100, 100);
	delay(800);
	Stop();
	if(real->sensarEnfrentePared() < iRampa){
   		SepararPared();
   	}
   	Stop();
}

void Movimiento::dejarKit(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, uint8_t iCase){
  tMapa[iPiso][iRow][iCol].victima(true);
  real->escribirLCD("DEJAR KIT");
  digitalWrite(lVictima, HIGH);
  float fTemp, fDeseadoT;
  if(real->sensarRampa() < iRampa && real->sensarRampa() > -iRampa) {
	switch(iCase) {
	case 2:                     //izq
	  fDeseadoT = fDeseado < 90 ? fDeseado + 270 : fDeseado - 90;
	  break;
	case 1:                     //der
	  fDeseadoT = fDeseado > 270 ? fDeseado - 270 : fDeseado + 90;
	  break;
	}
	fTemp = fDeseado;
	fDeseado = fDeseadoT;
	VueltaGyro(tMapa, cDir, iCol, iRow, iPiso);
	fDeseado = fTemp;
  }
  Stop();
  for (pos = 90; pos <= 165; pos += 1) {   // goes from 180 degrees to 0 degrees
	myservo.write(pos);                    // tell servo to go to position in variable 'pos'
	delay(15);                             // waits 15ms for the servo to reach the position
  }
  for (pos = 165; pos >= 90; pos -= 1) {       // goes from 0 degrees to 180 degrees
	myservo.write(pos);                    // tell servo to go to position in variable 'pos'
	delay(15);                             // waits 15ms for the servo to reach the position
  }
  VueltaGyro(tMapa, cDir, iCol, iRow, iPiso);
  Stop();
  digitalWrite(lVictima, LOW);
  while(Serial2.available())
	cVictima = (char)Serial2.read();
}


void Movimiento::retroceder(Tile tMapa[3][10][10],  char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso){
  real->escribirLCD("NEGRO");
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
  while(eCount1 < encoder30) {
	potenciasDerecho(iPowDD, iPowII);
	Back(iPowII, iPowDD);
  }
  eCount1 = 0;
}

void Movimiento::acomodaChoque(uint8_t switchCase){
  real->escribirLCD("AUCH");
  int encoderTemp = eCount1;
  Stop();
  switch(switchCase) {
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

void Movimiento::avanzar(Tile tMapa[3][10][10], char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso){                                               //Modificarse en realidad
  float bumperMax = 0.0, bumperMin = 0.0;
  cParedes = 0;
  int iPowDD, iPowII;
  uint8_t switchCase, iCase;
  real->apantallanteLCD("FRONT");
  while(Serial2.available())
	cVictima = (char)Serial2.read();
  AlineaPA(cDir);
  switch(cDir) {
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

  eCount1 = 0;
  while(eCount1 < (encoder30 / 2) && real->sensarEnfrentePared() > 7) {
	potenciasDerecho(iPowDD, iPowII);
	Front(iPowDD, iPowII);

	//Calor
	while(Serial2.available())
	  cVictima = (char)Serial2.read();

	if(cVictima&0b00000010 && !tMapa[iPiso][iRow][iCol].victima()) {
	  int countT = eCount1;
	  iCase = (cVictima&0b00000001) ? 1 : 2;
	  real->escribirLCD("Victima");
	  Stop();
	  dejarKit(tMapa, cDir, iCol, iRow, iPiso, iCase);
	  eCount1 = countT;
	}
	switchCase = real->switches();
	if(switchCase > 0 && real->sensarEnfrente()) {
	  acomodaChoque(switchCase);
	}

	//bumper
	bumperMin = real->sensarRampaFloat() < bumperMin ? real->sensarRampaFloat() : bumperMin;
	bumperMax = real->sensarRampaFloat() > bumperMax ? real->sensarRampaFloat() : bumperMax;
	if (bumperMax - bumperMin >= toleranciaBumper)
	  tMapa[iPiso][iRow][iCol].bumper(true);
	cVictima = 0;
  }

  switch(cDir) {
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

  contadorIzq = contadorDer = 0;

  while(eCount1 < encoder30  && real->sensarEnfrentePared() > 7) {
	potenciasDerecho(iPowDD, iPowII);
	Front(iPowDD, iPowII);

	//Calor
	while(Serial2.available())
	  cVictima = (char)Serial2.read();

	if(cVictima&0b00000010 && !tMapa[iPiso][iRow][iCol].victima()) {
	  int countT = eCount1;
	  iCase = (cVictima&0b00000001) ? 1 : 2;
	  real->escribirLCD("Victima");
	  Stop();
	  dejarKit(tMapa, cDir, iCol, iRow, iPiso, iCase);
	  eCount1 = countT;
	}
	switchCase = real->switches();
	if(switchCase > 0 && real->sensarEnfrente()) {
	  acomodaChoque(switchCase);
	}

	//bumper
	bumperMin = real->sensarRampaFloat() < bumperMin ? real->sensarRampaFloat() : bumperMin;
	bumperMax = real->sensarRampaFloat() > bumperMax ? real->sensarRampaFloat() : bumperMax;
	if (bumperMax - bumperMin >= toleranciaBumper)
	  tMapa[iPiso][iRow][iCol].bumper(true);
	cVictima = 0;
  }
  /*StopX();
  delay(5);
  Stop();*/
  if(contadorIzq > 4)
	cParedes |= 0b00000100;

  if(contadorDer > 4)
	cParedes |= 0b00000001;

  real->escribirLCD(String(bumperMin)+" "+String(bumperMax)+" "+String(bumperMax - bumperMin));
  //VueltaGyro(tMapa, cDir, iCol, iRow, iPiso);
  eCount1 = 0;
  if( !real->color() && real->sensarRampa() < iRampa && real->sensarRampa() > -iRampa && real->sensarEnfrentePared() < 20) {
	cParedes |= 0b00000010;
	real->escribirLCD("Separar");
	Back(80, 80);
	delay(100);
	SepararPared();
	iCase = 0;
	real->escribirLCD("Acabe");
  }
}

//Aquí obviamente hay que poner las cosas de moverse con los motores, hasta ahorita es solamente modificar mapa
void Movimiento::derecha(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso){                                                                                                              //Modificarse en realidad
  real->escribirLCD("DER");
  alinear = !(real->sensarIzquierda());
  uint8_t iCase;
  while(Serial2.available())
	cVictima = (char)Serial2.read();
  if(cVictima&0b00000010 && !tMapa[iPiso][iRow][iCol].victima()) {
	iCase = (cVictima&0b00000001) ? 1 : 2;
	real->escribirLCD("Victima");
	Stop();
	dejarKit(tMapa, cDir, iCol, iRow, iPiso, iCase);
  }
  switch(cDir) {
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
  //VueltaGyro(tMapa, cDir, iCol, iRow, iPiso);
  VueltaGyro(tMapa, cDir, iCol, iRow, iPiso);
}
//Aquí obviamente hay que poner las cosas de moverse con los motores, hasta ahorita es solamente modificar mapa
void Movimiento::izquierda(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso){
  real->escribirLCD("IZQ");
  alinear = !(real->sensarDerecha());
  uint8_t iCase;
  while(Serial2.available())
	cVictima = (char)Serial2.read();
  if(cVictima&0b00000010 && !tMapa[iPiso][iRow][iCol].victima()) {
	iCase = (cVictima&0b00000001) ? 1 : 2;
	real->escribirLCD("Victima");
	Stop();
	dejarKit(tMapa, cDir, iCol, iRow, iPiso, iCase);
  }
  switch(cDir) {
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
  //VueltaGyro(tMapa, cDir, iCol, iRow, iPiso);
  VueltaGyro(tMapa, cDir, iCol, iRow, iPiso);
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
  for(int i = sMov.length()-1; i >= 0; i--) {
	switch(sMov[i]) {
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
  //Stop();
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
	for(uint8_t j = 0; j < iTamano; j++) {
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
  if(mapa.comparaMapa(iMapa, tMapa, cD, iCol, iRow, iNCol, iNRow, iPiso)) {     //Hace las instrucciones que recibe de la función en forma de string
	hacerInstrucciones(tMapa, cDir, iCol, iRow, iPiso, mapa.getInstrucciones(iMapa, cMapa, tMapa, iNCol, iNRow, iPiso));
	//hacerInstrucciones(cMapa, tMapa, cDir, iCol, iRow, iPiso);
	iCol = iNCol, iRow = iNRow;
	return true;
  }
  return false;
}

//La hice bool para que de una forma estuviera como condición de un loop, pero aún no se me ocurre cómo
bool Movimiento::decidir(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso){
  if(tMapa[iPiso][iRow][iCol].cuadroNegro()) {
	retroceder(tMapa, cDir, iCol, iRow, iPiso);
  }
  if(real->sensarEnfrentePared() < 20) {
	SepararPared();
  }
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
  if(cVictima&0b00000010 && !tMapa[iPiso][iRow][iCol].victima()) {
	iCase = (cVictima&0b00000001) ? 1 : 2;
	real->escribirLCD("Victima");
	Stop();
	dejarKit(tMapa, cDir, iCol, iRow, iPiso, iCase);
  }
  //Esto ya no debe de ser necesario con la clase Mapear y SensarRealidad
  tMapa[iPiso][iRow][iCol].existe(true);
  //Esto, no sé si sea mejor tenerlo aquí o en la clase Mapear
  tMapa[iPiso][iRow][iCol].visitado(true);
  //Todos estos sensados los hace con el mapa virtual, por eso dependemos en que el robot sea preciso.
  //Si no hay pared a la derecha Y no está visitado, muevete hacia allá
  if(mapa.sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'r') && mapa.sensaVisitado(tMapa, cDir, iCol, iRow, iPiso, 'r')) {
	derecha(tMapa, cDir, iCol, iRow, iPiso);
	avanzar(tMapa, cDir, iCol, iRow, iPiso);
	Stop();
	return true;
  }
  //Si no hay pared enfrente Y no está visitado, muevete hacia allá
  else if(mapa.sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'u') && mapa.sensaVisitado(tMapa, cDir, iCol, iRow, iPiso, 'u')) {
	avanzar(tMapa, cDir, iCol, iRow, iPiso);
	Stop();
	return true;
  }
  //Si no hay pared a la izquierda y no está visitado, muevete hacia allá
  else if(mapa.sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'l') && mapa.sensaVisitado(tMapa, cDir, iCol, iRow, iPiso, 'l')) {
	izquierda(tMapa, cDir, iCol, iRow, iPiso);
	avanzar(tMapa, cDir, iCol, iRow, iPiso);
	Stop();
	return true;
  }
  //Si no hay pared atrás y no está visitado, muevete hacia allá
  //Sólo entraría a este caso en el primer movimiento de la ronda (si queda mirando a un deadend)
  else if(mapa.sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'd') && mapa.sensaVisitado(tMapa, cDir, iCol, iRow, iPiso, 'd')) {
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
  if(mapa.sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'r') && mapa.sensaVisitado(tMapa, cDir, iCol, iRow, iPiso, 'r')) {
	Serial.println("DER");
	switch(cDir) {
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
  else if(mapa.sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'u') && mapa.sensaVisitado(tMapa, cDir, iCol, iRow, iPiso, 'u')) {
	Serial.println("ENF");
	switch(cDir) {
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
  else if(mapa.sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'l') && mapa.sensaVisitado(tMapa, cDir, iCol, iRow, iPiso, 'l')) {
	Serial.println("IZQ");
	switch(cDir) {
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
  else if(mapa.sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'd') && mapa.sensaVisitado(tMapa, cDir, iCol, iRow, iPiso, 'd')) {
	Serial.println("ATR");
	switch(cDir) {
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

void Movimiento::encoder() {
  eCount1++;

}

char Movimiento::getParedes() {
  return cParedes;
}
