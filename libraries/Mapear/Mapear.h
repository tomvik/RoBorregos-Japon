#ifndef Mapear_h
#define Mapear_h

#include <Arduino.h>
#include <Tile.h>
#include <Movimiento.h>
#include <SensarRealidad.h>
///////////Dimensiones///////////////////
const uint8_t kMapSizeMapear = 15;
const uint8_t kMapFloorsMapear = 4;

// Usa la clase Tile y la clase SensarRealidad
// NOTE ÓRDEN DE PARÁMETROS
// Mapa, Direccion, Columna, Row, Piso, Caso, Bool

class Mapear {
  public:
    // Constructor default
    Mapear();
    // Constructor con SensarRealidad y Movimiento y last pos
    Mapear(SensarRealidad *ma, Movimiento *ro, uint8_t *iPM);
    // Verifica si hay espacio en la matriz
    bool espacio(char cDir, uint8_t iCol, uint8_t iRow, char cCase);
    //Mapea que hay rampa y todo lo necesario
    void afterRampa(char cDir, uint8_t &iCol, uint8_t &iRow);
    ////////////////Desplaza los datos de la matriz//////////////////
    //Desplaza hacia abajo
    void moverRowAbajo(Tile tMapa[kMapFloorsMapear][kMapSizeMapear][kMapSizeMapear], uint8_t &iPiso);
    //Desplaza hacia arriba
    void moverRowArr(Tile tMapa[kMapFloorsMapear][kMapSizeMapear][kMapSizeMapear], uint8_t &iPiso);
    //Desplaza hacia la izquierda
    void moverColIzq(Tile tMapa[kMapFloorsMapear][kMapSizeMapear][kMapSizeMapear], uint8_t &iPiso);
    //Desplaza hacia la derecha
    void moverColDer(Tile tMapa[kMapFloorsMapear][kMapSizeMapear][kMapSizeMapear], uint8_t &iPiso);
    //Llama a las 4 de desplazar dependiendo del caso y modifica la columna y el row
    void desplazaDatos(Tile tMapa[kMapFloorsMapear][kMapSizeMapear][kMapSizeMapear], char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, char cCase);
    // dependiendo de si es true o false, pone existente o pared.
    void escribeMapaLoP(Tile tMapa[kMapFloorsMapear][kMapSizeMapear][kMapSizeMapear], char cDir, uint8_t iCol, uint8_t iRow, uint8_t &iPiso, char cCase, bool bLoP);
    //////////////////////////////Mapea//////////////////////////////////////////
    //Mapea con las variables
    void llenaMapaVariable(Tile tMapa[kMapFloorsMapear][kMapSizeMapear][kMapSizeMapear], char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);
    //Mapea con los sensores
    void llenaMapaSensor(Tile tMapa[kMapFloorsMapear][kMapSizeMapear][kMapSizeMapear], char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);

  private:
  	uint8_t *iPisoMax;
    uint8_t iColor;
    SensarRealidad *mapa;
    Movimiento *robot;
};

#endif
