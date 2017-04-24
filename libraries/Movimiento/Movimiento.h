#ifndef Movimiento_h
#define Movimiento_h

#include "Arduino.h"
#include <SensarMapa.h>
#include <SensarRealidad.h>
#include <Tile.h>

class Movimiento{
public:
	//////////////////////////////////Constructores////////////////////////////////////
	Movimiento();
	Movimiento(uint8_t iPowi, uint8_t iPowd, uint8_t iT, SensarRealidad *r);
	//////////////////////////////////Movimientos//////////////////////////////////////
	void Stop();
	void StopInterrupt();
	void Front(uint8_t powD, uint8_t powI);
	void Back(uint8_t powD, uint8_t powI);
	void Right(uint8_t powD, uint8_t powI);
	void Left(uint8_t powD, uint8_t powI);
	void acomodaChoque(uint8_t switchCase);
	void SepararPared();
	void AlineaPA(char cDir);
	void potenciasDerecho(float fDeseado, float &grados, int &iPowDD, int &iPowII);
	void dejarKit(float fDeseado, Tile tMapa[3][10][10], uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, uint8_t iCase);
	void identificaVictima(Tile tMapa[3][10][10], uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, uint8_t iCase, float fDeseado);
	void ErrorGradosVuelta(float fDeseado, float &grados);
	void VueltaGyro(float fDeseado, Tile tMapa[3][10][10], uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);
	void pasaRampa(char cDir);
	void retroceder(uint8_t &iCol, uint8_t &iRow, char cDir, Tile tMapa[3][10][10], uint8_t &iPiso);
	void avanzar(uint8_t &iCol, uint8_t &iRow, char cDir, Tile tMapa[3][10][10], uint8_t &iPiso);					//Modificar motores y PID
	void derecha(char &cDir, Tile tMapa[3][10][10], uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);														//Modificar motores y PID
	void izquierda(char &cDir, Tile tMapa[3][10][10], uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);														//Modificar motores y PID
	void encoder();

	//////////////////////////////////Buscar cuadro////////////////////////////////////
	void hacerInstrucciones(String sMov, uint8_t &iCol, uint8_t &iRow, char &cDir, Tile tMapa[3][10][10], uint8_t &iPiso);		//Hace las instrucciones de abajo
	bool goToVisitado(Tile tMapa[3][10][10], uint8_t &iCol, uint8_t &iRow, char &cDir, char cD, uint8_t &iPiso);		//Busca no visitado
	
	////////////////Decide qué hacer con la prioridad d, e, i, a, busca////////////////
	bool decidir(Tile tMapa[3][10][10], uint8_t &iCol, uint8_t &iRow, char &cDir, uint8_t &iPiso);				//Decide a donde moverse, modificar bool
private:
	uint8_t iPowI;
	uint8_t iPowD;
	uint8_t iTamano, limit_Vision;
	int encoder30;
	float kpA;
	int eCount1, pos;
	float kp, fRef;
	SensarMapa mapa;
	SensarRealidad *real;
	bool alinear;
	char probVisual, probVisual2;
	uint8_t kParedAlinear;
};

#endif