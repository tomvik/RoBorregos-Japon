/*
        TODO
                - Comunicacion 2 rasps -> nano -> mega
                - Victima visual pared no pared
 */

#include <Arduino.h>
#include <Movimiento.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <utility/Adafruit_MS_PWMServoDriver.h>
#include <PID_v1.h>

/////////// Dimensiones ///////////////////
const uint8_t kMapSize = 15;
const uint8_t kMapFloors = 4;

////////// Mapas para algoritmo /////////////
uint8_t iMapa[kMapSize][kMapSize];
char cMapa[kMapSize][kMapSize];
//char cMapaImprime[(kMapSize*2)+1][(kMapSize*2)+1];

////////////////////// Define constants ///////////////////////////
const uint8_t kToleranciaBumper = 6;
const double kPrecisionImu = 4.85;
const uint8_t kRampaLimit = 17;

const int kLimITerm = 100;
const double kP_Ambas_Pared = 0.5;
const double kI_Ambas_Pared = .1;
const double kD_Ambas_Pared = 2;

const double kP_Una_Pared = 0.7;
const double kI_Una_Pared = 0.035;
const double kD_Una_Pared = 2;

const int kEncoder30 = 2350;
const int kEncoder15 = kEncoder30 / 2;
const double kP_Vueltas = 1.111;
const int kDistanciaEnfrente = 45;
const int kDistanciaAtras = 55;
const int kDistanciaLejos = 75;
const int kMapearPared = 7;
const int kParedDeseadoIzq = 52; // 105 mm
const int kParedDeseadoDer = 52; // 105 mm

const uint8_t kVelocidadBaseMenor = 100;


