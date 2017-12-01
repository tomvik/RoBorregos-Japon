/* Code made by the RoBorregos team in 2017 for the RoboCup JR. Rescue Maze category.
 * Tomás Lugo, Sebastián Esquer, Ernesto Cervantez, and Alexis Virgen.
 * "El Mariachi" Achieved a third place on the international RoboCup.
 *
 * This class has all the functions related to movement. Oh boi, this is the funniest one.
 *
*/
#ifndef Movimiento_h
#define Movimiento_h

#include <Arduino.h>
#include <SensarMapa.h>
#include <SensarRealidad.h>
#include <Tile.h>
///////////Dimensions///////////////////
const uint8_t kMapSizeMovimiento = 15;
const uint8_t kMapFloorsMovimiento = 4;

class Movimiento {
  public:
    /// Constructor with parameters (Wait to see the constructor con de .cpp, it's great.)
    Movimiento(uint8_t iPowd, uint8_t iPowi, SensarRealidad *r, char *c, uint8_t *ic, uint8_t *ir, uint8_t *ip, Tile (*tM)[kMapSizeMovimiento][kMapSizeMovimiento], uint8_t *iPM);

    //////////////////////////////////Core movements//////////////////////////////////////
    /// Adjust velocity to be in range and then set it to the motors
    void velocidad(uint8_t powIzq, uint8_t powDer);
    /// Stop the Motors
    void stop();
    /// Put the motors in "Front" Mode
    void front();
    /// Put the motors in "Back" Mode
    void back();
    /// Put the motors in "Right" Mode
    void right();
    /// Put the motors in "Left" Mode
    void left();

    //////////////////////////////////Correction movements//////////////////////////////////////
    /// This is a function called after every turn or moveFront, it's so the robot is at the center of the tile, only where there's no bumper on the tile.
    void alinear();
    /// It alligns itself after bumping with a wall with one of the two switches, or the angle by the IMU.
    void acomodaChoque(uint8_t switchCase);
    /// "Fixes" the IMU accumulated erro by alligning to a wall once in a while and taking that new value as a reference.
    void corregirIMU();

    //////////////////////////////////PID controls//////////////////////////////////////
    /// Calculates the power for each motor to go straight, depending on the distance of the wall and the IMU
    /// There were different cases for going up or down the ramp
    void potenciasDerecho(uint8_t &potenciaIzq, uint8_t &potenciaDer, uint8_t caso = 0);
    /// Controls the velocity while turning left
    void vueltaIzq(bool caso = true);
    /// Controls the velocity while turning right
    void vueltaDer(bool caso = true);


    //////////////////////////////////Complete movements//////////////////////////////////////
    /// Goes one tile forward
    void avanzar();
    /// Turns 90 degrees to the right
    void derecha();
    /// Turns 90 degrees to the left
    void izquierda();
    /// Goes one tile backwards
    void retroceder();
    /// Goes up or down the ramp
    void pasaRampa();


    //////////////////////////////////Victims related//////////////////////////////////////
    /// Leaves a kit and maps that there's a victim in there
    void dejarKit(uint8_t iCase);
    /// Leaves no kit, but waits and maps that there's a victim in there
    void noKit(uint8_t iCase);
    /// Checks if there's a victim and where... man this function was a mess with serial
    void checarVictima(bool caso = true);


    //////////////////////////////////Path Finding//////////////////////////////////////
    /// Makes the instructions that the path finding gave to him
    void hacerInstrucciones(String sMov);
    /// Searches the nearest non visited tile, or ramp or initial point
    bool goToVisitado(char cD);


    //////////////////////////////////Main function//////////////////////////////////////
    /// Decides where to go
    bool decidir();

    //////////////////////////////////Getter//////////////////////////////////////
    //Returns where are the walls by the variables
    char getParedes();
    //Returns if there was a color or not
    uint8_t getColor();


    //////////////////////////////////Auxiliar//////////////////////////////////////
    /// Encoder 1
    void encoder1();
    /// Encoder 2
    void encoder2();
    // Button 1
    void boton1();
    //Updates the array, this was for the vision part.
    /*
    Because our distance sensor was on the front part of the robot and the camera on the back part, 
    There was like 12cm of space in between, so in some cases the sensor could be sensing a wall, when in reality there was none to the camera
    */
    void llenaArreglo(bool b);
    //Shows the map in serial. Really useful to debug the algorithm
    //void muestra();

  private:
  	Tile (*tMapa)[kMapSizeMovimiento][kMapSizeMovimiento];
    int iTerm, lastInput, cuadrosSeguidos;
    bool bBoton1, bLack;
    char cVictima, cParedes, *cDir, *cDirLast;
    uint8_t *iCol, *iRow, *iPiso, *iPisoMax;
    uint8_t servo_pos, iPowI, iPowD, contadorIzq, contadorDer, resetIMU, iColor, iVisual, iCalor, iCalorD, iKit;
    volatile uint16_t eCount1, eCount2;
    SensarMapa mapa;
    SensarRealidad *real;
};

#endif
