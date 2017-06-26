#include "Arduino.h"
#include "Movimiento.h"
#include <string.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <PID_v1.h>

//////////////////////Define constants///////////////////////////
const uint8_t kToleranciaBumper = 10;
const float kPrecisionImu = 5;             //Ocupa ser float?
const uint8_t kMapSize = 10;
const uint8_t kRampaLimit = 17;
const float kI_Front_Pared = 0;
const uint8_t kP_Front_Pared = 10;
const float kP_Front_Angle = 0;
const int kEncoder30 = 1180;
const int kEncoder15 = kEncoder30/2;
const int kSampleTime = 35;      //Puede ser uint8_t o int, no?
const float kP_Vueltas = 1.3;
const int kDistanciaEnfrente = 6;
const int kMapearPared = 4;

//////////////////////Define pins and motors//////////////////////////
#define lVictima 33
#define pin_Servo 9
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotorRightF = AFMS.getMotor(4);
Adafruit_DCMotor *myMotorRightB = AFMS.getMotor(1);
Adafruit_DCMotor *myMotorLeftF = AFMS.getMotor(2);
Adafruit_DCMotor *myMotorLeftB = AFMS.getMotor(3);//al revés
Servo myservo;
//1 Enfrente derecha
//2 Atras derecha
//3 Enfrente izquierda
//4 Atras izquierda

//////////////////////PID FRONT///////////////////////////
double inIzq, outIzq, inDer, outDer, fSet;

PID izqPID(&inIzq, &outIzq, &fSet, 10, 0, 0, DIRECT);
PID derPID(&inDer, &outDer, &fSet, 10, 0, 0, REVERSE);

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
Movimiento::Movimiento(uint8_t iPowd, uint8_t iPowi, SensarRealidad *r, char *c, uint8_t *ic, uint8_t *ir, uint8_t *ip){
  ////////////////////////Inicializamos los motores y Servo/////////////////////////
  iPowI = iPowi;
  iPowD = iPowd;
  servo_pos = 90;
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
  myservo.attach(pin_Servo);
  myservo.write(servo_pos);
  //////////////////Inicializamos variables en 0////////////////////////////////
  fRef = fDeseado = eCount1 = cVictima = cParedes = ITerm = vueltasDadas = fSetPoint = fSet = alinear = 0;
  //////////////////////////Inicializamos el apuntador a los sensores, posición y LED//////////////////////
  real = r, iCol = ic, iRow = ir, iPiso = ip, cDir = c;
  pinMode(lVictima,OUTPUT);
  //////////////////////////Inicializamos del PID de irse derecho
  izqPID.SetMode(AUTOMATIC);
  derPID.SetMode(AUTOMATIC);
}

void Movimiento::velocidad(int PowI, int PowD) {
  if(PowI > 255) PowI = 255;
  if(PowD > 255) PowD = 255;
  myMotorLeftF->setSpeed(PowI);
  myMotorRightF->setSpeed(PowD);
  myMotorLeftB->setSpeed(PowI);
  myMotorRightB->setSpeed(PowD);
}


