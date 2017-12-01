/* Code made by the RoBorregos team in 2017 for the RoboCup JR. Rescue Maze category.
 * Tomás Lugo, Sebastián Esquer, Ernesto Cervantez, and Alexis Virgen.
 * "El Mariachi" Achieved a third place on the international RoboCup.
 *
 * This class has all the functions to check the map, and the "hard" part of path finding.
 * It's fun. And there are better ways to do it. Highly recommend A* algorithm, it's the best for this situation.
 *
 */
#ifndef SensarMapa_h
#define SensarMapa_h

#include <Arduino.h>
#include <Tile.h>
///////////Dimensions///////////////////
const uint8_t kMapSizeSensarMapa = 15;
const uint8_t kMapFloorsSensarMapa = 4;

class SensarMapa {
  public:
    ////////////////////////////Constructor//////////////////////////////////////
    SensarMapa();

    ////////////////////////////"Getters"/////////////////////////////////////////
    //Returns true if the path is free
    bool sensa_Pared(Tile tMapa[kMapFloorsSensarMapa][kMapSizeSensarMapa][kMapSizeSensarMapa], char cDir, uint8_t iCol, uint8_t iRow, uint8_t iPiso, char cCase);
    //Returns true if it hasn't been visited yet
    bool sensaVisitado(Tile tMapa[kMapFloorsSensarMapa][kMapSizeSensarMapa][kMapSizeSensarMapa], char cDir, uint8_t iCol, uint8_t iRow, uint8_t iPiso, char cCase);
    //Returns true if it exists
    bool sensaExiste(Tile tMapa[kMapFloorsSensarMapa][kMapSizeSensarMapa][kMapSizeSensarMapa], char cDir, uint8_t iCol, uint8_t iRow, uint8_t iPiso, char cCase);

    ////////////////////////////All this is for the path finding//////////////////////////
    //Recursive function which fills the map with all the possible paths to be taken, considering the lenght and difficulty of it.
    void llenaMapa(uint8_t iMapa[kMapSizeSensarMapa][kMapSizeSensarMapa], char cMapa[kMapSizeSensarMapa][kMapSizeSensarMapa], Tile tMapa[kMapFloorsSensarMapa][kMapSizeSensarMapa][kMapSizeSensarMapa], char cDir, uint8_t iCol, uint8_t iRow, uint8_t iPiso);
    //Compares which of all the possible paths is the shortest and safest, and returns true if there's still an unvisited tile or ramp to a previous floor
    bool comparaMapa(uint8_t iMapa[kMapSizeSensarMapa][kMapSizeSensarMapa], Tile tMapa[kMapFloorsSensarMapa][kMapSizeSensarMapa][kMapSizeSensarMapa], char cD, uint8_t iCol, uint8_t iRow, uint8_t &iNCol, uint8_t &iNRow, uint8_t iPiso);
    //Returns in a string with the reverse set of instructions to follow
    String getInstrucciones(uint8_t iMapa[kMapSizeSensarMapa][kMapSizeSensarMapa], char cMapa[kMapSizeSensarMapa][kMapSizeSensarMapa], Tile tMapa[kMapFloorsSensarMapa][kMapSizeSensarMapa][kMapSizeSensarMapa], uint8_t iNCol, uint8_t iNRow, uint8_t iPiso);
    //Gives the time of the recursive function to initialize.
    void tiempoI(unsigned long ul);

  private:
    unsigned long tInicio;
};

#endif
