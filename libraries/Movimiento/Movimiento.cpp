#include <Arduino.h>
#include <Movimiento.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <utility/Adafruit_MS_PWMServoDriver.h>
#include <PID_v1.h>

//////////////////////Define constants///////////////////////////
const uint8_t kToleranciaBumper = 10;
const double kPrecisionImu = 4.85;
const uint8_t kMapSize = 10;
const uint8_t kRampaLimit = 17;

const int kLimITerm = 100;
const double kP_Ambas_Pared = 0.5;
const double kI_Ambas_Pared = .1;
const double kD_Ambas_Pared = 2;

const double kP_Una_Pared = 0.7;
const double kI_Una_Pared = 0.035;
const double kD_Una_Pared = 2;

const int kEncoder30 = 2400;
const int kEncoder15 = kEncoder30 / 2;
const double kP_Vueltas = 1.4;
const int kDistanciaEnfrente = 65;
const int kDistanciaAtras = 55;
const int kMapearPared = 8;
const int kParedDeseadoIzq = 55; // 105 mm
const int kParedDeseadoDer = 55; // 105 mm

const uint8_t kVelocidadBaseMenor = 100;


//////////////////////Define pins and motors//////////////////////////
#define pin_Servo 10
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotorRightF = AFMS.getMotor(1);
Adafruit_DCMotor *myMotorRightB = AFMS.getMotor(4);
Adafruit_DCMotor *myMotorLeftF = AFMS.getMotor(3);
Adafruit_DCMotor *myMotorLeftB = AFMS.getMotor(2);//al revés
Servo myservo;
//1 Enfrente derecha
//2 Atras derecha
//3 Enfrente izquierda
//4 Atras izquierda

//////////////////////PID FRONT///////////////////////////
double inIzqIMU, outIzqIMU, inDerIMU, outDerIMU, fSetPoint, outIzqPARED, outDerPARED;

PID PID_IMU_izq(&inIzqIMU, &outIzqIMU, &fSetPoint, 1.25, 0, 0, DIRECT);
PID PID_IMU_der(&inDerIMU, &outDerIMU, &fSetPoint, 1.25, 0, 0, REVERSE);

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
Movimiento::Movimiento(uint8_t iPowd, uint8_t iPowi, SensarRealidad *r, char *c, uint8_t *ic, uint8_t *ir, uint8_t *ip, char *cl, uint8_t *icl, uint8_t *irl, uint8_t *ipl, Tile (*tB)[10][10], Tile (*tM)[10][10], uint8_t *iPM, uint8_t *iPML) {
	 //////////////////Inicializamos variables en 0////////////////////////////////
  eCount1 = eCount2 = cVictima = cParedes = iTerm = fSetPoint = iColor = resetIMU = bBoton1 = 0;
  //////////////////////////Inicializamos el apuntador a los sensores, posición y LED//////////////////////
  real = r, iCol = ic, iRow = ir, iPiso = ip, cDir = c, iColLast = icl, iRowLast = irl, iPisoLast = ipl, cDirLast = cl, tBueno = tB, tMapa = tM, iPisoMax = iPM, iPisoMaxLast = iPML;
  //Atom sucks
  ////////////////////////Inicializamos los motores y Servo/////////////////////////
	iPowI = iPowi;
	iPowD = iPowd;
	AFMS.begin();
	velocidad(iPowI, iPowD);
	myservo.attach(pin_Servo);
	myservo.write(90);
	//////////////////////////Inicializamos del PID de irse derecho
	PID_IMU_izq.SetMode(AUTOMATIC);
	PID_IMU_der.SetMode(AUTOMATIC);
}

bool Movimiento::velocidad(uint8_t powIzq, uint8_t powDer) {
	if(powIzq < 0) powIzq = 0;
	if(powDer < 0) powDer = 0;
	if(powIzq > 255) powIzq = 255;
	if(powDer > 255) powDer = 255;

	myMotorLeftF->setSpeed(powIzq);
	myMotorLeftB->setSpeed(powIzq);

	myMotorRightF->setSpeed(powDer);
	myMotorRightB->setSpeed(powDer);
  if(bBoton1){
    bLack = true;
    stop();
    real->escribirLCD("     Perdon", "     Perdon");
    return false;
  }
  return true;
}


void Movimiento::stop() {
	// velocidad(255, 255);

	myMotorLeftF->run(BREAK);
	myMotorLeftB->run(BREAK);
	myMotorRightF->run(BREAK);
	myMotorRightB->run(BREAK);

	delay(40);

	myMotorLeftF->run(RELEASE);
	myMotorLeftB->run(RELEASE);
	myMotorRightF->run(RELEASE);
	myMotorRightB->run(RELEASE);

	delay(40);

	eCount1 = eCount2 = 0;
}

