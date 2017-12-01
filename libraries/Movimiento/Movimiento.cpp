/* Code made by the RoBorregos team in 2017 for the RoboCup JR. Rescue Maze category.
 * Tomás Lugo, Sebastián Esquer, Ernesto Cervantez, and Alexis Virgen.
 * "El Mariachi" Achieved a third place on the international RoboCup.
 *
 * This class has all the functions related to movement. Oh boi, this is the funniest one.
 * For more info, check the Movimiento.h file
*/
#include <Arduino.h>
#include <Movimiento.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <utility/Adafruit_MS_PWMServoDriver.h>
#include <PID_v1.h>

/////////// Dimensions ///////////////////
const uint8_t kMapSize = 15;
const uint8_t kMapFloors = 4;

////////// Maps for the path finding and test /////////////
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

const int kEncoder30 = 2330;
const int kEncoder15 = kEncoder30 / 2;
const double kP_Vueltas = 1.111;
const int kDistanciaEnfrente = 47;
const int kDistanciaAtras = 55;
const int kDistanciaLejos = 75;
const int kMapearPared = 4;
const int kParedDeseadoIzq = 52; // 105 mm
const int kParedDeseadoDer = 52; // 105 mm

const uint8_t kVelocidadBaseMenor = 100;

uint8_t iAhora, iAntes, contadorNegro;
bool der[18];

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
   n = north
   e = east
   s = south
   w = west
   sMov
   d = right
   e = front
   i = left
   a = behind
 */
Movimiento::Movimiento(uint8_t iPowd, uint8_t iPowi, SensarRealidad *r, char *c, uint8_t *ic, uint8_t *ir, uint8_t *ip, Tile (*tM)[kMapSize][kMapSize], uint8_t *iPM) {
	/// Initialize variables
	iAhora = iAntes = eCount1 = eCount2 = cVictima = cParedes = iTerm = fSetPoint = iColor = resetIMU = iVisual = iKit = iCalorD = bBoton1 = 0;
	iCalor = 5;
	cuadrosSeguidos = -100;
	real = r, iCol = ic, iRow = ir, iPiso = ip, cDir = c, tMapa = tM, iPisoMax = iPM;

	/// Initialize motors and servo
	iPowI = iPowi;
	iPowD = iPowd;
	AFMS.begin();
	velocidad(iPowI, iPowD);
	myservo.attach(pin_Servo);
	myservo.write(90);
	/// Initialize PID for IMU
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

	//This was to make a lack of progress and not lose all the data, never had time to implement it
	//Correctly, instead, because of some electrical issues, we had to stop the motors and the initialize it.
	if(bBoton1) {
		stop();
		delay(10000);
	}
}


