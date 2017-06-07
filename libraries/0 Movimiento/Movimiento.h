#ifndef Movimiento_h
#define Movimiento_h

#include "Arduino.h"
#include <SensarMapa.h>
#include <SensarRealidad.h>
#include <Tile.h>

//Mapa, direccion, col, row, piso, caso, fdeseado

class Movimiento{
public:
	//////////////////////////////////Constructores////////////////////////////////////
	Movimiento();
	Movimiento(uint8_t iPowd, uint8_t iPowi, uint8_t iT, SensarRealidad *row);
	//////////////////////////////////Movimientos//////////////////////////////////////
	void Stop();
	void StopInterrupt();
	void Front(uint8_t powD, uint8_t powI);
	void Back(uint8_t powD, uint8_t powI);
	void Right(uint8_t powD, uint8_t powI);
	void Left(uint8_t powD, uint8_t powI);
	void acomodaChoque(uint8_t switchCase);
	void checarBumper();
	void SepararPared();
	void AlineaPA(char cDir);
	void potenciasDerecho(int &potenciaDer, int &potenciaIzq);
	void dejarKit(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, uint8_t iCase);
	void ErrorGradosVuelta(float &error);
	void VueltaGyro(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, bool kit);
	void pasaRampa(char cDir);
	void retroceder(Tile tMapa[3][10][10], char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);
	void avanzar(Tile tMapa[3][10][10], char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);
	void derecha(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);
	void izquierda(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);
	void encoder();
	char getParedes();

	//////////////////////////////////Buscar cuadro////////////////////////////////////
	void hacerInstrucciones(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, String sMov);
	bool goToVisitado(Tile tMapa[3][10][10], char &cDir, char cD, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);

	////////////////Decide qué hacer con la prioridad d, e, i, a, busca////////////////
	bool decidir(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);
	bool decidir_Prueba(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);
private:
	uint8_t iPowI, iPowD, iTamano, kParedAlinear, iRampa, contadorIzq, contadorDer;
	int encoder30, eCount1, pos;
	float kpA, kp, fRef, fDeseado;
	SensarMapa mapa;
	SensarRealidad *real;
	bool alinear;
	char cVictima, cParedes;
};

#endif
