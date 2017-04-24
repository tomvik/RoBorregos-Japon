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

Movimiento *mover;
void setup() {
  SensarRealidad sensarr;
  SensarRealidad *sensar = &sensarr;
  Movimiento robot(100, 100, 10, sensar);
  mover = &robot;
  mover->Stop();
  Serial.begin(9600);
  Serial2.begin(9600);
  while(Serial2.available()){
      Serial2.read();
  }
  attachInterrupt(4, encoder1, RISING);
  Mapear mapa(sensar, mover);
  Tile tMapa[3][10][10];
  uint8_t iRow = 4, iCol = 4, iPiso = 0;
  char cDir = 'n';
  tMapa[iPiso][iCol][iRow].inicio(true);
  tMapa[iPiso][iCol][iRow].visitado(true);
  tMapa[iPiso][iCol][iRow].existe(true);
  Serial2.println("Avanza");
  if(sensar->sensarAtras()){
    tMapa[iPiso][iRow+1][iCol].existe(true);
  }
  else{
    tMapa[iPiso][iRow][iCol].abajo(true, &tMapa[iPiso][iRow+1][iCol]);
  }
  mover->Stop();
  mapa.llenaMapa(tMapa, cDir, iCol, iRow, iPiso);
  while (mover->decidir(tMapa, iCol, iRow, cDir, iPiso)) {
    mapa.llenaMapa(tMapa, cDir, iCol, iRow, iPiso);
  }
  while(true){
    mover->goToVisitado(tMapa, iCol, iRow, cDir, 'i', iPiso);
    mover->Stop();
  }
}


void loop() {
  // put your main code here, to run repeatedly:
}

void encoder1() {
  mover->encoder();
}
