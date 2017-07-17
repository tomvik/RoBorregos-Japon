#ifndef SensarRealidad_h
#define SensarRealidad_h

#include <Arduino.h>
// HAY QUE INCLUIR TODAS LAS LIBRERIAS DE LOS SENSORES

class SensarRealidad {
  public:
    /////////////////////CONSTRUCTOR////////////////////
    SensarRealidad();
    //Escribe en la LCD en ambas lineas
    void escribirLCD(String sE1, String sE2 = " ");
    //Parpadea en la LCD en ambas lineas
    void apantallanteLCD(String sE1, String sE2 = " ");
    //Inicializa la cantidad de sensores que le pongas
    void inicializarSensoresDistancia(const uint8_t kINICIO_I2C);
    //Regresa la distancia de enfrente
    int getDistanciaEnfrente();
    //Regresa la distancia de la derecha
    int getDistanciaDerecha();
    //Regresa la distancia de atrás
    int getDistanciaAtras();
    //Regresa la distancia de la izquierda
    int getDistanciaIzquierda();
    //Regresa true si está libre el camino enfrente
    bool caminoEnfrente();
    //Regresa true si está libre el camino a la derecha
    bool caminoDerecha();
    //Regresa true si está libre el camino atrás
    bool caminoAtras();
    //Regresa true si está libre el camino a la izquierda
    bool caminoIzquierda();
    //Regresa la calibración del IMU
    uint8_t getIMUCalibrationStatus();
    //Calibra al IMU
    void calibracionIMU();
    //Regresa el Angulo en el eje X
    bool getAngulo(double &angle);
    //Regresa el Angulo en el eje Y
    double sensarRampa();
    //Regresa si está muy inclinado
    uint8_t switchesIMU(double fDeseado, double grados);
    //Regresa si chocó
    uint8_t switches();
    //Regresa 0 si es blanco, 1 si es negro, 2 si es checkpoint
    uint8_t color();
    //Regresa 0 si no hay, 1 si hay victima
    bool visual();
    // Tests
    void test();
    //Escribe en la EEPROM
    static void escribirEEPROM(int dir, int value);
    //Lee de la EEPROM
    static int leerEEPROM(int dir);
  private:
    double lastAngle;
};

#endif
