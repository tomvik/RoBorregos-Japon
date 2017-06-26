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

uint8_t iRow = 4, iCol = 4, iPiso = 0;
uint8_t *iR = &iRow, *iC = &iCol, *iP = &iPiso;
char cDir = 'n';
char *cD = &cDir;
Movimiento *mover;
void setup() {
  Serial.begin(9600);
  Serial2.begin(115200);
  while(Serial2.available()){
      Serial2.read();
  }
  attachInterrupt(4, encoder1, RISING); 
  SensarRealidad sensarr;
  SensarRealidad *sensar = &sensarr;
  sensar->escribirLCD("El", "Mariachi");
  Movimiento robot(150, 150, sensar, cD, iC, iR, iP);
  mover = &robot;
  Mapear mapa(sensar, mover);
  
  Tile tMapa[3][10][10];
  
  tMapa[iPiso][iCol][iRow].inicio(true);
  tMapa[iPiso][iCol][iRow].visitado(true);
  tMapa[iPiso][iCol][iRow].existe(true);

  tMapa[iPiso][iRow][iCol].abajo(true, &tMapa[iPiso][iRow+1][iCol]);
  if(sensar->sensarEnfrente()){
    tMapa[iPiso][iRow-1][iCol].existe(true);
  }
  else{
    tMapa[iPiso][iRow][iCol].arriba(true, &tMapa[iPiso][iRow-1][iCol]);
  }
  if(sensar->sensarDerecha()){
    tMapa[iPiso][iRow][iCol+1].existe(true);
  }
  else{
    tMapa[iPiso][iRow][iCol].derecha(true, &tMapa[iPiso][iRow][iCol+1]);
  }
  if(sensar->sensarIzquierda()){
    tMapa[iPiso][iRow][iCol-1].existe(true);
  }
  else{
    tMapa[iPiso][iRow][iCol].izquierda(true, &tMapa[iPiso][iRow][iCol-1]);
  }
  mover->Stop();
  //mapa.llenaMapa(tMapa, cDir, iCol, iRow, iPiso);
  while (mover->decidir(tMapa)) {
    mover->Stop();
    sensar->apantallanteLCD("R " + String(iRow) + " C " + String(iCol), "D " + String(cDir) + " ");
    delay(2000);
    mapa.llenaMapa(tMapa, cDir, iCol, iRow, iPiso);
  }
  sensar->escribirLCD("Let's go home");
  while(true){
    mover->goToVisitado(tMapa, 'i');
    mover->Stop();
    sensar->escribirLCD("Inicio");
  }
}


void loop() {
  // put your main code here, to run repeatedly:
}

void encoder1() {
  mover->encoder();
}