void Movimiento::front() {
	myMotorLeftF->run(BACKWARD);
	myMotorLeftB->run(BACKWARD);

	myMotorRightF->run(BACKWARD);
	myMotorRightB->run(FORWARD);
}

void Movimiento::back() {
	myMotorLeftF->run(FORWARD);
	myMotorLeftB->run(FORWARD);

	myMotorRightF->run(FORWARD);
	myMotorRightB->run(BACKWARD);
}

void Movimiento::right() {
	myMotorLeftF->run(BACKWARD);
	myMotorLeftB->run(BACKWARD);

	myMotorRightF->run(FORWARD);
	myMotorRightB->run(BACKWARD);
}

void Movimiento::left() {
	myMotorLeftF->run(FORWARD);
	myMotorLeftB->run(FORWARD);

	myMotorRightF->run(BACKWARD);
	myMotorRightB->run(FORWARD);
}

void Movimiento::alinearParedEnfrente() {
	unsigned long inicio = millis();
	uint8_t potIzq, potDer;
	potIzq = potDer = kVelocidadBaseMenor;
	int iActual = real->getDistanciaEnfrente();
	real->escribirLCD("Alinear enfrente");
	while ((iActual < kDistanciaEnfrente - 6 || iActual > kDistanciaEnfrente + 6 ) && inicio + 2000 > millis()) {
		if(!velocidad(potIzq, potDer))
      return;
		if(iActual < kDistanciaEnfrente) { //Muy cerca
			back();
		} else { //Muy lejos
			front();
		}
		iActual = real->getDistanciaEnfrente();
	}
	stop();
}

void Movimiento::alinearParedAtras() {
	unsigned long inicio = millis();
	uint8_t potIzq, potDer;
	potIzq = potDer = kVelocidadBaseMenor;
	int iActual = real->getDistanciaAtras();
	real->escribirLCD("Alinear atras");
	while ((iActual < kDistanciaAtras - 6 || iActual > kDistanciaAtras + 6 ) && inicio + 2000 > millis()) {
		if(!velocidad(potIzq, potDer))
      return;
		if(iActual < kDistanciaAtras) { //Muy cerca
			front();
		} else { //Muy lejos
			back();
		}
		iActual = real->getDistanciaAtras();
	}
	stop();
}

void Movimiento::corregirIMU() {
	if(!(real->caminoAtras()) && (resetIMU > 10)) {
		real->escribirLCD("corregir IMU");
		double fRef = 0, angle;
		back();
		if(!velocidad(kVelocidadBaseMenor, kVelocidadBaseMenor))
      return;
		while(real->getDistanciaAtras() > 10) {
			real->escribirLCD(String(real->getDistanciaAtras()));
		}
		delay(600); // TODO: Checar con sensor
		stop();
		delay(100);

		for(int i = 0; i < 5; i++) {
			real->getAngulo(angle);
			fRef += angle;
		}
		fRef /= 5;
		fSetPoint = fRef;

		front();
		if(!velocidad(kVelocidadBaseMenor, kVelocidadBaseMenor))
      return;
		while(real->getDistanciaAtras() < 20) {
			real->escribirLCD(String(real->getDistanciaAtras()));
		}
		//stop();

		resetIMU = 0;
		real->escribirLCD(String(fSetPoint));
	}
}


