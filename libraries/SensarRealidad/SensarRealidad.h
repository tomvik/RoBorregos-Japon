#ifndef SensarRealidad_h
#define SensarRealidad_h

#include <Arduino.h>
// HAY QUE INCLUIR TODAS LAS LIBRERIAS DE LOS SENSORES

class SensarRealidad {
  public:
    /////////////////////CONSTRUCTOR////////////////////
    SensarRealidad();

    void escribirLCD(String sE1, String sE2 = " ");
    void apantallanteLCD(String sE1, String sE2 = " ");

    void inicializarSensoresDistancia(const uint8_t kINICIO_I2C);
    int getDistanciaEnfrente();
    int getDistanciaDerecha();
    int getDistanciaAtras();
    int getDistanciaIzquierda();
    bool caminoEnfrente();
    bool caminoDerecha();
    bool caminoAtras();
    bool caminoIzquierda();

    uint8_t getIMUCalibrationStatus();
    void calibracionIMU();
    double getAngulo();
    double sensarRampa();
    uint8_t switchesIMU(double fDeseado, double grados);

    uint8_t switches();

    bool color();

    static void escribirEEPROM(int dir, int value);
    static int leerEEPROM(int dir);
};

#endif