void Movimiento::stop() {

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
	// Caso 1 = turn

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
				if(distanciaEnfrente == -1) {
					break;
				}
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
				if(distanciaEnfrente == -1) {
					break;
				}
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
	//Ponemos si hay pared o no y actualizamos el contador y true es pared
	der[iAhora] = (distanciaDer < 90 && distanciaDer > 0);
	iAhora = iAhora >= 17 ? 0 : iAhora + 1;

	if(distanciaIzq < 120 && distanciaDer < 120 && distanciaIzq > 0 && distanciaDer > 0) {
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

	} else if(distanciaIzq < 120 && distanciaIzq > 0) {
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
	} else if(distanciaDer < 120 && distanciaDer > 0) {
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

void Movimiento::vueltaIzq(bool caso) {
	real->escribirLCD("Vuelta IZQ");
	resetIMU += 2;
	iTerm = 0;
	cuadrosSeguidos = -100;
	bool atras = true;
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

			if(millis() >= inicio + 4000) {
				stop();
				if(atras) {
					front();
					atras = false;
				}
				else {
					back();
					atras = true;
				}
				delay(275);
				inicio = millis();
				stop();
			} else if(millis() >= inicio + 3000) {
				velocidad(255, 255);
			} else if (millis() >= inicio + 2700) {
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

			if(millis() >= inicio + 4000) {
				stop();
				if(atras) {
					front();
					atras = false;
				}
				else {
					back();
					atras = true;
				}
				delay(275);
				inicio = millis();
				stop();
			} else if(millis() >= inicio + 3000) {
				velocidad(255, 255);
			} else if (millis() >= inicio + 2700) {
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
	bool atras = true;
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

			if(millis() >= inicio + 4000) {
				stop();
				if(atras) {
					front();
					atras = false;
				}
				else {
					back();
					atras = true;
				}
				delay(275);
				inicio = millis();
				stop();
			} else if(millis() >= inicio + 3000) {
				velocidad(255, 255);
			} else if (millis() >= inicio + 2700) {
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

			if(millis() >= inicio + 4000) {
				stop();
				if(atras) {
					front();
					atras = false;
				}
				else {
					back();
					atras = true;
				}
				delay(275);
				inicio = millis();
				stop();
			} else if(millis() >= inicio + 3000) {
				velocidad(255, 255);
			} else if (millis() >= inicio + 2700) {
				velocidad(175, 175);
			} else velocidad(potIzq + dif, potDer + dif);
			checarVictima(false);
			right();
		}
	}
	stop();
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
			contadorNegro++;         // NEGRO

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
		iColor = 1;         // NEGRO

	if(iColor != 1  && abs(real->sensarRampa()) < abs(kRampaLimit)) {
		alinear();
	}
}

void Movimiento::derecha() {
	//real->escribirLCD("DER");
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

void Movimiento::izquierda() {
	//real->escribirLCD("IZQ");
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
		real->escribirLCD(String(iPowII), String(iPowDD));
	}
	stop();
	alinear();
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
	delay(1500);
	stop();
	alinear();
}

void Movimiento::dejarKit(uint8_t iCase) {
	iKit++;
	tMapa[*iPiso][*iRow][*iCol].victima(true);
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
		delay(1600);
		myservo.write(90);
	while(Serial2.available())
		cVictima = (char)Serial2.read();
}

void Movimiento::noKit(uint8_t iCase) {
	real->apantallanteLCD("VICTIMA sin KIT", String(iCase));
	tMapa[*iPiso][*iRow][*iCol].victima(true);
	delay(3500);
	real->apantallanteLCD("There should be", "more kits allowed");
	delay(1500);
}

void Movimiento::checarVictima(bool caso) {
	//Lee Serial
	cVictima = 0;
	Serial2.print("M");
	while(!Serial2.available() && !(real->getMalo())) ;
	while(Serial2.available()) {
		cVictima = (char)Serial2.read();
	}
	//Si está dando vuelta
	if(!caso) {
		//Si es posible victima de calor
		if(!(cVictima & 0b00100000) && cVictima&0b00000010 && !tMapa[*iPiso][*iRow][*iCol].victima()) {
			//Se detiene
			uint8_t iCase = (cVictima&0b00000001) ? 1 : 2;
			uint16_t encoderTemp1 = eCount1;
			uint16_t encoderTemp2 = eCount2;
			stop();
			iCalorD++;
			iCalor = (iCalorD > 4) ? iCalorD : iCalor;
			//Si se pasó de kits
			if(iKit > 12) {
				noKit(iCase);
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
			// 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
			iAntes = iAhora < 17 ? iAhora + 1 : iAhora - 17;
			if(der[iAntes]) {

				real->escribirLCD("VICTIMAAAAA");
				//Se detiene
				uint8_t iCase = (cVictima&0b00000001) ? 1 : 2;
				uint16_t encoderTemp1 = eCount1;
				uint16_t encoderTemp2 = eCount2;
				stop();
				//Visual
				if((iVisual < iCalor) && (cVictima & 0b00100000)) {
					//Visual Derecha
					if(iCase == 1) {
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
					while(!(cVictima == 0b10000000) && !(cVictima == 0b01000000) && !(cVictima == 0b00100000) && !(cVictima == 0b00010000) && start + 400 >= millis()) {
						while(Serial2.available()) {
							cVictima = (char)Serial2.read();
						}
					}
					real->escribirLCD("SALI");
					//delay(40000);
					//Si no se pasó de tiempo
					if(start + 400 >= millis()) {
						//H
						if(cVictima == 0b10000000) {
							iVisual++;
							real->escribirLCD("VICTIMA", "HHHHHHHHH");
							delay(700);
							//Si se pasó de kits
							if(iKit > 12) {
								noKit(iCase);
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
							delay(700);
							//Si se pasó de kits
							if(iKit > 12) {
								noKit(iCase);
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
							delay(4500);
						}
						//Nada
						else if(cVictima == 0b00010000) {
							real->escribirLCD("NADA");
						}
					}
					//Time limit
					else{
						real->escribirLCD("SSSSLLLLOOW");
					}
					//Derecha Buscar
					if(iCase == 1) {
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
				else if(!(cVictima & 0b00100000) && (cVictima & 0b00000010)) {
					iCalorD++;
					iCalor = (iCalorD > 4) ? iCalorD : iCalor;
					real->apantallanteLCD("NORMAL");
					//Si se pasó de kits
					if(iKit > 12) {
						noKit(iCase);
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
}

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

bool Movimiento::goToVisitado(char cD) {
	//Fills up the map with infinites
	for (uint8_t i = 0; i < kMapSize; i++)
		for(uint8_t j = 0; j < kMapSize; j++) {
			iMapa[i][j] = 0;
			cMapa[i][j] = 'n';
		}
	//Gives a one to where we begin
	iMapa[*iRow][*iCol] = 1;
	cMapa[*iRow][*iCol] = 'i';
	//Recursive function
	real->escribirLCD("   LLENA", "   MAPA");
	mapa.tiempoI(millis());
	mapa.llenaMapa(iMapa, cMapa, tMapa, *cDir, *iCol, *iRow, *iPiso);
	///////////////Prints the map//////////////////////////////
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
	uint8_t iNCol = 100, iNRow = 100;
	real->escribirLCD("   COMPARA", "   HACER");
	if(mapa.comparaMapa(iMapa, tMapa, cD, *iCol, *iRow, iNCol, iNRow, *iPiso)) {
		hacerInstrucciones(mapa.getInstrucciones(iMapa, cMapa, tMapa, iNCol, iNRow, *iPiso));
		*iCol = iNCol, *iRow = iNRow;
		return true;
	}
	return false;
}

/*
It's a bool so it can be a condition on the main loop
The algorithm is simple: Go Right, if not Go Front, if not Go Left, if not Go Backwards, if none is possible, look up
for the nearest non visited tile. If there's none on this floor, go back to the previous floor, if this isn't possible, return false
and then look up for the initial point.
The conditions to move to another tile is: 
	1. Can I move there? (there's no wall)
	2. Have I not been there? (It hasn't been visited)
Special case: If you already know there's a bumper, just go forward, it's the safest path.

NOTE: All these "sensing" is with the virtual map, that's why it's important that the robot is mapping correctly
*/
bool Movimiento::decidir() {
	bLack = false;
	if(tMapa[*iPiso][*iRow][*iCol].cuadroNegro()) {
		retroceder();
	}
	checarVictima();
	tMapa[*iPiso][*iRow][*iCol].existe(true);
	tMapa[*iPiso][*iRow][*iCol].visitado(true);
	if(tMapa[*iPiso][*iRow][*iCol].bumper() && mapa.sensa_Pared(tMapa, *cDir, *iCol, *iRow, *iPiso, 'u') && mapa.sensaVisitado(tMapa, *cDir, *iCol, *iRow, *iPiso, 'u')) {
		avanzar();
		stop();
		return true;
	}
	if(mapa.sensa_Pared(tMapa, *cDir, *iCol, *iRow, *iPiso, 'r') && mapa.sensaVisitado(tMapa, *cDir, *iCol, *iRow, *iPiso, 'r')) {
		derecha();
		avanzar();
		stop();
		return true;
	}
	else if(mapa.sensa_Pared(tMapa, *cDir, *iCol, *iRow, *iPiso, 'u') && mapa.sensaVisitado(tMapa, *cDir, *iCol, *iRow, *iPiso, 'u')) {
		avanzar();
		stop();
		return true;
	}
	else if(mapa.sensa_Pared(tMapa, *cDir, *iCol, *iRow, *iPiso, 'l') && mapa.sensaVisitado(tMapa, *cDir, *iCol, *iRow, *iPiso, 'l')) {
		izquierda();
		avanzar();
		stop();
		return true;
	}
	else if(mapa.sensa_Pared(tMapa, *cDir, *iCol, *iRow, *iPiso, 'd') && mapa.sensaVisitado(tMapa, *cDir, *iCol, *iRow, *iPiso, 'd')) {
		derecha();
		derecha();
		avanzar();
		stop();
		return true;
	}
	else{
		return goToVisitado( 'n');
	}
}

char Movimiento::getParedes() {
	return cParedes;
}

uint8_t Movimiento::getColor(){
	return iColor;
}

void Movimiento::encoder1() {
	eCount1++;
}

void Movimiento::encoder2() {
	eCount2++;
}

void Movimiento::boton1() {
	bBoton1 = true;
}

void Movimiento::llenaArreglo(bool b){
	for(int i = 0; i < 18; i++){
		der[i] = b;
	}
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
This was my first function for the erros on the angles, I still don't give up on it :( so simple and beautiful
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
