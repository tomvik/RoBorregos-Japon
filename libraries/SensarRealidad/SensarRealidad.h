#ifndef SensarRealidad_h
#define SensarRealidad_h

#include "Arduino.h"

//HAY QUE INCLUIR TODAS LAS LIBRERIAS DE LOS SENSORES

class SensarRealidad{
public:
	/////////////////////CONSTRUCTOR////////////////////
	SensarRealidad();

	////////////////////SENSA PARED/////////////////////
	//Pensaba que estas funciones hacerlas con sharp
	//Que regresen TRUE si NO HAY pared
	uint8_t calcDistanciaUS(uint8_t trigger, uint8_t echo);
	uint8_t calcDistanciaSharp(uint8_t sensor);
	uint8_t sensarEnfrentePared();
	uint8_t sensarDerechaPared();
	uint8_t sensarIzquierdaPared();
	uint8_t prueba();
	byte getIMUCalStatus();
	bool sensarEnfrente();
	bool sensarAtras();
	bool sensarDerecha();
	bool sensarIzquierda();
	int sensarRampa();
	float sensarOrientacion();
	//uint8_t sensarTemperatura();
	uint8_t switches();
private:
};

#endif