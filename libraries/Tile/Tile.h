/* Code made by the RoBorregos team in 2017 for the RoboCup JR. Rescue Maze category.
 * Tomás Lugo, Sebastián Esquer, Ernesto Cervantez, and Alexis Virgen.
 * "El Mariachi" Achieved a third place on the international RoboCup.
 *
 * This is the core of the code, this is the class of how we save the data and manage it.
 * 
 * We used bitwise in here, this so instead of having a possibility on a boolean (a byte), we have 8 possibilities on a char or int (8 bits).
 * The operations are simple, pure | (or) and & (and) to assign or compare the data.
 *
 */
#ifndef Tile_h
#define Tile_h

#include <Arduino.h>

/*
  IDEA We could use bitset, it could be a microoptimization.
  Didn't implement the possibility to change a data, could work later on.
  Also, didn't implement at the end the possibility of having several victims, maybe a thing to do later.
  Also, didn't implement the size of a bumper (Magnitud of how "bad" is the bumper)

  Using macros to don't have to use an if on the setters could be better:

  #define victima(true) dato1|='\xnn'
  #define victima(false) dato1&='\xnn'
*/

class Tile {
  public:
    //Constructor
    Tile();

    //Getters
    //Returns true if there's a wall up
    bool arriba();
    //Returns true if there's a wall on the right
    bool derecha();
    //Returns true if there's a wall down
    bool abajo();
    //Returns true if there's a wall on the left
    bool izquierda();
    //Returns true if there's a victim somewhere
    bool victima();
    //Returns true if there tile is black
    bool cuadroNegro();
    //Returns true if there tile is a checkpoint
    bool checkpoint();
    //Returns true if that tile has been visited
    bool visitado();
    //Returns the number of the floor it connects to
    uint8_t piso();
    //Returns true if that tile is going down the ramp
    bool rampaAbajo();
    //Returns true if that tile is going down the ramp
    bool rampaArriba();
    //Returns true if that tile exists (It exists if you have sensed there's a path to there)
    bool existe();
    //Returns true if that tile is the initial point
    bool inicio();
    //Returns true if there's a bumper on that tile
    bool bumper();

    //Setters¿
    //Assign the upper wall and the lower one of the upper tile
    void arriba(const bool &b, Tile *laDeArriba);
    //Assign the wall on the right and the one on the left of the right tile
    void derecha(const bool &b, Tile *laDeDerecha);
    //Assign the lower wall and the upper one of the lower tile
    void abajo(const bool &b, Tile *laDeAabajo);
    //Assign the wall on the left and the one on the right of the left tile
    void izquierda(const bool &b, Tile *laDeIzquierda);
    //Assign a victim to the tile
    void victima(const bool &b);
    //Assign as a black tile the actual tile
    void cuadroNegro(const bool &b);
    //Assign as a checkpoint the actual tile
    void checkpoint(const bool &b);
    //Assign as visited the actual tile
    void visitado(const bool &b);
    //Assign the floor to which it connects
    void piso(const int &i);
    //Assign that it's a ramp going down
    void rampaAbajo(const bool &b);
    //Assign that the tile exists
    void existe(const bool &b);
    //Assign the actual tile as the beginning
    void inicio(const bool &b);
    //Assign that it's a ramp going up
    void rampaArriba(const bool &b);
    //Assign that there's a bumper on the tile
    void bumper(const bool &b);

  private:
    char dato1, dato2;
    // dato1: up, right, down, left, victim, black tile, checkpoint, visited
    // dato2: (First tree used to assign the floor number), Down ramp, Exists, Beginning, UpRamp, Bumper
};

#endif
