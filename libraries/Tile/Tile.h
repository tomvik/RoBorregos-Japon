#ifndef Tile_h
#define Tile_h

#include <Arduino.h>

/*
  IDEA Podria considerarse usar bitset, hace en automatico todo esto... #optimizacion
  Usar Macros para evitar el if en los Setters
  #define victima(true) dato1|='\xnn'
  #define victima(false) dato1&='\xnn'
*/

class Tile {
  public:
    //Constructor
    Tile();

    //Getters
    /*
       Con & and comprobamos si es 0 o 1
       Ponemos un valor literal ' ' de char para evitar conversiones inecesarias
       Poniendo '\xhhh' se puede especificar el valor en hexadecimal del char ascii
     */
    //Regresa true si hay pared arriba
    bool arriba();
    //Regresa true si hay pared a la derecha
    bool derecha();
    //Regresa true si hay pared abajo
    bool abajo();
    //Regresa true si hay pared a la izquierda
    bool izquierda();
    //Regresa true si hay victima en el cuadro
    bool victima();
    //Regresa true si es cuadro negro
    bool cuadroNegro();
    //Regresa true si es checkpoint
    bool checkpoint();
    //Regresa true si ha sido visitado
    bool visitado();
    //Regresa el piso al que conecta
    uint8_t piso();
    //Regresa true si es rampa hacia abajo
    bool rampaAbajo();
    //Regresa true si es rampa hacia arriba
    bool rampaArriba();
    //Regresa true si existe el cuadro
    bool existe();
    //Regresa true si es inicio
    bool inicio();
    //Regresa true si hay un bumper
    bool bumper();

    /*
    bool victimaArriba();
    bool victimaDerecha();
    bool victimaAbajo();
    bool victimaIzquierda();
    bool visualArriba();
    bool visualDerecha();
    bool visualAbajo();
    bool visualIzquierda();
    */

    //Setters
    /*
       Nota: Los datos booleanos si true=1
       Con | or ponemos el 1 que manden, &and ponemos 0
     */
    //Asigna pared arriba al actual y abajo al de arriba
    void arriba(const bool &b, Tile *laDeArriba);
    //Asigna pared derecha al actual e izquierda al de la derecha
    void derecha(const bool &b, Tile *laDeDerecha);
    //Asigna pared abajo al actual y arriba al de abajo
    void abajo(const bool &b, Tile *laDeAabajo);
    //Asigna pared izquierda al actual y derecha a la izquierda
    void izquierda(const bool &b, Tile *laDeIzquierda);
    //Asigna victima al cuadro actual
    void victima(const bool &b);
    //Asigna cuadro negro al tile actual
    void cuadroNegro(const bool &b);
    //Asigna checkpoint al tile actual
    void checkpoint(const bool &b);
    //Asigna al tile actual como visitado
    void visitado(const bool &b);
    //Asigna a cuál piso conecta
    void piso(const int &i);
    //Asigna que es rampa hacia abajo
    void rampaAbajo(const bool &b);
    //Asigna que el tile es existente
    void existe(const bool &b);
    //Asigna que el tile actual es el inicial
    void inicio(const bool &b);
    //Asigna que es rampa hacia arriba
    void rampaArriba(const bool &b);
    //Asigna que hay un bumper
    void bumper(const bool &b);

    /*
    void victimaArriba(const bool &b);
    void victimaDerecha(const bool &b);
    void victimaAbajo(const bool &b);
    void victimaIzquierda(const bool &b);
    void visualArriba(const bool &b);
    void visualDerecha(const bool &b);
    void visualAbajo(const bool &b);
    void visualIzquierda(const bool &b);
    */

  private:
    char dato1, dato2;
    // dato1: arr, der, aba, izq, victima, cuadro negro, checkpoint, visitada
    // dato2: 0, 0, 0, Rampabajo, existe, inicio, rampaArriba, bumper (Primeros 3 usados para enumeración de piso)
    // dato3: vicArr, vicDer, vicAba, vicIzq, visualArr, visualDer, visualAba, visualIzq
};

#endif
