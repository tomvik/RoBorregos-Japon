#ifndef Movimiento_h
#define Movimiento_h

#include <Arduino.h>
#include <SensarMapa.h>
#include <SensarRealidad.h>
#include <Tile.h>

// Mapa, direccion, col, row, piso, caso, fdeseado

class Movimiento {
  public:
    //////////////////////////////////Constructor////////////////////////////////////
    Movimiento(uint8_t iPowd, uint8_t iPowi, SensarRealidad *r, char *c, uint8_t *ic, uint8_t *ir, uint8_t *ip);

    //////////////////////////////////Movimientos//////////////////////////////////////
    //Ajusta la velocidad
    void velocidad(uint8_t powIzq, uint8_t powDer);
    //Para los motores
    void stop();
    //Pone los motores para enfrente
    void front();
    //Pone los motores para atrás
    void back();
    //Pone los motores para la derecha
    void right();
    //Pone los motores para la izquierda
    void left();
    //Se acomoda tras chocar con la pared dependiendo del caso
    void acomodaChoque(uint8_t switchCase);
    //Se separa de la pared de enfrente
    void separarPared();
    //Calcula la velocidad para avanzar derecho
    void potenciasDerecho(uint8_t &potenciaIzq, uint8_t &potenciaDer);
    //Deja kit y mapea que hay una victima
    void dejarKit(Tile tMapa[3][10][10], uint8_t iCase);

    // void ErrorGradosVuelta(float &error);
    //Pasa la rampa
    void pasaRampa();
    //Retrocede cuando hay un tile negro
    void retroceder(Tile tMapa[3][10][10]);
    //Avanza un cuadro
    void avanzar(Tile tMapa[3][10][10]);
    //Da vuelta a la derecha de 90
    void derecha(Tile tMapa[3][10][10]);
    //Da vuelta a la izquierda de 90
    void izquierda(Tile tMapa[3][10][10]);
    //Controla la veloicad para dar la vuelta a la izquierda
    void vueltaIzq(Tile tMapa[3][10][10]);
    //Controla la velocidad para dar la vuelta a la derecha
    void vueltaDer(Tile tMapa[3][10][10]);
    //Corrige el IMU con la pared de atrás
    void corregirIMU();
    //No está puesta wtf
    void alinearAtAd();
    //Encoder 1
    void encoder1();
    //Encoder 2
    void encoder2();

    //Hace las instrucciones para llegar al cuadro deseado
    void hacerInstrucciones(Tile tMapa[3][10][10], String sMov);

    //////////////////////////////////Buscar cuadro////////////////////////////////////
    bool goToVisitado(Tile tMapa[3][10][10], char cD);

    ////////////////Decide qué hacer con la prioridad d, e, i, a, busca////////////////
    bool decidir(Tile tMapa[3][10][10]);
    // bool decidir_Prueba(Tile tMapa[3][10][10]);

    /////////////Getter//////////
    //Regresa cuales paredes se mapeó
    char getParedes();

  private:
    int iTerm, lastInput;
    char cVictima, cParedes, *cDir;
    uint8_t *iCol, *iRow, *iPiso;
    uint8_t servo_pos, iPowI, iPowD, contadorIzq, contadorDer, resetIMU;
    volatile uint16_t eCount1, eCount2;
    SensarMapa mapa;
    SensarRealidad *real;
};

#endif
