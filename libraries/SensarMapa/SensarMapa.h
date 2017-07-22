#ifndef SensarMapa_h
#define SensarMapa_h

#include <Arduino.h>
#include <Tile.h>
///////////Dimensiones///////////////////
const uint8_t kMapSizeSensarMapa = 15;
const uint8_t kMapFloorsSensarMapa = 4;

//Mapa int, Mapa tile, Direccion, Columna, Row, Piso, Caso, Bool
class SensarMapa {
  public:
    ////////////////////////////Constructores default//////////////////////////////////////
    SensarMapa();

    ////////////////////////////"Getters"/////////////////////////////////////////
    //Regresa true si el camino está libre
    bool sensa_Pared(Tile tMapa[kMapFloorsSensarMapa][kMapSizeSensarMapa][kMapSizeSensarMapa], char cDir, uint8_t iCol, uint8_t iRow, uint8_t iPiso, char cCase);
    //Regresa true si no ha sido visitado
    bool sensaVisitado(Tile tMapa[kMapFloorsSensarMapa][kMapSizeSensarMapa][kMapSizeSensarMapa], char cDir, uint8_t iCol, uint8_t iRow, uint8_t iPiso, char cCase);
    //Regresa true si existe
    bool sensaExiste(Tile tMapa[kMapFloorsSensarMapa][kMapSizeSensarMapa][kMapSizeSensarMapa], char cDir, uint8_t iCol, uint8_t iRow, uint8_t iPiso, char cCase);

    ////////////////////////////Para la función de Go_to//////////////////////////
    //Función recursiva que busca el camino más rápido de un tile a los demás
    void llenaMapa(uint8_t iMapa[kMapSizeSensarMapa][kMapSizeSensarMapa], char cMapa[kMapSizeSensarMapa][kMapSizeSensarMapa], Tile tMapa[kMapFloorsSensarMapa][kMapSizeSensarMapa][kMapSizeSensarMapa], char cDir, uint8_t iCol, uint8_t iRow, uint8_t iPiso);
    //Compara cuál es el camino más corto
    bool comparaMapa(uint8_t iMapa[kMapSizeSensarMapa][kMapSizeSensarMapa], Tile tMapa[kMapFloorsSensarMapa][kMapSizeSensarMapa][kMapSizeSensarMapa], char cD, uint8_t iCol, uint8_t iRow, uint8_t &iNCol, uint8_t &iNRow, uint8_t iPiso);
    //Regresa en string las instrucciones a seguir para ir al tile deseado
    String getInstrucciones(uint8_t iMapa[kMapSizeSensarMapa][kMapSizeSensarMapa], char cMapa[kMapSizeSensarMapa][kMapSizeSensarMapa], Tile tMapa[kMapFloorsSensarMapa][kMapSizeSensarMapa][kMapSizeSensarMapa], uint8_t iNCol, uint8_t iNRow, uint8_t iPiso);
    //Pone el tiempo de inicio para la funcion recursiva
    void tiempoI(unsigned long ul);

  private:
    unsigned long tInicio;
};

#endif
