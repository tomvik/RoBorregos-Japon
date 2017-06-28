#ifndef SensarRealidad_h
#define SensarRealidad_h

#include "Arduino.h"

//HAY QUE INCLUIR TODAS LAS LIBRERIAS DE LOS SENSORES

class SensarRealidad{
public:
	/////////////////////CONSTRUCTOR////////////////////
	SensarRealidad();

	uint8_t sensarEnfrentePared();
	uint8_t sensarDerechaPared();
	uint8_t sensarIzquierdaPared();
	uint8_t sensarAtrasPared();
	bool color();
	byte getIMUCalStatus();
	bool sensarEnfrente();
	bool sensarAtras();
	bool sensarDerecha();
	bool sensarIzquierda();
	int sensarRampa();
	float sensarRampaFloat();
	float sensarOrientacion();
	float getAngulo();
	//uint8_t sensarTemperatura();
	uint8_t switches();
	uint8_t switchesIMU(float fDeseado, float grados);
	void escribirLCD(String sE1, String sE2 = " ");
	void apantallanteLCD(String sE1, String sE2 = " ");

	static void escribirEEPROM(int dir, int value);
	static int leerEEPROM(int dir);
	void inicializar(int x);
	void imu();

private:
};

#endif
