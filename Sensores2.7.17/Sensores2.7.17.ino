#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Tile.h>
#include <SensarMapa.h>
#include <Movimiento.h>
#include <SensarRealidad.h>
#include <Mapear.h>
#include <Servo.h>

#define switchIzquierda 35
#define switchDerecha 34

Movimiento *mover;

void setup() {
  Serial.begin(9600);
  //attachInterrupt(5, encoder1, RISING);
  SensarRealidad sensarr;
  SensarRealidad *sensar = &sensarr;
  Movimiento robot(100, 100, 10, sensar);
  mover = &robot;
  Mapear mapa(sensar, mover);
  Tile tMapa[3][10][10];
  uint8_t iRow = 0, iCol = 0, iPiso = 0 ;
  char cDir = 'n';
  tMapa[iPiso][iCol][iRow].inicio(true);
  tMapa[iPiso][iCol][iRow].visitado(true);
  tMapa[iPiso][iCol][iRow].existe(true);
  delay(3000);
  Serial.println("HOLA");
  float grados = sensar->sensarOrientacion();
  int pI, pD;
  while(true){
    
  }
  /*mover->derecha(cDir, tMapa, iCol, iRow, iPiso);
  mapa.llenaMapa(tMapa, cDir, iCol, iRow, iPiso);
  mover->izquierda(cDir, tMapa, iCol, iRow, iPiso);*/
  mapa.llenaMapa(tMapa, cDir, iCol, iRow, iPiso);
  while (mover->decidir(tMapa, iCol, iRow, cDir, iPiso)) {
    mapa.llenaMapa(tMapa, cDir, iCol, iRow, iPiso);
  }
  //mover->goToVisitado(tMapa, iCol, iRow, cDir, 'i', iPiso);*/
}


void loop() {
  // put your main code here, to run repeatedly:

}

void encoder1() {
  mover->encoder();
}