void Movimiento::vueltaIzq() {
	real->escribirLCD("Vuelta IZQ");
	resetIMU += 2;
	iTerm = 0;
	int potIzq, potDer, dif;
	//unsigned long inicio = millis();
	double posInicial, limInf, limSup;

	double angle;
	if(!real->getAngulo(angle)) {
		fSetPoint = angle;
	}

	fSetPoint -= 90;
	if(fSetPoint < 0) fSetPoint += 360;

	limInf = fSetPoint - kPrecisionImu;
	if(limInf < 0) limInf += 360;

	limSup = fSetPoint + kPrecisionImu;
	if(limSup >= 360.0) limSup -= 360;

	real->getAngulo(posInicial);
	potIzq = potDer = kVelocidadBaseMenor;
	left();
	velocidad(225, 225);

	if(limSup > limInf) {
		while(posInicial < limInf || posInicial > limSup) {
			// TODO implementar correcion
			real->getAngulo(posInicial);
			if(posInicial < 90 && fSetPoint > 270)
				dif = abs(fSetPoint - 360 - posInicial);
			else
				dif = abs(fSetPoint - posInicial);
			if(dif > 90) dif = 90;
			dif *= kP_Vueltas;
      if(!velocidad(potIzq + dif, potDer + dif))
        return;
			/*if (millis() >= inicio + 10000) {
			    velocidad(160, 160);
			   } else if (millis() >= inicio + 5000) {
			    velocidad(170, 170);
			   }*/
		}
	} else {
		while(posInicial < limInf && posInicial > limSup) {
			// TODO implementar correcion
			real->getAngulo(posInicial);
			if(posInicial < 90 && fSetPoint > 270)
				dif = abs(fSetPoint - 360 - posInicial);
			else
				dif = abs(fSetPoint - posInicial);
			if(dif > 90) dif = 90;
			 dif *= kP_Vueltas;
			if(!velocidad(potIzq + dif, potDer + dif))
        return;
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
	//stop();
	if(real->getDistanciaEnfrente() < 200) {
    alinearParedEnfrente();
  }
  else if(real->getDistanciaAtras() < 200) {
    alinearParedAtras();
  }
}

void Movimiento::vueltaDer() {
	real->escribirLCD("Vuelta DER");
	resetIMU += 2;
	iTerm = 0;
	int potIzq, potDer, dif;
	//unsigned long inicio = millis();
	double posInicial, limInf, limSup;

	double angle;
	if(!real->getAngulo(angle)) {
		fSetPoint = angle;
		real->apantallanteLCD("JALOOO");
	}

	fSetPoint += 90;
	if(fSetPoint >= 360) fSetPoint -= 360;

	limInf = fSetPoint - kPrecisionImu;
	if(limInf < 0) limInf += 360;

	limSup = fSetPoint + kPrecisionImu;
	if(limSup >= 360.0) limSup -= 360;

	real->getAngulo(posInicial);
	potIzq = potDer = kVelocidadBaseMenor;
	right();
	velocidad(225, 225);

	if(limSup > limInf) {
		while(posInicial < limInf || posInicial > limSup) {
			// TODO implementar correcion
			real->getAngulo(posInicial);
			if(posInicial > 270 && fSetPoint < 90)
				dif = abs(fSetPoint + 360 - posInicial);
			else
				dif = abs(fSetPoint - posInicial);
			if(dif > 90) dif = 90;
			dif *= kP_Vueltas;
      if(!velocidad(potIzq + dif, potDer + dif))
        return;
			/*if (millis() >= inicio + 10000) {
			    velocidad(160, 160);
			   } else if (millis() >= inicio + 5000) {
			    velocidad(170, 170);
			   }*/
		}
	} else {
		while(posInicial < limInf && posInicial > limSup) {
			// TODO implementar correcion
			real->getAngulo(posInicial);
			if(posInicial > 270 && fSetPoint < 90)
				dif = abs(fSetPoint + 360 - posInicial);
			else
				dif = abs(fSetPoint - posInicial);
			if(dif > 90) dif = 90;
			dif *= kP_Vueltas;
      if(!velocidad(potIzq + dif, potDer + dif))
        return;
			/*if (millis() >= inicio + 10000) {
			   velocidad(160, 160);
			   } else if (millis() >= inicio + 5000) {
			    velocidad(170, 170);
			   }*/
		}
	}
	//stop();
	if(real->getDistanciaEnfrente() < 200) {
    alinearParedEnfrente();
  }
  else if(real->getDistanciaAtras() < 200) {
    alinearParedAtras();
  }
}

void Movimiento::potenciasDerecho(uint8_t &potenciaIzq, uint8_t &potenciaDer) {
	double angle;
	if(!real->getAngulo(angle)) {
		fSetPoint = angle;
	}

	if((*cDir) == 'n' && angle > 270 && fSetPoint < 90) {
		inIzqIMU = angle - 360;
		inDerIMU = angle - 360;
	} else if((*cDir) == 'n' && angle < 90 && fSetPoint > 270) {
		inIzqIMU = angle + 360;
		inDerIMU = angle + 360;
	} else {
		inIzqIMU = angle;
		inDerIMU = angle;
	}
	PID_IMU_izq.Compute();
	PID_IMU_der.Compute();

	//real->escribirLCD(String(angle) + " " + String(inIzqIMU) + " " + String(fSetPoint), String(outDerIMU) + "    " + String(outIzqIMU));

	int distanciaIzq = real->getDistanciaIzquierda(), distanciaDer = real->getDistanciaDerecha(), iError, iParaD;
	if(distanciaIzq < 125 && distanciaDer < 125) {
		contadorIzq++;
		contadorDer++;

		iError = distanciaDer - distanciaIzq;
		if(-5 < iError && iError < 5) iError = 0;

		iTerm += iError;
		if(iTerm > kLimITerm) iTerm = kLimITerm;
		else if(iTerm < -kLimITerm) iTerm = -kLimITerm;

		iParaD = lastInput - iError;
		if(-4 < iParaD && iParaD < 4) iParaD = 0;

		// Se tiene que mover a la izquierda si iError < 0
		// Se tiene que mover a la derecha si iError > 0
		outDerPARED = outIzqPARED = iError * kP_Ambas_Pared - iTerm * kI_Ambas_Pared - iParaD * kD_Ambas_Pared;
		outDerPARED *= -1;

	} else if(distanciaIzq < 125) {
		contadorIzq++;
		iError = kParedDeseadoIzq - distanciaIzq;
		if(-5 < iError && iError < 5) iError = 0;

		// debe ser negativo, creo
		iTerm -= iError;
		if(iTerm > kLimITerm) iTerm = kLimITerm;
		else if(iTerm < -kLimITerm) iTerm = -kLimITerm;

		iParaD = lastInput - iError;
		if(-4 < iParaD && iParaD < 4) iParaD = 0;

		outIzqPARED = iError * kP_Una_Pared - iTerm * kI_Una_Pared - iParaD * kD_Una_Pared;
		outDerPARED = -iError * kP_Una_Pared + iTerm * kI_Una_Pared + iParaD * kD_Una_Pared;
	} else if(distanciaDer < 125) {
		contadorDer++;
		iError = kParedDeseadoDer - distanciaDer;
		if(-5 < iError && iError < 5) iError = 0;

		iTerm += iError;
		if(iTerm > kLimITerm) iTerm = kLimITerm;
		else if(iTerm < -kLimITerm) iTerm = -kLimITerm;

		iParaD = lastInput - iError;
		if(-4 < iParaD && iParaD < 4) iParaD = 0;

		outIzqPARED = -iError * kP_Una_Pared - iTerm * kI_Una_Pared - iParaD * kD_Una_Pared;
		outDerPARED = iError * kP_Una_Pared + iTerm * kI_Una_Pared + iParaD * kD_Una_Pared;
	} else {
		outIzqPARED = outDerPARED = iTerm = 0;
	}
	potenciaIzq = iPowI + outIzqIMU + outIzqPARED;
	potenciaDer = iPowD + outDerIMU + outDerPARED;
	if(potenciaIzq < kVelocidadBaseMenor) potenciaIzq = kVelocidadBaseMenor;
	if(potenciaDer < kVelocidadBaseMenor) potenciaDer = kVelocidadBaseMenor;
	lastInput = iError;

	real->escribirLCD(String(distanciaDer) + "     " + String(distanciaIzq));
	// real->escribirLCD(String(outDerIMU) + "     " + String(outIzqIMU), String(outDerPARED) + "     " + String(outIzqPARED));
}

void Movimiento::pasaRampa() {
	resetIMU += 10;
	while(Serial2.available())
		cVictima = (char)Serial2.read();
	real->apantallanteLCD("Rampa");
	uint8_t iPowII, iPowDD;
	front();
	while(real->sensarRampa() < -kRampaLimit || real->sensarRampa() > kRampaLimit) {
		potenciasDerecho(iPowII, iPowDD);
    if(!velocidad(iPowII, iPowDD))
      return;
	}
	cParedes = 0;
	velocidad(kVelocidadBaseMenor, kVelocidadBaseMenor);
	delay(800);
	stop();
	if(real->getDistanciaEnfrente() < 200) { //Si hay pared enfrente
		alinearParedEnfrente();
	}
}

void Movimiento::dejarKit(uint8_t iCase) {
	stop();
	tMapa[*iPiso][*iRow][*iCol].victima(true);
	if(real->sensarRampa() < kRampaLimit && real->sensarRampa() > -kRampaLimit) {
		switch(iCase) {
		case 1:
			real->apantallanteLCD("VICTIMA con KIT", "derecha");
			myservo.write(70);
			delay(500);
			myservo.write(110);
			delay(500);
			myservo.write(0);
			break;
		case 2:
			real->apantallanteLCD("VICTIMA con KIT", "izquierda");
			myservo.write(110);
			delay(500);
			myservo.write(70);
			delay(500);
			myservo.write(180);
			break;
		}
		delay(1000);
		myservo.write(90);
	}
	while(Serial2.available())
		cVictima = (char)Serial2.read();
}


void Movimiento::retroceder() {
	real->escribirLCD("CUADRO NEGRO");
	uint8_t iPowII = iPowI, iPowDD = iPowD;
	switch(*cDir) {
	case 'n':
		(*iRow)++;
		break;
	case 'e':
		(*iCol)--;
		break;
	case 's':
		(*iRow)--;
		break;
	case 'w':
		(*iCol)++;
		break;
	}
	//stop();
	back();
	while(eCount1 + eCount2 < kEncoder30) {
		potenciasDerecho(iPowDD, iPowII);
		if(!velocidad(iPowII, iPowDD))
      return;
	}
}

void Movimiento::acomodaChoque(uint8_t switchCase) {
	real->escribirLCD("LIMIT");
	//stop();
	uint16_t encoderTemp1 = eCount1;
	uint16_t encoderTemp2 = eCount2;
	switch(switchCase) {
	case 1:
		back();
		velocidad(255, 0);
		delay(400);
		velocidad(0, 255);
		delay(400);
		break;
	case 2:
		back();
		velocidad(0, 255);
		delay(400);
		velocidad(255, 0);
		delay(400);
		break;
	}
	//stop();
	encoderTemp1 -= eCount1;
	encoderTemp2 -= eCount2;
	eCount1 = encoderTemp1;
	eCount2 = encoderTemp2;
}

void Movimiento::avanzar() {
	cParedes = 0;
	resetIMU++;
	double bumperMin = 0.0, bumperMax = 0.0;
	uint8_t iPowII, iPowDD, switchCase, iCase;

	while(Serial2.available())
		cVictima = (char)Serial2.read();

	corregirIMU();
	eCount1 = eCount2 = 0;

	velocidad(iPowI, iPowD);
	front();
	while(eCount1 + eCount2 < kEncoder15 && real->getDistanciaEnfrente() > kDistanciaEnfrente) {
		potenciasDerecho(iPowII, iPowDD);
		if(!velocidad(iPowII, iPowDD))
      return;

		//Calor
		while(Serial2.available())
			cVictima = (char)Serial2.read();

		if(cVictima&0b00000010 && !tMapa[*iPiso][*iRow][*iCol].victima()) {
			iCase = (cVictima&0b00000001) ? 1 : 2;
			dejarKit( iCase);
			front();
		}
		switchCase = real->switches();
		if(switchCase > 0 && real->caminoEnfrente()) {
			acomodaChoque(switchCase);
		}

		//bumper
		bumperMin = real->sensarRampa() < bumperMin ? real->sensarRampa() : bumperMin;
		bumperMax = real->sensarRampa() > bumperMax ? real->sensarRampa() : bumperMax;
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

	front();
	while(eCount1 + eCount2 < kEncoder30  && real->getDistanciaEnfrente() > kDistanciaEnfrente) {
		potenciasDerecho(iPowII, iPowDD);
		if(!velocidad(iPowII, iPowDD))
      return;

		//Calor
		while(Serial2.available())
			cVictima = (char)Serial2.read();


		if(cVictima&0b00000010 && !tMapa[*iPiso][*iRow][*iCol].victima()) {
			iCase = (cVictima&0b00000001) ? 1 : 2;
			dejarKit( iCase);
			front();
		}
		switchCase = real->switches();
		if(switchCase > 0 && real->caminoEnfrente()) {
			acomodaChoque(switchCase);
		}

		//bumper
		bumperMin = real->sensarRampa() < bumperMin ? real->sensarRampa() : bumperMin;
		bumperMax = real->sensarRampa() > bumperMax ? real->sensarRampa() : bumperMax;
		if (bumperMax - bumperMin >= kToleranciaBumper)
			tMapa[*iPiso][*iRow][*iCol].bumper(true);
		cVictima = 0;
	}
	// stop();
	if(contadorIzq > kMapearPared)
		cParedes |= 0b00000100;

	if(contadorDer > kMapearPared)
		cParedes |= 0b00000001;

	eCount1 = eCount2 = 0;
  iColor = real->color();
  if(real->getDistanciaEnfrente() < 200){
    cParedes |= 0b00000010;
  }
	if(iColor != 1  && real->sensarRampa() < abs(kRampaLimit)) {
		if(real->getDistanciaEnfrente() < 200) {
			alinearParedEnfrente();
		}
		else if(real->getDistanciaAtras() < 200) {
			alinearParedAtras();
		}
	}
}

//Aquí obviamente hay que poner las cosas de moverse con los motores, hasta ahorita es solamente modificar mapa
void Movimiento::derecha() {                                                                                                              //Modificarse en realidad
	real->escribirLCD("DER");
	uint8_t iCase;
	while(Serial2.available())
		cVictima = (char)Serial2.read();
	if(cVictima&0b00000010 && !tMapa[*iPiso][*iRow][*iCol].victima()) {
		iCase = (cVictima&0b00000001) ? 1 : 2;
		dejarKit( iCase);
	}
	switch(*cDir) {
	case 'n':
		(*cDir) ='e';
		break;
	case 'e':
		(*cDir) = 's';
		break;
	case 's':
		(*cDir) = 'w';
		break;
	case 'w':
		(*cDir) = 'n';
		break;
	}
	vueltaDer();
}

//Aquí obviamente hay que poner las cosas de moverse con los motores, hasta ahorita es solamente modificar mapa
void Movimiento::izquierda() {
	real->escribirLCD("IZQ");
	uint8_t iCase;
	while(Serial2.available())
		cVictima = (char)Serial2.read();
	if(cVictima&0b00000010 && !tMapa[*iPiso][*iRow][*iCol].victima()) {
		iCase = (cVictima&0b00000001) ? 1 : 2;
		dejarKit( iCase);
	}
	switch(*cDir) {
	case 'n':
		(*cDir) = 'w';
		break;
	case 'e':
		(*cDir) = 'n';
		break;
	case 's':
		(*cDir) = 'e';
		break;
	case 'w':
		(*cDir) = 's';
		break;
	}
	vueltaIzq();
}

//Recibe el string de a dónde moverse y ejecuta las acciones llamando a las funciones de arriba
void Movimiento::hacerInstrucciones(String sMov) {
	real->escribirLCD("PATH");
	///////////////////////////TEST/////////////////////////////////
	/*for(int i = sMov.length()-1; i >= 0; i--) {
	        Serial.print(sMov[i]);
	   }
	   Serial.println(" ");
	   delay(5000);
	   cDir = (char)Serial.read();*/
	for(int i = sMov.length()-1; i >= 0; i--) {
		switch(sMov[i]) {
		case 'r':
			derecha();
			avanzar();
			break;
		case 'u':
			avanzar();
			break;
		case 'l':
			izquierda();
			avanzar();
			break;
		case 'd':
			derecha();
			derecha();
			avanzar();
			break;
		}
    if(tMapa[*iPiso][*iRow][*iCol].checkpoint()){
      stop();
      checkpoint();
    }
    if(bLack){
      return;
    }
	}
	//stop();
}

//Busca dónde está el no visitado usando funciones de aquí y de sensar mapa.
//Aquí imprime el mapa de int
//La funcion comparaMapa se podría modificar para depender si quiero ir al inicio o a un cuadro no visitado
bool Movimiento::goToVisitado(char cD) {
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
	/*for (uint8_t i = 0; i < kMapSize; ++i) {
	        for(uint8_t j=0; j<kMapSize; j++) {
	                Serial.print(iMapa[i][j]); Serial.print(" ");
	        }
	        Serial.println();
	   }
	   Serial.println();
	   Serial.println();
	   for (uint8_t i = 0; i < kMapSize; ++i) {
	        for(uint8_t j=0; j<kMapSize; j++) {
	                Serial.print(cMapa[i][j]); Serial.print(" ");
	        }
	        Serial.println();
	   }
		 delay(5000);*/
	//Nuevas coordenadas a dónde moverse
	uint8_t iNCol = 100, iNRow = 100;
	//Compara las distancias para escoger la más pequeña
	if(mapa.comparaMapa(iMapa, tMapa, cD, *iCol, *iRow, iNCol, iNRow, *iPiso)) { //Hace las instrucciones que recibe de la función en forma de string
		hacerInstrucciones(mapa.getInstrucciones(iMapa, cMapa, tMapa, iNCol, iNRow, *iPiso));
		*iCol = iNCol, *iRow = iNRow;
    if(bLack){
      lack();
    }
		return true;
	}
	return false;
}


//La hice bool para que de una forma estuviera como condición de un loop, pero aún no se me ocurre cómo
bool Movimiento::decidir() {
  bLack = false;
	// stop();
	if(tMapa[*iPiso][*iRow][*iCol].cuadroNegro()) {
		retroceder();
	}
	/*if(real->getDistanciaEnfrente() < 200) {
		alinearParedEnfrente();
	}
	else if(real->getDistanciaAtras() < 200) {
		alinearParedAtras();
	}*/
	uint8_t iCase;
	while(Serial2.available())
		cVictima = (char)Serial2.read();
	if(cVictima&0b00000010 && !tMapa[*iPiso][*iRow][*iCol].victima()) {
		iCase = (cVictima&0b00000001) ? 1 : 2;
		dejarKit(iCase);
	}
	//Esto ya no debe de ser necesario con la clase Mapear y SensarRealidad
	tMapa[*iPiso][*iRow][*iCol].existe(true);
	//Esto, no sé si sea mejor tenerlo aquí o en la clase Mapear
	tMapa[*iPiso][*iRow][*iCol].visitado(true);
	//Todos estos sensados los hace con el mapa virtual, por eso dependemos en que el robot sea preciso.
	//Si no hay pared a la derecha Y no está visitado, muevete hacia allá
	if(mapa.sensa_Pared(tMapa, *cDir, *iCol, *iRow, *iPiso, 'r') && mapa.sensaVisitado(tMapa, *cDir, *iCol, *iRow, *iPiso, 'r')) {
		derecha();
		avanzar();
		stop();
    if(bLack){
      lack();
    }
		return true;
	}
	//Si no hay pared enfrente Y no está visitado, muevete hacia allá
	else if(mapa.sensa_Pared(tMapa, *cDir, *iCol, *iRow, *iPiso, 'u') && mapa.sensaVisitado(tMapa, *cDir, *iCol, *iRow, *iPiso, 'u')) {
		avanzar();
		stop();
    if(bLack){
      lack();
    }
		return true;
	}
	//Si no hay pared a la izquierda y no está visitado, muevete hacia allá
	else if(mapa.sensa_Pared(tMapa, *cDir, *iCol, *iRow, *iPiso, 'l') && mapa.sensaVisitado(tMapa, *cDir, *iCol, *iRow, *iPiso, 'l')) {
		izquierda();
		avanzar();
		stop();
    if(bLack){
      lack();
    }
		return true;
	}
	//Si no hay pared atrás y no está visitado, muevete hacia allá
	//Sólo entraría a este caso en el primer movimiento de la ronda (si queda mirando a un deadend)
	else if(mapa.sensa_Pared(tMapa, *cDir, *iCol, *iRow, *iPiso, 'd') && mapa.sensaVisitado(tMapa, *cDir, *iCol, *iRow, *iPiso, 'd')) {
		derecha();
		derecha();
		avanzar();
		stop();
    if(bLack){
      lack();
    }
		return true;
	}
	//Aquí es cuando entra a lo recursivo
	else{
		//Llama la función recursiva
		return goToVisitado( 'n');
	}
}

/*bool Movimiento::decidir_Prueba() {
        //Esto ya no debe de ser necesario con la clase Mapear y SensarRealidad
        tMapa[*iPiso][*iRow][*iCol].existe(true);
        //Esto, no sé si sea mejor tenerlo aquí o en la clase Mapear
        tMapa[*iPiso][*iRow][*iCol].visitado(true);
        //Todos estos sensados los hace con el mapa virtual, por eso dependemos en que el robot sea preciso.
        //Si no hay pared a la derecha Y no está visitado, muevete hacia allá
        if(mapa.sensa_Pared( *cDir, *iCol, *iRow, *iPiso, 'r') && mapa.sensaVisitado( *cDir, *iCol, *iRow, *iPiso, 'r')) {
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
        else if(mapa.sensa_Pared( *cDir, *iCol, *iRow, *iPiso, 'u') && mapa.sensaVisitado( *cDir, *iCol, *iRow, *iPiso, 'u')) {
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
        else if(mapa.sensa_Pared( *cDir, *iCol, *iRow, *iPiso, 'l') && mapa.sensaVisitado( *cDir, *iCol, *iRow, *iPiso, 'l')) {
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
        else if(mapa.sensa_Pared( *cDir, *iCol, *iRow, *iPiso, 'd') && mapa.sensaVisitado( *cDir, *iCol, *iRow, *iPiso, 'd')) {
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
                return goToVisitado( 'n');
        }
   }*/

void Movimiento::encoder1() {
	eCount1++;
}

void Movimiento::encoder2() {
  eCount2++;
}

void Movimiento::boton1() {
  bBoton1 = true;
}

char Movimiento::getParedes() {
	return cParedes;
}

void Movimiento::checkpoint(){
  stop();
  real->apantallanteLCD("    CHECKkk", "   POINT");
  for(int i = 0; i <= 2; i++){
    for(int j = 0; j < kMapSize; j++){
      for(int z = 0; z < kMapSize; z++){
        tBueno[i][j][z] = tMapa[i][j][z];
      }
    }
  }
  (*iPisoLast) = (*iPiso);
  (*iColLast) = (*iCol);
  (*iRowLast) = (*iRow);
  (*cDirLast) = (*cDir);
  (*iPisoMaxLast) = (*iPisoMax);
}

void Movimiento::lack(){
  double angle;
  stop();
  bBoton1 = false;
  for(int i = 0; i <= 2; i++){
    for(int j = 0; j < kMapSize; j++){
      for(int z = 0; z < kMapSize; z++){
        tMapa[i][j][z] = tBueno[i][j][z];
      }
    }
  }
  real->apantallanteLCD("    WAIT", "   BYT");
  while(!bBoton1){
    real->getAngulo(angle);
    fSetPoint = angle;
  }
  bBoton1 = false;
  (*iPiso) = (*iPisoLast);
  (*iCol) = (*iColLast);
  (*iRow) = (*iRowLast);
  (*cDir) = (*cDirLast);
  (*iPisoMax) = (*iPisoMaxLast);
  real->apantallanteLCD("    BYE", "   BYT");
  while(Serial2.available())
    cVictima = (char)Serial2.read();
  cVictima = 0;
  bLack = true;
  bBoton1 = false;
}

bool Movimiento::getLack(){
  return bLack;
}

//////////////Funcion de impresión de mapa//////////////
void Movimiento::muestra(bool t){
  char cMapa[21][21];
  for(int j = 0; j < 21; j++){
    for(int z = 0; z < 21; z++){
      cMapa[j][z] = 'x';
    }
  }
  if(t){
      int iRowC = 1;
    for(int iRowT = 0; iRowT < 10; iRowT++) {
      int iColC = 1;
      for(int iColT = 0; iColT < 10; iColT++) {
        if(tMapa[(*iPiso)][iRowT][iColT].inicio()) {
          cMapa[iRowC][iColC] = 'i';
        }
        else if(tMapa[(*iPiso)][iRowT][iColT].visitado()) {
          cMapa[iRowC][iColC] = 'v';
        }
        else if(tMapa[(*iPiso)][iRowT][iColT].existe()) {
          cMapa[iRowC][iColC] = 'n';
        }
        else{
          cMapa[iRowC][iColC] = 'x';
        }
        if(tMapa[(*iPiso)][iRowT][iColT].arriba()) {
          cMapa[iRowC-1][iColC] = 'w';
        }
        else{
          cMapa[iRowC-1][iColC] = '.';
        }
        if(tMapa[(*iPiso)][iRowT][iColT].derecha()) {
          cMapa[iRowC][iColC+1] = 'w';
        }
        else{
          cMapa[iRowC][iColC+1] = '.';
        }
        if(tMapa[(*iPiso)][iRowT][iColT].abajo()) {
          cMapa[iRowC+1][iColC] = 'w';
        }
        else{
          cMapa[iRowC+1][iColC] = '.';
        }
        if(tMapa[(*iPiso)][iRowT][iColT].izquierda()) {
          cMapa[iRowC][iColC-1] = 'w';
        }
        else{
          cMapa[iRowC][iColC-1] = '.';
        }
        iColC+=2;
      }
      iRowC+=2;
    }
  }
  else{
      int iRowC = 1;
    for(int iRowT = 0; iRowT < 10; iRowT++) {
      int iColC = 1;
      for(int iColT = 0; iColT < 10; iColT++) {
        if(tBueno[(*iPiso)][iRowT][iColT].inicio()) {
          cMapa[iRowC][iColC] = 'i';
        }
        else if(tBueno[(*iPiso)][iRowT][iColT].visitado()) {
          cMapa[iRowC][iColC] = 'v';
        }
        else if(tBueno[(*iPiso)][iRowT][iColT].existe()) {
          cMapa[iRowC][iColC] = 'n';
        }
        else{
          cMapa[iRowC][iColC] = 'x';
        }
        if(tBueno[(*iPiso)][iRowT][iColT].arriba()) {
          cMapa[iRowC-1][iColC] = 'w';
        }
        else{
          cMapa[iRowC-1][iColC] = '.';
        }
        if(tBueno[(*iPiso)][iRowT][iColT].derecha()) {
          cMapa[iRowC][iColC+1] = 'w';
        }
        else{
          cMapa[iRowC][iColC+1] = '.';
        }
        if(tBueno[(*iPiso)][iRowT][iColT].abajo()) {
          cMapa[iRowC+1][iColC] = 'w';
        }
        else{
          cMapa[iRowC+1][iColC] = '.';
        }
        if(tBueno[(*iPiso)][iRowT][iColT].izquierda()) {
          cMapa[iRowC][iColC-1] = 'w';
        }
        else{
          cMapa[iRowC][iColC-1] = '.';
        }
        iColC+=2;
      }
      iRowC+=2;
    }
  }
  
  for(int i = 0; i < 21; i++) {
    for(int j = 0; j < 21; j++) {
      Serial.print(cMapa[i][j]);
      Serial.print(" ");
    }
    Serial.println(" ");
  }
  Serial.println(" ");
  delay(200);
}


/*
   void Movimiento::ErrorGradosVuelta(double &grados) {
        int iM;
        grados = real->getAngulo(angle);
        grados = (grados >= 360) ? 0 - fDeseado : grados - fDeseado;
        if(grados > 180) {
                iM = grados/180;
                grados = -( 180 - (grados - (iM + 180)));
        } else if(grados < -180) {
                grados *= -1;
                iM = grados/180;
                grados = ( 180 - (grados - (iM + 180)));
        }
   }
 */