void Movimiento::Stop() {
  velocidad(255, 255);

  myMotorRightB->run(BREAK);
  myMotorLeftB->run(BREAK);
  myMotorRightF->run(BREAK);
  myMotorLeftF->run(BREAK);

  delay(50);

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
  myMotorLeftB->run(BACKWARD);
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
}
void Movimiento::SepararPared(){
  unsigned long ahora = millis();
  uint8_t iActual = real->sensarEnfrentePared(), iPowDD;
  real->escribirLCD("SEPARA");
  while (iActual != kDistanciaEnfrente && ahora+2000 > millis()) {
	iActual = real->sensarEnfrentePared();
	if(iActual < kDistanciaEnfrente) {     //Muy cerca
	  iPowDD = 80 + (kDistanciaEnfrente-iActual);
	  Back(iPowDD, iPowDD);
	}
	else if(iActual > kDistanciaEnfrente) {      //Muy lejos
	  iPowDD = 80 + (iActual-kDistanciaEnfrente);
	  Front(iPowDD, iPowDD);
	}
  }
  if(ahora+2000 <= millis()) {
	Back(80, 80);
	delay(500);
  }
  eCount1 = 0;
}
void Movimiento::AlineaPA(){
  if(alinear) {
	Back(80, 80);
	delay(1200);
	Stop();
	fRef = 0;
	for(int i=0; i < 10; i++) {
	  fRef += real->sensarOrientacion();
	}
	fRef /= 10;
	switch(*cDir)
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

void Movimiento::vueltaIzq(Tile tMapa[3][10][10]) {
  real->escribirLCD("Vuelta IZQ");
  vueltasDadas++;
  int potIzq, potDer, dif;
  //unsigned long inicio = millis();
  float posInicial, limInf, limSup;

  fSetPoint -= 90;
  if(fSetPoint < 0) fSetPoint += 360;
  fSet = fSetPoint;

  limInf = fSetPoint - kPrecisionImu;
  if(limInf < 0) limInf += 360;

  limSup = fSetPoint + kPrecisionImu;
  if(limSup >= 360.0) limSup -= 360;

  posInicial = real->getAngulo();
  potIzq = potDer = 85;
  Left(200, 200);

  if(limSup > limInf) {
      while(posInicial < limInf || posInicial > limSup) {
        posInicial = real->getAngulo();
        dif = abs(fSetPoint - posInicial);
        if(dif > 90) dif = 90;
        dif *= kP_Vueltas;
        velocidad(potIzq + dif, potDer + dif);
        /*if (millis() >= inicio + 10000) {
            velocidad(160, 160);
        } else if (millis() >= inicio + 5000) {
            velocidad(170, 170);
        }*/
      }
  } else {
      while(posInicial < limInf && posInicial > limSup) {
        posInicial = real->getAngulo();
        dif = abs(fSetPoint - posInicial);
        if(dif > 90) dif = 90;
        dif *= kP_Vueltas;
        velocidad(potIzq + dif, potDer + dif);
        /*x = 85 + abs(fSetPoint - posInicial) * 1.2 / 90;
        y = 85 + abs(fSetPoint - posInicial) * 1.2 / 90;
        velocidad(x,y);*/
        /*if (millis() >= inicio + 10000) {
          velocidad(160, 160);
        } else if (millis() >= inicio + 5000) {
            velocidad(170, 170);
        }*/
      }
  }
  Stop();
  velocidad(iPowI, iPowD);
}

void Movimiento::vueltaDer(Tile tMapa[3][10][10]) {
  real->escribirLCD("Vuelta DER");
  vueltasDadas++;
  int potIzq, potDer, dif;
  //unsigned long inicio = millis();
  float posInicial, limInf, limSup;

  fSetPoint += 90;
  if(fSetPoint >= 360) fSetPoint -= 360;
  fSet = fSetPoint;

  limInf = fSetPoint - kPrecisionImu;
  if(limInf < 0) limInf += 360;

  limSup = fSetPoint + kPrecisionImu;
  if(limSup >= 360.0) limSup -= 360;

  posInicial = real->getAngulo();
  potIzq = potDer = 85;
  Right(200, 200);

  if(limSup > limInf) {
      while(posInicial < limInf || posInicial > limSup) {
        posInicial = real->getAngulo();
        dif = abs(fSetPoint - posInicial);
        if(dif > 90) dif = 90;
        dif *= kP_Vueltas;
        velocidad(potIzq + dif, potDer + dif);
        /*if (millis() >= inicio + 10000) {
            velocidad(160, 160);
        } else if (millis() >= inicio + 5000) {
            velocidad(170, 170);
        }*/
      }
  } else {
      while(posInicial < limInf && posInicial > limSup) {
        posInicial = real->getAngulo();
        dif = abs(fSetPoint - posInicial);
        if(dif > 90) dif = 90;
        dif *= kP_Vueltas;
        velocidad(potIzq + dif, potDer + dif);
        /*if (millis() >= inicio + 10000) {
          velocidad(160, 160);
        } else if (millis() >= inicio + 5000) {
            velocidad(170, 170);
        }*/
      }
  }
  Stop();
  velocidad(iPowI, iPowD);
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

void Movimiento::potenciasDerecho(int &potenciaDer, int &potenciaIzq) {
/*
	unsigned long now = millis();

  if((now - lastTime) >= kSampleTime) {
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
		potenciaIzq_Act = iPowI - iError * kP_Front_Pared - (ITerm * kI_Front_Pared);
		potenciaDer_Act = iPowD + iError * kP_Front_Pared + (ITerm * kI_Front_Pared);
	  } else if(iError >= 2) {
		// Se tiene que mover a la derecha
		potenciaIzq_Act = iPowI + iError * kP_Front_Pared + (ITerm * kI_Front_Pared);
		potenciaDer_Act = iPowD - iError * kP_Front_Pared - (ITerm * kI_Front_Pared);
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
	  potenciaDer_Act = iPowD - iError * kP_Front_Pared - (ITerm * kI_Front_Pared);
	  potenciaIzq_Act = iPowI + iError * kP_Front_Pared + (ITerm * kI_Front_Pared);
	} else if(distanciaDer < 15) {
	  contadorDer++;
	  iError = pDeseadoDer - distanciaDer;
    ITerm += iError;
    if(ITerm > 50) ITerm= 50;
    else if(ITerm < -50) ITerm= -50;
	  potenciaDer_Act = iPowD + iError * kP_Front_Pared + (ITerm * kI_Front_Pared);
	  potenciaIzq_Act = iPowI - iError * kP_Front_Pared - (ITerm * kI_Front_Pared);
	} else{
    ITerm = 0;
	  potenciaDer_Act = potenciaIzq_Act = 200;
	}

	//ErrorGradosVuelta(error);
	potenciaDer = iPowD + (error * kP_Front_Angle) + potenciaDer_Act;
	potenciaIzq = iPowI - (error * kP_Front_Angle) + potenciaIzq_Act;
	potenciaDer/=2;
	potenciaIzq/=2;
	potenciaIzq = potenciaIzq > 250 ? 250 : (potenciaIzq < 0 ? 0 : potenciaIzq);
	potenciaDer = potenciaDer > 250 ? 250 : (potenciaDer < 0 ? 0 : potenciaDer);

	lastTime = now;
}*/

  float angle = real->getAngulo();

  if((*cDir) == 'n' && angle > 270) {
    inIzq = angle - 360;
    inDer = angle - 360;
  } else {
    inIzq = angle;
    inDer = angle;
  }

  real->escribirLCD(String(angle), String(outIzq) + "    " + String(outDer));

  izqPID.Compute();
  derPID.Compute();
  potenciaIzq = iPowI + outIzq;
  potenciaDer = iPowD + outDer;

  delay(20);
}

void Movimiento::pasaRampa(){
	while(Serial2.available())
		cVictima = (char)Serial2.read();
	real->escribirLCD("Rampa");
	int iPowII, iPowDD;
	switch(*cDir)
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
	while(real->sensarRampa() < -kRampaLimit || real->sensarRampa() > kRampaLimit){
		potenciasDerecho(iPowDD, iPowII);
		Front(iPowDD, iPowII);
	}
	cParedes = 0;
	Front(100, 100);
	delay(800);
	Stop();
	if(real->sensarEnfrentePared() < kRampaLimit){
   		SepararPared();
   	}
   	Stop();
}

void Movimiento::dejarKit(Tile tMapa[3][10][10], uint8_t iCase){
  tMapa[*iPiso][*iRow][*iCol].victima(true);
  real->escribirLCD("DEJAR KIT");
  digitalWrite(lVictima, HIGH);
  float fTemp, fDeseadoT;
  if(real->sensarRampa() < kRampaLimit && real->sensarRampa() > -kRampaLimit) {
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
	fDeseado = fTemp;
  }
  Stop();
  for (servo_pos = 90; servo_pos <= 165; servo_pos += 1) {   // goes from 180 degrees to 0 degrees
	myservo.write(servo_pos);                                  // tell servo to go to position in variable 'pos'
	delay(15);                                                 // waits 15ms for the servo to reach the position
  }
  for (servo_pos = 165; servo_pos >= 90; servo_pos -= 1) {   // goes from 0 degrees to 180 degrees
	myservo.write(servo_pos);                                  // tell servo to go to position in variable 'pos'
	delay(15);                                                 // waits 15ms for the servo to reach the position
  }
  Stop();
  digitalWrite(lVictima, LOW);
  while(Serial2.available())
	cVictima = (char)Serial2.read();
}


void Movimiento::retroceder(Tile tMapa[3][10][10]){
  real->escribirLCD("NEGRO");
  int iPowDD, iPowII;
  switch(*cDir)
  {
  case 'n':
	fDeseado = fRef;
	(*iRow)++;
	break;
  case 'e':
	fDeseado = fRef < 270 ? fRef+90 : fRef-270;
	(*iCol)--;
	break;
  case 's':
	fDeseado = fRef < 180 ? fRef+180 : fRef-180;
	(*iRow)--;
	break;
  case 'w':
	fDeseado = fRef < 90 ? fRef+270 : fRef-90;
	(*iCol)++;
	break;
  }
  while(eCount1 < kEncoder30) {
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

void Movimiento::avanzar(Tile tMapa[3][10][10]){
  float bumperMax = 0.0, bumperMin = 0.0;
  cParedes = 0;
  int iPowDD, iPowII;
  uint8_t switchCase, iCase;
  real->apantallanteLCD("FRONT");
  while(Serial2.available())
	cVictima = (char)Serial2.read();
  /*AlineaPA(cDir);
  switch(*cDir) {
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
}*/

  eCount1 = 0;
  Front(iPowDD, iPowII);
  while(eCount1 < kEncoder15 && real->sensarEnfrentePared() > kDistanciaEnfrente) {
  	potenciasDerecho(iPowDD, iPowII);
    velocidad(iPowII, iPowDD);

  	//Calor
  	/*while(Serial2.available())
  	  cVictima = (char)Serial2.read();

  	if(cVictima&0b00000010 && !tMapa[*iPiso][*iRow][*iCol].victima()) {
  	  int countT = eCount1;
  	  iCase = (cVictima&0b00000001) ? 1 : 2;
  	  real->escribirLCD("Victima");
  	  Stop();
  	  dejarKit(tMapa, *cDir, *iCol, *iRow, *iPiso, iCase);
  	  eCount1 = countT;
  	}*/
  	/*switchCase = real->switches();
  	if(switchCase > 0 && real->sensarEnfrente()) {
  	  acomodaChoque(switchCase);
  	}*/

  	//bumper
  	bumperMin = real->sensarRampaFloat() < bumperMin ? real->sensarRampaFloat() : bumperMin;
  	bumperMax = real->sensarRampaFloat() > bumperMax ? real->sensarRampaFloat() : bumperMax;
  	if (bumperMax - bumperMin >= kToleranciaBumper)
  	  tMapa[*iPiso][*iRow][*iCol].bumper(true);
  	cVictima = 0;
  }

  switch(*cDir) {
  case 'n':
	(*iRow)--;
	break;
  case 'e':
	(*iCol)++;
	break;
  case 's':
	(*iRow)++;
	break;
  case 'w':
	(*iCol)--;
	break;
  }

  contadorIzq = contadorDer = 0;

  while(eCount1 < kEncoder30  && real->sensarEnfrentePared() > kDistanciaEnfrente) {
	potenciasDerecho(iPowDD, iPowII);
	Front(iPowDD, iPowII);

	//Calor
	while(Serial2.available())
	  cVictima = (char)Serial2.read();

	if(cVictima&0b00000010 && !tMapa[*iPiso][*iRow][*iCol].victima()) {
	  int countT = eCount1;
	  iCase = (cVictima&0b00000001) ? 1 : 2;
	  real->escribirLCD("Victima");
	  Stop();
	  dejarKit(tMapa, iCase);
	  eCount1 = countT;
	}
	switchCase = real->switches();
	if(switchCase > 0 && real->sensarEnfrente()) {
	  acomodaChoque(switchCase);
	}

	//bumper
	bumperMin = real->sensarRampaFloat() < bumperMin ? real->sensarRampaFloat() : bumperMin;
	bumperMax = real->sensarRampaFloat() > bumperMax ? real->sensarRampaFloat() : bumperMax;
	if (bumperMax - bumperMin >= kToleranciaBumper)
	  tMapa[*iPiso][*iRow][*iCol].bumper(true);
	cVictima = 0;
  }
  Stop();
  if(contadorIzq > kMapearPared)
	cParedes |= 0b00000100;

  if(contadorDer > kMapearPared)
	cParedes |= 0b00000001;

  eCount1 = 0;
  if( !real->color() && real->sensarRampa() < kRampaLimit && real->sensarRampa() > -kRampaLimit && real->sensarEnfrentePared() < 20) {
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
void Movimiento::derecha(Tile tMapa[3][10][10]){                                                                                                              //Modificarse en realidad
  real->escribirLCD("DER");
  alinear = !(real->sensarIzquierda());
  uint8_t iCase;
  while(Serial2.available())
    cVictima = (char)Serial2.read();
  if(cVictima&0b00000010 && !tMapa[*iPiso][*iRow][*iCol].victima()) {
    	iCase = (cVictima&0b00000001) ? 1 : 2;
    	real->escribirLCD("Victima");
    	Stop();
    	dejarKit(tMapa, iCase);
  }
  switch(*cDir) {
    case 'n':
  	fDeseado = fRef < 270 ? fRef+90 : fRef-270;
   	*cDir='e';
   	break;
    case 'e':
   	fDeseado = fRef < 180 ? fRef+180 : fRef-180;
   	(*cDir) = 's';
   	break;
    case 's':
   	fDeseado = fRef < 90 ? fRef+270 : fRef-90;
   	(*cDir) = 'w';
   	break;
    case 'w':
   	fDeseado = fRef;
   	(*cDir) = 'n';
   	break;
  }
  vueltaDer(tMapa);
}
//Aquí obviamente hay que poner las cosas de moverse con los motores, hasta ahorita es solamente modificar mapa
void Movimiento::izquierda(Tile tMapa[3][10][10]){
  real->escribirLCD("IZQ");
  alinear = !(real->sensarDerecha());
  uint8_t iCase;
  while(Serial2.available())
	cVictima = (char)Serial2.read();
  if(cVictima&0b00000010 && !tMapa[*iPiso][*iRow][*iCol].victima()) {
	iCase = (cVictima&0b00000001) ? 1 : 2;
	real->escribirLCD("Victima");
	Stop();
	dejarKit(tMapa, iCase);
  }
  switch(*cDir) {
  case 'n':
	fDeseado = fRef < 90 ? fRef+270 : fRef-90;
	(*cDir) = 'w';
	break;
  case 'e':
	fDeseado = fRef;
	(*cDir) = 'n';
	break;
  case 's':
	fDeseado = fRef < 270 ? fRef+90 : fRef-270;
	(*cDir) = 'e';
	break;
  case 'w':
	fDeseado = fRef < 180 ? fRef+180 : fRef-180;
	(*cDir) = 's';
	break;
  }
  vueltaIzq(tMapa);
}
//Recibe el string de a dónde moverse y ejecuta las acciones llamando a las funciones de arriba
void Movimiento::hacerInstrucciones(Tile tMapa[3][10][10], String sMov){
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
	  derecha(tMapa);
	  avanzar(tMapa);
	  break;
	case 'u':
	  avanzar(tMapa);
	  break;
	case 'l':
	  izquierda(tMapa);
	  avanzar(tMapa);
	  break;
	case 'd':
	  derecha(tMapa);
	  derecha(tMapa);
	  avanzar(tMapa);
	  break;
	}
  }
  //Stop();
}

//Busca dónde está el no visitado usando funciones de aquí y de sensar mapa.
//Aquí imprime el mapa de int
//La funcion comparaMapa se podría modificar para depender si quiero ir al inicio o a un cuadro no visitado
bool Movimiento::goToVisitado(Tile tMapa[3][10][10], char cD){
  //Declara un mapa de int, debe ser del tamaño que el otro mapa. Será mejor declararlo desde un principio del código?
  uint8_t iMapa[10][10];
  char cMapa[10][10];
  //Llena el mapa de 0
  for (uint8_t i = 0; i < kMapSize; i++)
	for(uint8_t j = 0; j < kMapSize; j++) {
	  iMapa[i][j] = 0;
	  cMapa[i][j] = 'n';
	}
  //Pone 1 en donde vamos a iniciar
  iMapa[*iRow][*iCol] = 1;
  cMapa[*iRow][*iCol] = 'i';
  //LA FUNCION RECURSIVA
  mapa.llenaMapa(iMapa, cMapa, tMapa, *cDir, *iCol, *iRow, *iPiso);
  ///////////////Imprime el mapa//////////////////////////////
  /*for (uint8_t i = 0; i < kMapSize; ++i){
          for(uint8_t j=0; j<kMapSize; j++){
                  Serial.print(iMapa[i][j]); Serial.print(" ");
          }
          Serial.println();
     }
     Serial.println();
     Serial.println();
     for (uint8_t i = 0; i < kMapSize; ++i){
          for(uint8_t j=0; j<kMapSize; j++){
                  Serial.print(cMapa[i][j]); Serial.print(" ");
          }
          Serial.println();
     }
     delay(5000);*/
  //Nuevas coordenadas a dónde moverse
  uint8_t iNCol = 100, iNRow = 100;
  //Compara las distancias para escoger la más pequeña
  if(mapa.comparaMapa(iMapa, tMapa, cD, *iCol, *iRow, iNCol, iNRow, *iPiso)) {     //Hace las instrucciones que recibe de la función en forma de string
	hacerInstrucciones(tMapa, mapa.getInstrucciones(iMapa, cMapa, tMapa, iNCol, iNRow, *iPiso));
	*iCol = iNCol, *iRow = iNRow;
	return true;
  }
  return false;
}

//La hice bool para que de una forma estuviera como condición de un loop, pero aún no se me ocurre cómo
bool Movimiento::decidir(Tile tMapa[3][10][10]){
  Stop();
  if(tMapa[*iPiso][*iRow][*iCol].cuadroNegro()) {
	retroceder(tMapa);
  }
  if(real->sensarEnfrentePared() < 20) {
	SepararPared();
  }
  switch(*cDir)
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
  if(cVictima&0b00000010 && !tMapa[*iPiso][*iRow][*iCol].victima()) {
	iCase = (cVictima&0b00000001) ? 1 : 2;
	real->escribirLCD("Victima");
	Stop();
	dejarKit(tMapa, iCase);
  }
  //Esto ya no debe de ser necesario con la clase Mapear y SensarRealidad
  tMapa[*iPiso][*iRow][*iCol].existe(true);
  //Esto, no sé si sea mejor tenerlo aquí o en la clase Mapear
  tMapa[*iPiso][*iRow][*iCol].visitado(true);
  //Todos estos sensados los hace con el mapa virtual, por eso dependemos en que el robot sea preciso.
  //Si no hay pared a la derecha Y no está visitado, muevete hacia allá
  if(mapa.sensa_Pared(tMapa, *cDir, *iCol, *iRow, *iPiso, 'r') && mapa.sensaVisitado(tMapa, *cDir, *iCol, *iRow, *iPiso, 'r')) {
	derecha(tMapa);
	avanzar(tMapa);
	Stop();
	return true;
  }
  //Si no hay pared enfrente Y no está visitado, muevete hacia allá
  else if(mapa.sensa_Pared(tMapa, *cDir, *iCol, *iRow, *iPiso, 'u') && mapa.sensaVisitado(tMapa, *cDir, *iCol, *iRow, *iPiso, 'u')) {
	avanzar(tMapa);
	Stop();
	return true;
  }
  //Si no hay pared a la izquierda y no está visitado, muevete hacia allá
  else if(mapa.sensa_Pared(tMapa, *cDir, *iCol, *iRow, *iPiso, 'l') && mapa.sensaVisitado(tMapa, *cDir, *iCol, *iRow, *iPiso, 'l')) {
	izquierda(tMapa);
	avanzar(tMapa);
	Stop();
	return true;
  }
  //Si no hay pared atrás y no está visitado, muevete hacia allá
  //Sólo entraría a este caso en el primer movimiento de la ronda (si queda mirando a un deadend)
  else if(mapa.sensa_Pared(tMapa, *cDir, *iCol, *iRow, *iPiso, 'd') && mapa.sensaVisitado(tMapa, *cDir, *iCol, *iRow, *iPiso, 'd')) {
	derecha(tMapa);
	derecha(tMapa);
	avanzar(tMapa);
	Stop();
	return true;
  }
  //Aquí es cuando entra a lo recursivo
  else{
	//Llama la función recursiva
	return goToVisitado(tMapa, 'n');
  }
}

bool Movimiento::decidir_Prueba(Tile tMapa[3][10][10]){
  //Esto ya no debe de ser necesario con la clase Mapear y SensarRealidad
  tMapa[*iPiso][*iRow][*iCol].existe(true);
  //Esto, no sé si sea mejor tenerlo aquí o en la clase Mapear
  tMapa[*iPiso][*iRow][*iCol].visitado(true);
  //Todos estos sensados los hace con el mapa virtual, por eso dependemos en que el robot sea preciso.
  //Si no hay pared a la derecha Y no está visitado, muevete hacia allá
  if(mapa.sensa_Pared(tMapa, *cDir, *iCol, *iRow, *iPiso, 'r') && mapa.sensaVisitado(tMapa, *cDir, *iCol, *iRow, *iPiso, 'r')) {
	Serial.println("DER");
	switch(*cDir) {
	case 'n':
	  (*iCol)++;
	  (*cDir) = 'e';
	  break;
	case 'e':
	  (*iRow)++;
	  (*cDir) = 's';
	  break;
	case 's':
	  (*iCol)--;
	  (*cDir) = 'w';
	  break;
	case 'w':
	  (*iRow)--;
	  (*cDir) = 'n';
	  break;
	}
	return true;
  }
  //Si no hay pared enfrente Y no está visitado, muevete hacia allá
  else if(mapa.sensa_Pared(tMapa, *cDir, *iCol, *iRow, *iPiso, 'u') && mapa.sensaVisitado(tMapa, *cDir, *iCol, *iRow, *iPiso, 'u')) {
	Serial.println("ENF");
	switch(*cDir) {
	case 'n':
	  (*iRow)--;
	  (*cDir) = 'n';
	  break;
	case 'e':
	  (*iCol)++;
	  (*cDir) = 'e';
	  break;
	case 's':
	  (*iRow)++;
	  (*cDir) = 's';
	  break;
	case 'w':
	  (*iCol)--;
	  (*cDir) = 'w';
	  break;
	}
	return true;
  }
  //Si no hay pared a la izquierda y no está visitado, muevete hacia allá
  else if(mapa.sensa_Pared(tMapa, *cDir, *iCol, *iRow, *iPiso, 'l') && mapa.sensaVisitado(tMapa, *cDir, *iCol, *iRow, *iPiso, 'l')) {
	Serial.println("IZQ");
	switch(*cDir) {
	case 'n':
	  (*iCol)--;
	  (*cDir) = 'w';
	  break;
	case 'e':
	  (*iRow)--;
	  (*cDir) = 'n';
	  break;
	case 's':
	  (*iCol)++;
	  (*cDir) = 'e';
	  break;
	case 'w':
	  (*iRow)++;
	  (*cDir) = 's';
	  break;
	}
	return true;
  }
  //Si no hay pared atrás y no está visitado, muevete hacia allá
  //Sólo entraría a este caso en el primer movimiento de la ronda (si queda mirando a un deadend)
  else if(mapa.sensa_Pared(tMapa, *cDir, *iCol, *iRow, *iPiso, 'd') && mapa.sensaVisitado(tMapa, *cDir, *iCol, *iRow, *iPiso, 'd')) {
	Serial.println("ATR");
	switch(*cDir) {
	case 'n':
	  (*iRow)++;
	  (*cDir) = 's';
	  break;
	case 'e':
	  (*iCol)--;
	  (*cDir) = 'w';
	  break;
	case 's':
	  (*iRow)--;
	  (*cDir) = 'n';
	  break;
	case 'w':
	  (*iCol)++;
	  (*cDir) = 'e';
	  break;
	}
	return true;
  }
  //Aquí es cuando entra a lo recursivo
  else{
	//Llama la función recursiva
	return goToVisitado(tMapa, 'n');
  }
}

void Movimiento::encoder() {
  eCount1++;

}

char Movimiento::getParedes() {
  return cParedes;
}
