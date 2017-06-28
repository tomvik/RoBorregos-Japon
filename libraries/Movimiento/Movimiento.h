#ifndef Movimiento_h
#define Movimiento_h

#include <Arduino.h>
#include <SensarMapa.h>
#include <SensarRealidad.h>
#include <Tile.h>

//Mapa, direccion, col, row, piso, caso, fdeseado

class Movimiento {
  public:
    //////////////////////////////////Constructores////////////////////////////////////
    Movimiento(uint8_t iPowd, uint8_t iPowi, SensarRealidad *r, char *c, uint8_t *ic, uint8_t *ir, uint8_t *ip);
    //////////////////////////////////Movimientos//////////////////////////////////////
    void velocidad(uint8_t powIzq, uint8_t powDer);
    void stop();
    void StopInterrupt();
    void front();
    void back();
    void right();
    void left();
    void acomodaChoque(uint8_t switchCase);
    void checarBumper();
    void SepararPared();
    void potenciasDerecho(uint8_t &potenciaIzq, uint8_t &potenciaDer);
    void dejarKit(Tile tMapa[3][10][10], uint8_t iCase);
    // void ErrorGradosVuelta(float &error);
    void pasaRampa();
    void retroceder(Tile tMapa[3][10][10]);
    void avanzar(Tile tMapa[3][10][10]);
    void derecha(Tile tMapa[3][10][10]);
    void izquierda(Tile tMapa[3][10][10]);
    void encoder1();
    void encoder2();
    char getParedes();
    void vueltaIzq(Tile tMapa[3][10][10]);
    void vueltaDer(Tile tMapa[3][10][10]);
    void corregirIMU();
    void alinearAtAd();

    //////////////////////////////////Buscar cuadro////////////////////////////////////
    void hacerInstrucciones(Tile tMapa[3][10][10], String sMov);

    //////////////////////////////////Buscar cuadro////////////////////////////////////
    bool goToVisitado(Tile tMapa[3][10][10], char cD);

    ////////////////Decide qué hacer con la prioridad d, e, i, a, busca////////////////
    bool decidir(Tile tMapa[3][10][10]);
    bool decidir_Prueba(Tile tMapa[3][10][10]);
  private:
    volatile uint16_t eCount1, eCount2;
    uint8_t *iCol, *iRow, *iPiso;
    uint8_t iPowI, iPowD, contadorIzq, contadorDer;
    int servo_pos, vueltasDadas, cuadrosVisitados;
    float fSetPoint;
    bool rampa;
    SensarMapa mapa;
    SensarRealidad *real;
    char cVictima, cParedes, *cDir;
    unsigned long lastTime;
    double iTerm, lastInput;
};

#endif
