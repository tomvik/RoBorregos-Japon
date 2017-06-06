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
	void potenciasDerecho(float fDeseado, float &grados, int &iPowDD, int &iPowII);
	bool victimaPosible(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, uint8_t &iCase, bool bVuelta, bool bCalor, float fGrados);
	void mapearVictima(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, uint8_t iCase, bool bVuelta, float fGrados);
	void dejarKit(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, uint8_t iCase, float fDeseado);
	void identificaVictima(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, uint8_t iCase, float fDeseado);
	void ErrorGradosVuelta(float fDeseado, float &grados);
	void VueltaGyro(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, float fDeseado, bool kit);
	void pasaRampa(char cDir);
	void retroceder(Tile tMapa[3][10][10], char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);
	void avanzar(Tile tMapa[3][10][10], char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);
	void derecha(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);
	void izquierda(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);
	void encoder();

	//////////////////////////////////Buscar cuadro////////////////////////////////////
	void hacerInstrucciones(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, String sMov);
	bool goToVisitado(Tile tMapa[3][10][10], char &cDir, char cD, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);

	////////////////Decide qué hacer con la prioridad d, e, i, a, busca////////////////
	bool decidir(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);
	bool decidir_Prueba(Tile tMapa[3][10][10], char &cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);
private:
	uint8_t iPowI, iPowD, iTamano, limit_Vision, kParedAlinear, iRampa;
	int encoder30, eCount1, pos;
	unsigned long tVictima;
	float kpA, kp, fRef;
	SensarMapa mapa;
	SensarRealidad *real;
	bool alinear;
	char probVisual, probVisual2, cVictima;
};

#endif