//////////////////////Define pins and motors//////////////////////////
#define pin_Servo 10
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotorRightF = AFMS.getMotor(1);
Adafruit_DCMotor *myMotorRightB = AFMS.getMotor(4);
Adafruit_DCMotor *myMotorLeftF = AFMS.getMotor(3);
Adafruit_DCMotor *myMotorLeftB = AFMS.getMotor(2); //al revés
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
Movimiento::Movimiento(uint8_t iPowd, uint8_t iPowi, SensarRealidad *r, char *c, uint8_t *ic, uint8_t *ir, uint8_t *ip, Tile (*tM)[kMapSize][kMapSize], uint8_t *iPM) {
	//////////////////Inicializamos variables en 0////////////////////////////////
	eCount1 = eCount2 = cVictima = cParedes = iTerm = fSetPoint = iColor = resetIMU = iVisual = iKit = iCalorD = bBoton1 = 0;
  iCalor = 5;
	cuadrosSeguidos = -100;
	//////////////////////////Inicializamos el apuntador a los sensores, posición y LED//////////////////////
	real = r, iCol = ic, iRow = ir, iPiso = ip, cDir = c, tMapa = tM, iPisoMax = iPM;

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

void Movimiento::velocidad(uint8_t powIzq, uint8_t powDer) {
	if(powIzq < 0) powIzq = 0;
	if(powDer < 0) powDer = 0;
	if(powIzq > 255) powIzq = 255;
	if(powDer > 255) powDer = 255;

	myMotorLeftF->setSpeed(powIzq);
	myMotorLeftB->setSpeed(powIzq);

	myMotorRightF->setSpeed(powDer);
	myMotorRightB->setSpeed(powDer);

	if(bBoton1) {
		//TODO
    stop();
		delay(10000);
	}
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

void Movimiento::alinear() {
	// Caso 0 = normal
	// Caso 1 = vuelta

	if(!(tMapa[*iPiso][*iRow][*iCol].bumper())) {
		double posInicial, limInf, limSup;

		limInf = fSetPoint - kPrecisionImu;
		if(limInf < 0) limInf += 360;

		limSup = fSetPoint + kPrecisionImu;
		if(limSup >= 360.0) limSup -= 360;

		real->getAngulo(posInicial);

		if(limSup > limInf) {
			if(limSup > 270 && posInicial < 90)
				posInicial += 360;
			if(posInicial < limInf)
				vueltaDer(false);
			else if(posInicial > limSup)
				vueltaIzq(false);
		} else {
			if(posInicial > 270 && limInf < 90)
				posInicial -= 360;
			if(posInicial < limInf && posInicial > 180)
				vueltaDer(false);
			else if(posInicial > limSup && posInicial < 180)
				vueltaIzq(false);
		}

		uint8_t potIzq, potDer, acomodar;
		int deseado, deseadoAlt, margen = 5;
		potIzq = potDer = kVelocidadBaseMenor;
		int distanciaAtras = real->getDistanciaAtras();
		int distanciaEnfrente = real->getDistanciaEnfrente();
		unsigned long inicio = millis();

		if(cuadrosSeguidos > 0 && distanciaAtras > 0)
			acomodar = 1;
		else if(distanciaEnfrente >= 0 && distanciaAtras >= 0) {
			if(distanciaEnfrente < distanciaAtras) {
				acomodar = 0;
			} else {
				acomodar = 1;
			}
		} else if (distanciaEnfrente >= 0)
			acomodar = 0;
		else if (distanciaAtras >= 0)
			acomodar = 1;

		switch (acomodar) {
		case 0:
			real->escribirLCD("Alinear enfrente");
			if(distanciaEnfrente < 250) {
				deseado = kDistanciaEnfrente;
			} else {
				deseado = (distanciaEnfrente / 300) * 300 + kDistanciaLejos;
				deseadoAlt = ((distanciaEnfrente / 300) + 1) * 300 + kDistanciaLejos;
				if(abs(distanciaEnfrente - deseado) > abs(distanciaEnfrente - deseadoAlt) * 0.90)
					deseado = deseadoAlt;
				margen *= 20;
			}
			while ((distanciaEnfrente < deseado - margen || distanciaEnfrente > deseado + margen) && (inicio + 1000 > millis())) {
				distanciaEnfrente = real->getDistanciaEnfrente();
				real->escribirLCDabajo("     " + String(distanciaEnfrente));
				velocidad(potIzq, potDer);
				if(distanciaEnfrente < deseado) //Muy cerca
					back();
				else if (distanciaEnfrente == deseado)
					break;
				else //Muy lejos
					front();
				velocidad(potIzq, potDer);
			}
			break;
		case 1:
			real->escribirLCD("Alinear atras");
			if(distanciaAtras < 250) {
				deseado = kDistanciaAtras;
				if (cuadrosSeguidos > 0)
					deseado = cuadrosSeguidos * 300 + kDistanciaLejos;
			} else {
				if (cuadrosSeguidos > 0) {
					deseado = cuadrosSeguidos * 300 + kDistanciaLejos;
				} else {
					deseado = (distanciaAtras / 300) * 300 + kDistanciaLejos;
					deseadoAlt = ((distanciaAtras / 300) + 1) * 300 + kDistanciaLejos;
					if(abs(distanciaAtras - deseado) > abs(distanciaAtras - deseadoAlt) * 0.90)
						deseado = deseadoAlt;
				}
				margen *= 20;
			}
			while ((distanciaAtras < deseado - margen || distanciaAtras > deseado + margen) && (inicio + 1000 > millis())) {
				distanciaAtras = real->getDistanciaAtras();
				real->escribirLCDabajo("     " + String(distanciaAtras));
				velocidad(potIzq, potDer);
				if(distanciaAtras < deseado) //Muy cerca
					front();
				else if (distanciaAtras == deseado)
					break;
				else //Muy lejos
					back();
			}
			break;
		}
		stop();
	}
}

void Movimiento::corregirIMU() {
	if(!(real->caminoAtras()) && resetIMU > 10) {
		real->escribirLCD("corregir IMU");
		double angle, fRef = 0.0;
		resetIMU = 0;
		unsigned long inicio = millis();

		back();
		velocidad(kVelocidadBaseMenor, kVelocidadBaseMenor);
		while(real->getDistanciaAtras() > 20 && inicio + 1000 > millis()) {
			real->escribirLCD(String(real->getDistanciaAtras()));
		}
		delay(600);
		stop();
		delay(100);

		for(int i = 0; i < 5; i++) {
			real->getAngulo(angle);
			fRef += angle;
		}
		fRef /= 5;
		fSetPoint = fRef;

		inicio = millis();
		front();
		velocidad(kVelocidadBaseMenor, kVelocidadBaseMenor);
		while(real->getDistanciaAtras() < 20 && inicio + 1000 > millis()) {
			real->escribirLCD(String(real->getDistanciaAtras()));
		}
		stop();
	}
}


void Movimiento::vueltaIzq(bool caso) {
	real->escribirLCD("Vuelta IZQ");
	resetIMU += 2;
	iTerm = 0;
	cuadrosSeguidos = -100;
	int potIzq, potDer, dif;
	double posInicial, limInf, limSup;
	unsigned long inicio = millis();

	if(!real->getAngulo(posInicial)) {
		fSetPoint = posInicial;
		real->apantallanteLCD("   correccion", "   IMU  chafo");
	}

	if(caso) {
		fSetPoint -= 90;
		if(fSetPoint < 0) fSetPoint += 360;
	}

	limInf = fSetPoint - kPrecisionImu;
	if(limInf < 0) limInf += 360;

	limSup = fSetPoint + kPrecisionImu;
	if(limSup >= 360.0) limSup -= 360;

	potIzq = potDer = kVelocidadBaseMenor;
	velocidad(200, 200);

	if(limSup > limInf) {
		while(posInicial < limInf || posInicial > limSup) {
			real->getAngulo(posInicial);
			real->escribirLCDabajo("     " + String(posInicial));
			if(posInicial < 90 && fSetPoint > 270)
				dif = abs(fSetPoint - 360 - posInicial);
			else
				dif = abs(fSetPoint - posInicial);
			if(dif > 90) dif = 90;
			dif *= kP_Vueltas;

			if(millis() >= inicio + 16000) {
				stop();
				back();
				delay(100);
				inicio = millis();
				stop();
			} else if(millis() >= inicio + 12000) {
				velocidad(255, 255);
			} else if(millis() >= inicio + 8000) {
				velocidad(200, 200);
			} else if (millis() >= inicio + 4000) {
				velocidad(175, 175);
			} else velocidad(potIzq + dif, potDer + dif);

			checarVictima(false);
			left();
		}
	} else {
		while(posInicial < limInf && posInicial > limSup) {
			real->getAngulo(posInicial);
			real->escribirLCDabajo("     " + String(posInicial));
			if(posInicial < 90 && fSetPoint > 270)
				dif = abs(fSetPoint - 360 - posInicial);
			else
				dif = abs(fSetPoint - posInicial);
			if(dif > 90) dif = 90;
			dif *= kP_Vueltas;

			if(millis() >= inicio + 16000) {
				stop();
				back();
				delay(100);
				inicio = millis();
				stop();
			} else if(millis() >= inicio + 12000) {
				velocidad(255, 255);
			} else if(millis() >= inicio + 8000) {
				velocidad(200, 200);
			} else if (millis() >= inicio + 4000) {
				velocidad(175, 175);
			} else velocidad(potIzq + dif, potDer + dif);
			checarVictima(false);
			left();
		}
	}
	stop();
}

void Movimiento::vueltaDer(bool caso) {
	real->escribirLCD("Vuelta DER");
	resetIMU += 2;
	iTerm = 0;
	cuadrosSeguidos = -100;
	int potIzq, potDer, dif;
	double posInicial, limInf, limSup;
	unsigned long inicio = millis();

	if(!real->getAngulo(posInicial)) {
		fSetPoint = posInicial;
		real->apantallanteLCD("   correccion", "   IMU  chafo");
	}

	if(caso) {
		fSetPoint += 90;
		if(fSetPoint >= 360) fSetPoint -= 360;
	}

	limInf = fSetPoint - kPrecisionImu;
	if(limInf < 0) limInf += 360;

	limSup = fSetPoint + kPrecisionImu;
	if(limSup >= 360.0) limSup -= 360;

	potIzq = potDer = kVelocidadBaseMenor;
	velocidad(200, 200);

	if(limSup > limInf) {
		while(posInicial < limInf || posInicial > limSup) {
			real->getAngulo(posInicial);
			real->escribirLCDabajo("     " + String(posInicial));
			if(posInicial > 270 && fSetPoint < 90)
				dif = abs(fSetPoint + 360 - posInicial);
			else
				dif = abs(fSetPoint - posInicial);
			if(dif > 90) dif = 90;
			dif *= kP_Vueltas;

			if(millis() >= inicio + 16000) {
				stop();
				back();
				delay(100);
				inicio = millis();
				stop();
			} else if(millis() >= inicio + 12000) {
				velocidad(255, 255);
			} else if(millis() >= inicio + 8000) {
				velocidad(200, 200);
			} else if (millis() >= inicio + 4000) {
				velocidad(175, 175);
			} else velocidad(potIzq + dif, potDer + dif);
			checarVictima(false);
			right();
		}
	} else {
		while(posInicial < limInf && posInicial > limSup) {
			real->getAngulo(posInicial);
			real->escribirLCDabajo("     " + String(posInicial));
			if(posInicial > 270 && fSetPoint < 90)
				dif = abs(fSetPoint + 360 - posInicial);
			else
				dif = abs(fSetPoint - posInicial);
			if(dif > 90) dif = 90;
			dif *= kP_Vueltas;

			if(millis() >= inicio + 16000) {
				stop();
				back();
				delay(100);
				inicio = millis();
				stop();
			} else if(millis() >= inicio + 12000) {
				velocidad(255, 255);
			} else if(millis() >= inicio + 8000) {
				velocidad(200, 200);
			} else if (millis() >= inicio + 4000) {
				velocidad(175, 175);
			} else velocidad(potIzq + dif, potDer + dif);
			checarVictima(false);
			right();
		}
	}
	stop();
}

void Movimiento::potenciasDerecho(uint8_t &potenciaIzq, uint8_t &potenciaDer, uint8_t caso) {
	double angle;

	if(!real->getAngulo(angle)) {
		fSetPoint = angle;
	} else if(!(tMapa[*iPiso][*iRow][*iCol].bumper())) {
		if(angle < 20 && fSetPoint > 340) {
			if(angle - fSetPoint + 360 > 20) {
				acomodaChoque(2);
			}
			else if(angle - fSetPoint + 360 < -20) {
				acomodaChoque(1);
			}
		} else if(fSetPoint < 20 && angle > 340) {
			if(angle - fSetPoint - 360 > 20) {
				acomodaChoque(2);
			}
			else if(angle - fSetPoint - 360 < -20) {
				acomodaChoque(1);
			}
		} else if(angle - fSetPoint > 20) {
			acomodaChoque(2);
		} else if(angle - fSetPoint < -20) {
			acomodaChoque(1);
		}
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
	if(distanciaIzq < 125 && distanciaDer < 125 && distanciaIzq > 0 && distanciaDer > 0) {
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

	} else if(distanciaIzq < 125 && distanciaIzq > 0) {
		contadorIzq++;
		iError = kParedDeseadoIzq - distanciaIzq;
		if(caso == 1 || caso == 2) iError -= 20;
		if(-5 < iError && iError < 5) iError = 0;

		// debe ser negativo, creo
		iTerm -= iError;
		if(iTerm > kLimITerm) iTerm = kLimITerm;
		else if(iTerm < -kLimITerm) iTerm = -kLimITerm;

		iParaD = lastInput - iError;
		if(-4 < iParaD && iParaD < 4) iParaD = 0;

		outIzqPARED = iError * kP_Una_Pared - iTerm * kI_Una_Pared - iParaD * kD_Una_Pared;
		outDerPARED = -iError * kP_Una_Pared + iTerm * kI_Una_Pared + iParaD * kD_Una_Pared;
	} else if(distanciaDer < 125 && distanciaDer > 0) {
		contadorDer++;
		iError = kParedDeseadoDer - distanciaDer;
		if(caso == 1 || caso == 2) iError -= 20;
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

	if(caso == 2) {
		potenciaIzq *= 1.129;
		potenciaDer *= 1.129;
	} else if(caso == 3) {
		potenciaIzq *= 0.89;
		potenciaDer *= 0.89;
	}

	if(potenciaIzq < kVelocidadBaseMenor) potenciaIzq = kVelocidadBaseMenor;
	if(potenciaDer < kVelocidadBaseMenor) potenciaDer = kVelocidadBaseMenor;
	lastInput = iError;

	real->escribirLCD(String(distanciaDer) + "     " + String(distanciaIzq));
	// real->escribirLCD(String(outDerIMU) + "     " + String(outIzqIMU), String(outDerPARED) + "     " + String(outIzqPARED));
}

void Movimiento::pasaRampa() {
	front();
	resetIMU += 10;
	while(Serial2.available())
		cVictima = (char)Serial2.read();
	real->apantallanteLCD("Rampa");
	uint8_t iPowII, iPowDD;
	// bajando
	while(real->sensarRampa() < -kRampaLimit) {
		checarVictima();
		potenciasDerecho(iPowII, iPowDD, 1);
		front();
		velocidad(iPowII, iPowDD);
	}
	// subiendo
	while(real->sensarRampa() > kRampaLimit) {
		checarVictima();
		potenciasDerecho(iPowII, iPowDD, 2);
		velocidad(iPowII, iPowDD);
		front();
	}
	cParedes = 0;
	velocidad(kVelocidadBaseMenor, kVelocidadBaseMenor);
	delay(1200);
	stop();
	alinear();
}

void Movimiento::dejarKit(uint8_t iCase) {
  iKit++;
	tMapa[*iPiso][*iRow][*iCol].victima(true);
	if(real->sensarRampa() < kRampaLimit && real->sensarRampa() > -kRampaLimit) {
		switch(iCase) {
		case 1:
			real->apantallanteLCD("VICTIMA con KIT", "derecha");
			myservo.write(70);
			delay(400);
			myservo.write(110);
			delay(400);
			myservo.write(0);
			break;
		case 2:
			real->apantallanteLCD("VICTIMA con KIT", "izquierda");
			myservo.write(110);
			delay(400);
			myservo.write(70);
			delay(400);
			myservo.write(180);
			break;
		}
		delay(1200);
		myservo.write(90);
	}
	while(Serial2.available())
		cVictima = (char)Serial2.read();
}


void Movimiento::retroceder() {
	real->escribirLCD("CUADRO NEGRO");
	uint8_t iPowII = kVelocidadBaseMenor, iPowDD = kVelocidadBaseMenor;
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
	cuadrosSeguidos = -100;
	back();
	while(eCount1 + eCount2 < kEncoder30) {
		potenciasDerecho(iPowDD, iPowII, 3);
		velocidad(iPowII, iPowDD);
	}
	stop();
	alinear();
}

void Movimiento::acomodaChoque(uint8_t switchCase) {
	double basura;
	real->escribirLCD("     LIMIT");
	int encoderTemp1 = eCount1;
	int encoderTemp2 = eCount2;
	stop();
	unsigned long inicio = millis();
	back();
	switch(switchCase) {
	case 1:
		velocidad(iPowI, 0);
		while(millis() - inicio < 375)
			real->getAngulo(basura);

		inicio = millis();
		velocidad(kVelocidadBaseMenor, kVelocidadBaseMenor);
		while(millis() - inicio < 250)
			real->getAngulo(basura);
		vueltaDer(false);
		break;

	case 2:
		velocidad(0, iPowD);
		while(millis() - inicio < 375)
			real->getAngulo(basura);

		inicio = millis();
		velocidad(kVelocidadBaseMenor, kVelocidadBaseMenor);
		while(millis() - inicio < 250)
			real->getAngulo(basura);
		vueltaIzq(false);
		break;
	}
	stop();
	if(encoderTemp1 < 450)
		encoderTemp1 = 450;
	if(encoderTemp2 < 450)
		encoderTemp2 = 450;
	eCount1 = encoderTemp1 - 450;
	eCount2 = encoderTemp2 - 450;
	front();
}

void Movimiento::avanzar() {
	resetIMU++;
	cParedes = cVictima = eCount1 = eCount2 = 0;
	double bumperMin = 0.0, bumperMax = 0.0;
	uint8_t iPowII, iPowDD, switchCase;
	int distanciaEnfrente = 0;

	// alinear();
	corregirIMU();
	checarVictima();
	velocidad(iPowI, iPowD);

	if(!(real->caminoAtras())) {
		cuadrosSeguidos = 0;
	}

	front();
	distanciaEnfrente = real->getDistanciaEnfrente();
	while(eCount1 + eCount2 < kEncoder15 && (distanciaEnfrente > kDistanciaEnfrente || distanciaEnfrente == -1)) {
		checarVictima();
		front();
		potenciasDerecho(iPowII, iPowDD);
		velocidad(iPowII, iPowDD);

		switchCase = real->switches();
		if(switchCase > 0 && real->caminoEnfrente()) {
			acomodaChoque(switchCase);
		}

		// bumper
		if(!(tMapa[*iPiso][*iRow][*iCol].bumper())) {
			bumperMin = real->sensarRampa() < bumperMin ? real->sensarRampa() : bumperMin;
			bumperMax = real->sensarRampa() > bumperMax ? real->sensarRampa() : bumperMax;
			if(bumperMax - bumperMin >= kToleranciaBumper)
				tMapa[*iPiso][*iRow][*iCol].bumper(true);
		}
		distanciaEnfrente = real->getDistanciaEnfrente();
		real->escribirLCDabajo("     " + String(distanciaEnfrente));
	}

		cuadrosSeguidos++;
		switch(*cDir) {
		case 'n': (*iRow)--; break;
		case 'e': (*iCol)++; break;
		case 's': (*iRow)++; break;
		case 'w': (*iCol)--; break;
		}

		uint8_t contadorNegro;
		contadorIzq = contadorDer = bumperMin = bumperMax = contadorNegro = iColor = 0;

		while(eCount1 + eCount2 < kEncoder30 && (distanciaEnfrente > kDistanciaEnfrente || distanciaEnfrente == -1)) {
			checarVictima();
			front();
			potenciasDerecho(iPowII, iPowDD);
			velocidad(iPowII, iPowDD);

			switchCase = real->switches();
			if(switchCase > 0 && real->caminoEnfrente()) {
				acomodaChoque(switchCase);
			}

			// bumper
			if(!(tMapa[*iPiso][*iRow][*iCol].bumper())) {
				bumperMin = real->sensarRampa() < bumperMin ? real->sensarRampa() : bumperMin;
				bumperMax = real->sensarRampa() > bumperMax ? real->sensarRampa() : bumperMax;
				if(bumperMax - bumperMin >= kToleranciaBumper)
					tMapa[*iPiso][*iRow][*iCol].bumper(true);
			}

			if(cVictima & 0b00001000)
				contadorNegro++; // NEGRO

			distanciaEnfrente = real->getDistanciaEnfrente();
			real->escribirLCDabajo("     " + String(distanciaEnfrente));
		}
		if(!(tMapa[*iPiso][*iRow][*iCol].bumper())) {
			if(contadorIzq > kMapearPared)
				cParedes |= 0b00000100;
			if(distanciaEnfrente < 200 && distanciaEnfrente >= 0)
				cParedes |= 0b00000010;
			if(contadorDer > kMapearPared)
				cParedes |= 0b00000001;
		} else {
			if(!(abs(real->sensarRampa()) < abs(kRampaLimit))) {
				// front();
			} else {
				real->apantallanteLCD("BUUUUMPER");
				if(!(real->caminoIzquierda()))
					cParedes |= 0b00000100;
				if(!(real->caminoEnfrente()))
					cParedes |= 0b00000010;
				if(!(real->caminoDerecha()))
					cParedes |= 0b00000001;
			}
		}
    //TODO a mapear
		if(contadorNegro > kMapearPared)
			iColor = 1; // NEGRO

	if(iColor != 1  && abs(real->sensarRampa()) < abs(kRampaLimit)) {
		alinear();
	}
}

//Aquí obviamente hay que poner las cosas de moverse con los motores, hasta ahorita es solamente modificar mapa
void Movimiento::derecha() {                                                                                                              //Modificarse en realidad
	real->escribirLCD("DER");
	checarVictima();
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
	alinear();
}

//Aquí obviamente hay que poner las cosas de moverse con los motores, hasta ahorita es solamente modificar mapa
void Movimiento::izquierda() {
	real->escribirLCD("IZQ");
	checarVictima();
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
	alinear();
}

//Recibe el string de a dónde moverse y ejecuta las acciones llamando a las funciones de arriba
void Movimiento::hacerInstrucciones(String sMov) {
	real->escribirLCD("PATH");
	// delay(1000);
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
	}
}

//Busca dónde está el no visitado usando funciones de aquí y de sensar mapa.
//Aquí imprime el mapa de int
//La funcion comparaMapa se podría modificar para depender si quiero ir al inicio o a un cuadro no visitado
bool Movimiento::goToVisitado(char cD) {
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
	real->escribirLCD("   LLENA", "   MAPA");
	mapa.tiempoI(millis());
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
	real->escribirLCD("   COMPARA", "   HACER");
	if(mapa.comparaMapa(iMapa, tMapa, cD, *iCol, *iRow, iNCol, iNRow, *iPiso)) { //Hace las instrucciones que recibe de la función en forma de string
		hacerInstrucciones(mapa.getInstrucciones(iMapa, cMapa, tMapa, iNCol, iNRow, *iPiso));
		//TODO comentarlo?
		*iCol = iNCol, *iRow = iNRow;
		return true;
	}
	return false;
}


//La hice bool para que de una forma estuviera como condición de un loop, pero aún no se me ocurre cómo
bool Movimiento::decidir() {
	bLack = false;
	if(tMapa[*iPiso][*iRow][*iCol].cuadroNegro()) {
		retroceder();
	}
	checarVictima();
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
		return true;
	}
	//Si no hay pared enfrente Y no está visitado, muevete hacia allá
	else if(mapa.sensa_Pared(tMapa, *cDir, *iCol, *iRow, *iPiso, 'u') && mapa.sensaVisitado(tMapa, *cDir, *iCol, *iRow, *iPiso, 'u')) {
		avanzar();
		stop();
		return true;
	}
	//Si no hay pared a la izquierda y no está visitado, muevete hacia allá
	else if(mapa.sensa_Pared(tMapa, *cDir, *iCol, *iRow, *iPiso, 'l') && mapa.sensaVisitado(tMapa, *cDir, *iCol, *iRow, *iPiso, 'l')) {
		izquierda();
		avanzar();
		stop();
		return true;
	}
	//Si no hay pared atrás y no está visitado, muevete hacia allá
	//Sólo entraría a este caso en el primer movimiento de la ronda (si queda mirando a un deadend)
	else if(mapa.sensa_Pared(tMapa, *cDir, *iCol, *iRow, *iPiso, 'd') && mapa.sensaVisitado(tMapa, *cDir, *iCol, *iRow, *iPiso, 'd')) {
		derecha();
		derecha();
		avanzar();
		stop();
		return true;
	}
	//Aquí es cuando entra a lo recursivo
	else{
		//Llama la función recursiva
		return goToVisitado( 'n');
	}
}
void Movimiento::checarVictima(bool caso) {
  //Lee Serial
	cVictima = 0;
	Serial2.print("M");
	while(!Serial2.available());
	while(Serial2.available()) {
		cVictima = (char)Serial2.read();
	}
  //Si está dando vuelta
  if(!caso){
    //Si es posible victima de calor
    if(!(cVictima & 0b00100000) && cVictima&0b00000010 && !tMapa[*iPiso][*iRow][*iCol].victima()){
      //Se detiene
      uint8_t iCase = (cVictima&0b00000001) ? 1 : 2;
      uint16_t encoderTemp1 = eCount1;
      uint16_t encoderTemp2 = eCount2;
      stop();
      iCalorD++;
      iCalor = (iCalorD > 4) ? iCalorD : iCalor;
      //Si se pasó de kits
      if(iKit > 12){
        tMapa[*iPiso][*iRow][*iCol].victima(true);
        delay(5000);
      }
      else{
        dejarKit(iCase);
      }
      //Encoder normal
      eCount1 = encoderTemp1;
      eCount2 = encoderTemp2;
    }
  }
  //Si está andando normal o rampa
  else{
  	//Valida que puede haber una victima
  	if(cVictima&0b00000010 && !tMapa[*iPiso][*iRow][*iCol].victima() && ( (cVictima&0b00000001 && !(real->caminoDerecha()) )  || (cVictima&0b00000100 && !(real->caminoIzquierda()) ) ) ) {
  		real->escribirLCD("VICTIMAAAAA");
  		//Se detiene
  		uint8_t iCase = (cVictima&0b00000001) ? 1 : 2;
  		uint16_t encoderTemp1 = eCount1;
  		uint16_t encoderTemp2 = eCount2;
  		stop();
      //Visual
      if((iVisual < iCalor) && (cVictima & 0b00100000)){
        //Visual Derecha
        if(iCase == 1){
          real->escribirLCD("Derecha", "VISUAL");
          Serial2.print("I");
        }
        //Visual Izquierda
        else{
          real->escribirLCD("Izquierda", "VISUAL");
          Serial2.print("R");
        }
        //Esperar a ver cuál es
        real->escribirLCD("Cual", "Cual");
        unsigned long start = millis();
				char x;
        while(Serial2.available() && start + 400 >= millis()) {
          x = (char)Serial2.read();
        }
				x++;
        while(!Serial2.available() && start + 400 >= millis()) {
          delay(1);
        }
        while(!(cVictima == 0b10000000) && !(cVictima == 0b01000000) && !(cVictima == 0b00100000) && !(cVictima == 0b00010000) && start + 400 >= millis()){
          while(Serial2.available()) {
            cVictima = (char)Serial2.read();
          }
        }
        real->escribirLCD("SALI");
        //delay(40000);
        //Si no se pasó de tiempo
        if(start + 400 >= millis()){
          //H
          if(cVictima == 0b10000000) {
            iVisual++;
            real->escribirLCD("VICTIMA", "HHHHHHHHH");
            delay(1000);
            //Si se pasó de kits
            if(iKit > 12){
              tMapa[*iPiso][*iRow][*iCol].victima(true);
              delay(5000);
            }
            else{
              dejarKit(iCase);
              if(iKit <= 12)
                dejarKit(iCase);
            }
          }
          //S u Letra
          else if(cVictima == 0b01000000) {
            iVisual++;
            real->escribirLCD("VICTIMA", "SSSSSSSSS");
            delay(1000);
            //Si se pasó de kits
            if(iKit > 12){
              tMapa[*iPiso][*iRow][*iCol].victima(true);
              delay(5000);
            }
            else{
              dejarKit(iCase);
            }
          }
          //U
          else if(cVictima == 0b00100000) {
            iVisual++;
            real->escribirLCD("VICTIMA", "UUUUUUUUU");
            tMapa[*iPiso][*iRow][*iCol].victima(true);
            delay(5000);
          }
          //Nada
          else if(cVictima == 0b00010000){
            real->escribirLCD("NADA");
          }
        }
        //Time limit
        else{
          real->escribirLCD("SSSSLLLLOOW");
        }
        //Derecha Buscar
        if(iCase == 1){
          Serial2.print("B");
          Serial2.print("B");
        }
        //Izquierda Buscar
        else{
          Serial2.print("E");
          Serial2.print("E");
        }
      }
      //Calor
  		else if(!(cVictima & 0b00100000) && (cVictima & 0b00000010)){
        iCalorD++;
        iCalor = (iCalorD > 4) ? iCalorD : iCalor;
        real->apantallanteLCD("NORMAL");
        //Si se pasó de kits
  			if(iKit > 12){
          tMapa[*iPiso][*iRow][*iCol].victima(true);
          delay(5000);
        }
        else{
          dejarKit(iCase);
        }
  		}
      //Encoder normal
  		eCount1 = encoderTemp1;
  		eCount2 = encoderTemp2;
      while(Serial2.available())
        Serial2.read();
  	}
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

uint8_t Movimiento::getColor(){
  return iColor;
}

//////////////Funcion de impresión de mapa//////////////
/*void Movimiento::muestra(){
	for(int j = 0; j < (kMapSize*2)+1; j++) {
		for(int z = 0; z < (kMapSize*2)+1; z++) {
			cMapaImprime[j][z] = 'x';
		}
	}
	int iRowC = 1;
	for(int iRowT = 0; iRowT < kMapSize; iRowT++) {
		int iColC = 1;
		for(int iColT = 0; iColT < kMapSize; iColT++) {
			if(tMapa[(*iPiso)][iRowT][iColT].inicio()) {
				cMapaImprime[iRowC][iColC] = 'i';
			}
			else if(tMapa[(*iPiso)][iRowT][iColT].visitado()) {
				cMapaImprime[iRowC][iColC] = 'v';
			}
			else if(tMapa[(*iPiso)][iRowT][iColT].existe()) {
				cMapaImprime[iRowC][iColC] = 'n';
			}
			else{
				cMapaImprime[iRowC][iColC] = 'x';
			}
			if(tMapa[(*iPiso)][iRowT][iColT].arriba()) {
				cMapaImprime[iRowC-1][iColC] = 'w';
			}
			else{
				cMapaImprime[iRowC-1][iColC] = '.';
			}
			if(tMapa[(*iPiso)][iRowT][iColT].derecha()) {
				cMapaImprime[iRowC][iColC+1] = 'w';
			}
			else{
				cMapaImprime[iRowC][iColC+1] = '.';
			}
			if(tMapa[(*iPiso)][iRowT][iColT].abajo()) {
				cMapaImprime[iRowC+1][iColC] = 'w';
			}
			else{
				cMapaImprime[iRowC+1][iColC] = '.';
			}
			if(tMapa[(*iPiso)][iRowT][iColT].izquierda()) {
				cMapaImprime[iRowC][iColC-1] = 'w';
			}
			else{
				cMapaImprime[iRowC][iColC-1] = '.';
			}
			iColC+=2;
		}
		iRowC+=2;
	}

	for(int i = 0; i < (kMapSize * 2) + 1; i++) {
		for(int j = 0; j < (kMapSize * 2) + 1; j++) {
			Serial.print(cMapaImprime[i][j]);
			Serial.print(" ");
		}
		Serial.println(" ");
	}
	Serial.println(" ");
}*/


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
