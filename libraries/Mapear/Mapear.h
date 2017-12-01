/* Code made by the RoBorregos team in 2017 for the RoboCup JR. Rescue Maze category.
 * Tomás Lugo, Sebastián Esquer, Ernesto Cervantez, and Alexis Virgen.
 * "El Mariachi" Achieved a third place on the international RoboCup.
 *
 * Since the size of the matrix is fixed, sometimes we need to shift the data
 * So we don't get out of bounds. Also, we have two modes of mapping.
 * The "normal" being with the sensors: The procedure is: Reach destination -> stop -> sense all at once (one or two times) -> map
 *
 * The other one (Which I prefer but doesn't apply always. See special cases below) is way faster and is redundant (that is good)
 * The procedure is: While reaching the destination, mark how many times there was a wall -> if n > threshold, there's a wall -> map with variables
 *
 * Special cases: When the robot doesn't move (first tile), or finishes going through a ramp.
 */
#ifndef Mapear_h
#define Mapear_h

#include <Arduino.h>
#include <Tile.h>
#include <Movimiento.h>
#include <SensarRealidad.h>
///////////Dimensions of the map, could it be better to just pass it like tMapa*** ? probably, never had the time to actually try it///////////////////
const uint8_t kMapSizeMapear = 15;
const uint8_t kMapFloorsMapear = 4;

// Uses the class Tile and SensarRealidad
// ORDER OF PARAMETERS: Map, Direction, Column, Row, Floor, Case, Boolean

class Mapear {
  public:
    //Default Constructor (never really used)
    Mapear();
    //Constructor with SensarRealidad and Movimiento and last Max Floor
    Mapear(SensarRealidad *ma, Movimiento *ro, uint8_t *iPM);

    ////////////////Shifting the data of the matrix//////////////////
    //Returns true if there's space on the matrix (If you're on map[0][14] and want to put something on the right, there's no space)
    bool espacio(char cDir, uint8_t iCol, uint8_t iRow, char cCase);
    //Downwards
    void moverRowAbajo(Tile tMapa[kMapFloorsMapear][kMapSizeMapear][kMapSizeMapear], uint8_t &iPiso);
    //Upwards
    void moverRowArr(Tile tMapa[kMapFloorsMapear][kMapSizeMapear][kMapSizeMapear], uint8_t &iPiso);
    //To the left
    void moverColIzq(Tile tMapa[kMapFloorsMapear][kMapSizeMapear][kMapSizeMapear], uint8_t &iPiso);
    //To the Right
    void moverColDer(Tile tMapa[kMapFloorsMapear][kMapSizeMapear][kMapSizeMapear], uint8_t &iPiso);
    //Calls the 4 above depending on the case and updates the position
    void desplazaDatos(Tile tMapa[kMapFloorsMapear][kMapSizeMapear][kMapSizeMapear], char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, char cCase);

    ////////////////Mapping//////////////////
    //Handles the new position after the ramp
    void afterRampa(char cDir, uint8_t &iCol, uint8_t &iRow);
    //Depending on the boolean, writes if there's a wall or not. If true, there's a path. If false, there's a wall
    void escribeMapaLoP(Tile tMapa[kMapFloorsMapear][kMapSizeMapear][kMapSizeMapear], char cDir, uint8_t iCol, uint8_t iRow, uint8_t &iPiso, char cCase, bool bLoP);

    ////////////////Mode//////////////////
    //With the variables
    void llenaMapaVariable(Tile tMapa[kMapFloorsMapear][kMapSizeMapear][kMapSizeMapear], char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);
    //With the sensors
    void llenaMapaSensor(Tile tMapa[kMapFloorsMapear][kMapSizeMapear][kMapSizeMapear], char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso);

  private:
  	//Can the pointers be constant in private if I'm going to initialize them on the constructor? *thinking face*... actually just checked 
  	//Stack overflow, it seems pretty fucked up how to do that, is like calling an initializer after an initializer, like for a parent. Could work.
  	uint8_t *iPisoMax;
    uint8_t iColor;
    SensarRealidad *mapa;
    Movimiento *robot;
};

#endif
