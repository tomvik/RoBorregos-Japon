#ifndef SensarMapa_h
#define SensarMapa_h

#include <Arduino.h>
#include <Tile.h>

//Mapa int, Mapa tile, Direccion, Columna, Row, Piso, Caso, Bool
class SensarMapa {
  public:
    ////////////////////////////Constructores default//////////////////////////////////////
    SensarMapa();

    ////////////////////////////"Getters"/////////////////////////////////////////
    //Regresa true si el camino está libre
    bool sensa_Pared(Tile tMapa[3][10][10], char cDir, uint8_t iCol, uint8_t iRow, uint8_t iPiso, char cCase);
    //Regresa true si no ha sido visitado
    bool sensaVisitado(Tile tMapa[3][10][10], char cDir, uint8_t iCol, uint8_t iRow, uint8_t iPiso, char cCase);
    //Regresa true si existe
    bool sensaExiste(Tile tMapa[3][10][10], char cDir, uint8_t iCol, uint8_t iRow, uint8_t iPiso, char cCase);

    ////////////////////////////Para la función de Go_to//////////////////////////
    //Función recursiva que busca el camino más rápido de un tile a los demás
    void llenaMapa(uint8_t iMapa[10][10], char cMapa[10][10], Tile tMapa[3][10][10], char cDir, uint8_t iCol, uint8_t iRow, uint8_t iPiso);
    //Compara cuál es el camino más corto
    bool comparaMapa(uint8_t iMapa[10][10], Tile tMapa[3][10][10], char cD, uint8_t iCol, uint8_t iRow, uint8_t &iNCol, uint8_t &iNRow, uint8_t iPiso);
    //Regresa en string las instrucciones a seguir para ir al tile deseado
    String getInstrucciones(uint8_t iMapa[10][10], char cMapa[10][10], Tile tMapa[3][10][10], uint8_t iNCol, uint8_t iNRow, uint8_t iPiso);

  private:
    uint8_t iTamano, iBumper;
};

#endif
