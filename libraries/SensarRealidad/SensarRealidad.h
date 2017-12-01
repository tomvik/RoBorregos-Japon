/* Code made by the RoBorregos team in 2017 for the RoboCup JR. Rescue Maze category.
 * Tomás Lugo, Sebastián Esquer, Ernesto Cervantez, and Alexis Virgen.
 * "El Mariachi" Achieved a third place on the international RoboCup.
 *
 * This class has all the functions to check all the sensors and return the data in a way
 * it can be easily used. It also recieves the communication with the nano
 */
#ifndef SensarRealidad_h
#define SensarRealidad_h

#include <Arduino.h>

class SensarRealidad {
  public:
    /////////////////////CONSTRUCTOR////////////////////
    SensarRealidad();
    //Function that write on the LCD in both lines. You could send only one parameter.
    //Could make a template out of this, but just parse the data and send a string.
    void escribirLCD(String sE1, String sE2 = " ");
    //Blinks and writes the message on the line below.
    void escribirLCDabajo(String sE1);
    //Blinks and writes the message on both lines
    void apantallanteLCD(String sE1, String sE2 = " ");
    //Function that initialize the VL53L0X sensors
    void inicializarSensoresDistancia(const uint8_t kINICIO_I2C);
    //Returns the distance in front (mm)
    int getDistanciaEnfrente();
    //Returns the distance on the right (mm)
    int getDistanciaDerecha();
    //Returns the distance from behind (mm)
    int getDistanciaAtras();
    //Returns the distance from the left (mm)
    int getDistanciaIzquierda();
    //Returns true if there's a path in front
    bool caminoEnfrente();
    //Returns true if there's a path on the right
    bool caminoDerecha();
    //Returns true if there's a path behind
    bool caminoAtras();
    //Returns true if there's a path on the left
    bool caminoIzquierda();
    //Returns the calibration of the IMU
    uint8_t getIMUCalibrationStatus();
    //Calibrates the IMU
    //RECOMENDATION, the Croatian team would always at the beginning calibrate the sensor, maybe we should do that
    void calibracionIMU();
    //Returns the angle x
    bool getAngulo(double &angle);
    //This would tell us of the Serial communication isn't working (I really recommend just some digital pins)
    bool getMalo();
    //Returns the angle y
    double sensarRampa();
    //Returns if it's too inclined to one side
    uint8_t switchesIMU(double fDeseado, double grados);
    //Returns if it hit something
    uint8_t switches();
    //Returns 0 if it's white, 1 if it's black, 2 if it's a checkpoint
    uint8_t color();
    //Returns true if there's a visual victim
    bool visual();
    //Function to test the sensors. Really useful.
    void test();
    //Writes on the EEPROM
    static void escribirEEPROM(int dir, int value);
    //Reads from the EEPROM
    static int leerEEPROM(int dir);
  private:
    double lastAngle;
    bool malo;
};

#endif
