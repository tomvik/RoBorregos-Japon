#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <math.h>
#include <Tile.h>
#include <SensarMapa.h>
#include <Movimiento.h>
#include <SensarRealidad.h>
#include <Mapear.h>
#include <Servo.h>


#define ENCODER_A 4
#define ENCODER_B 5
Movimiento *mover;
float fDeseado = 0;
float kp = 1;

void encoderAlfa() {
  mover->encoder();
}

void setup() {
  Serial.begin(9600);
  Serial2.begin(115200);
  SensarRealidad sensarr;
  SensarRealidad *sensar = &sensarr;
  sensar->escribirLCD("El", "Mariachi");
  Movimiento robot(150, 150, 10, sensar);
  mover = &robot;
  attachInterrupt(ENCODER_A, encoderAlfa, RISING);
  while(Serial2.available()){
    (char)Serial2.read();
  }
  Mapear mapa(sensar, mover);
  Tile tMapa[3][10][10]; 
  mover->Stop();
  sensar->apantallanteLCD("      El", "    MARIACHI");
  uint8_t iRow = 4, iCol = 4, iPiso = 0;
  char cDir = 'n';
  int iD, iI;
  tMapa[iPiso][iCol][iRow].existe(true);
  tMapa[iPiso][iCol][iRow].inicio(true);
  tMapa[iPiso][iCol][iRow].visitado(true);

  tMapa[iPiso][iRow][iCol].abajo(true, &tMapa[iPiso][iRow + 1][iCol]);

  if(sensar->sensarEnfrente())
    tMapa[iPiso][iRow - 1][iCol].existe(true);
  else
    tMapa[iPiso][iRow][iCol].abajo(true, &tMapa[iPiso][iRow - 1][iCol]);

  if(sensar->sensarDerecha())
    tMapa[iPiso][iRow][iCol + 1].existe(true);
  else
    tMapa[iPiso][iRow][iCol].abajo(true, &tMapa[iPiso][iRow][iCol + 1]);

  if(sensar->sensarIzquierda())
    tMapa[iPiso][iRow][iCol - 1].existe(true);
  else
    tMapa[iPiso][iRow][iCol].abajo(true, &tMapa[iPiso][iRow][iCol - 1]);

  while (mover->decidir(tMapa, cDir, iCol, iRow, iPiso))
    mapa.llenaMapa(tMapa, cDir, iCol, iRow, iPiso);

  sensar->apantallanteLCD("Let's go home...");
  while(true) {
    mover->goToVisitado(tMapa, cDir, 'i', iCol, iRow, iPiso);
    mover->Stop();
  }
}

void loop() {
}
