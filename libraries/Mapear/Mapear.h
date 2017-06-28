#ifndef Mapear_h
#define Mapear_h

#include "Arduino.h"
#include "Tile.h"
#include "Movimiento.h"
#include "SensarRealidad.h"

// Usa la clase Tile y la clase SensarRealidad
// NOTE ÓRDEN DE PARÁMETROS
// Mapa, Direccion, Columna, Row, Piso, Caso, Bool

class Mapear {
public:
// Constructor default
Mapear();

/*
   Constructor con SensarRealidad y Movimietn
 */
Mapear(SensarRealidad *ma, Movimiento *ro);

// Verifica si hay espacio en la matriz
bool espacio(char cDir, uint8_t iCol, uint8_t iRow, char cCase);

////////////////Desplaza los datos de la matriz//////////////////
//Todos podrían estar en una misma función haciendo un switch entre ellas
void afterRampa(char cDir, uint8_t &iCol, uint8_t &iRow);
void moverRowAbajo(Tile tMapa[3][10][10], uint8_t &iPiso);
void moverRowArr(Tile tMapa[3][10][10], uint8_t &iPiso);
void moverColIzq(Tile tMapa[3][10][10], uint8_t &iPiso);
void moverColDer(Tile tMapa[3][10][10], uint8_t &iPiso);

//Llama a las 4 de arriba dependiendo del caso y modifica la columna y el row


void desplazaDatos(Tile tMapa[3][10][10], char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, char cCase);
/*
   dependiendo de si es true o false, pone existente o pared.
 */
void escribeMapaLoP(Tile tMapa[3][10][10], char cDir, uint8_t iCol, uint8_t iRow, uint8_t &iPiso, char cCase, bool bLoP);

//////////////////////////////Mapea//////////////////////////////////////////
void llenaMapaVariable(Tile tMapa[3][10][10], char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);
void llenaMapaSensor(Tile tMapa[3][10][10], char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);

private:
uint8_t iTamano, iPisoMax, iRampa;
SensarRealidad *mapa;
Movimiento *robot;
};

#endif
