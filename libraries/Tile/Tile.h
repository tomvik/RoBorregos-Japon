#ifndef Tile_h
#define Tile_h

#include "Arduino.h"

/*
*Podria considerarse usar bitset, hace en automatico todo esto...
*Usar Macros para evitar el if en los Setters
#define victima(true) dato1|='\xnn'
#define victima(false) dato1&='\xnn'
*/

class Tile{
    public:
        //Constructor
        Tile();
        //Getters
        /*
        Con & and comprobamos si es 0 o 1
         Ponemos un valor literal ' ' de char para evitar conversiones inecesarias
        Poniendo '\xhhh' se puede especificar el valor en hexadecimal del char ascii
        */  
        bool arriba();

        bool derecha();

        bool abajo();

        bool izquierda();

        bool victima();

        bool cuadroNegro();

        bool checkpoint();

        bool visitado();

        bool inicio();

        bool rampaArriba();

        bool rampaAbajo();

        bool bumper();

        bool existe();

        uint8_t piso();

        bool calorArriba();

        bool calorDerecha();

        bool calorAbajo();

        bool calorIzquierda();

        bool visualArriba();

        bool visualDerecha();

        bool visualAbajo();

        bool visualIzquierda();
        //Setters
        /* 
        Nota: Los datos booleanos si true=1
        Con | or ponemos el 1 que manden, &and ponemos 0
        */
        void victima(const bool &b);

        void cuadroNegro(const bool &b);

        void checkpoint(const bool &b);

        void visitado(const bool &b);

        void inicio(const bool &b);

        void rampaArriba(const bool &b);

        void bumper(const bool &b);

        void arriba(const bool &b, Tile *laDeArriba);

        void abajo(const bool &b, Tile *laDeAabajo);

        void derecha(const bool &b, Tile *laDeDerecha);

        void izquierda(const bool &b, Tile *laDeIzquierda);

        void existe(const bool &b);

        void piso(const int &i);

        void rampaAbajo(const bool &b);

        void calorArriba(const bool &b);

        void calorDerecha(const bool &b);

        void calorAbajo(const bool &b);

        void calorIzquierda(const bool &b);

        void visualArriba(const bool &b);

        void visualDerecha(const bool &b);

        void visualAbajo(const bool &b);

        void visualIzquierda(const bool &b);

    private:
        char dato1, dato2, dato3; 
            //dato1, //arr, der, aba, izq, victima, cuadro negro, checkpoint, visitada 
            //dato2,  //0, 0, 0, Rampabajo, existe, inicio, rampaArriba, bumper...........//Primeros 3 usados para enumeración de piso
            //dato3; //calorArr, calorDer, calorAba, calorIzq, visualArr, visualDer, visualAba, visualIzq
};

#endif