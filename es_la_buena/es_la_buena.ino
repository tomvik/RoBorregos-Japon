#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Tile.h>
#include <SensarMapa.h>
#include <Movimiento.h>
#include <SensarRealidad.h>
#include <Mapear.h>

/*void muestra(Tile tMapa[3][10][10], char cMapa[21][21], uint8_t iPiso){
  int iRowC = 1;
  for(int iRowT = 0; iRowT < 10; iRowT++){
    int iColC = 1;
    for(int iColT = 0; iColT < 10; iColT++){
      if(tMapa[iPiso][iRowT][iColT].inicio()){
        cMapa[iRowC][iColC] = 'i';
      }
      else if(tMapa[iPiso][iRowT][iColT].visitado()){
        cMapa[iRowC][iColC] = 'v';
      }
      else if(tMapa[iPiso][iRowT][iColT].existe()){
        cMapa[iRowC][iColC] = 'n';
      }
      else{ 
        cMapa[iRowC][iColC] = 'x';
      }
      if(tMapa[iPiso][iRowT][iColT].arriba()){
        cMapa[iRowC-1][iColC] = 'w';
      }
      else{
        cMapa[iRowC-1][iColC] = '.';
      }
      if(tMapa[iPiso][iRowT][iColT].derecha()){
        cMapa[iRowC][iColC+1] = 'w';
      }
      else{
        cMapa[iRowC][iColC+1] = '.';
      }
      if(tMapa[iPiso][iRowT][iColT].abajo()){
        cMapa[iRowC+1][iColC] = 'w';
      }
      else{
        cMapa[iRowC+1][iColC] = '.';
      }
      if(tMapa[iPiso][iRowT][iColT].izquierda()){
        cMapa[iRowC][iColC-1] = 'w';
      }
      else{
        cMapa[iRowC][iColC-1] = '.';
      }
      iColC+=2;
    }
    iRowC+=2;
  }
  for(int i = 0; i < 21; i++){
    for(int j = 0; j < 21; j++){
      Serial.print(cMapa[i][j]);
      Serial.print(" ");
    }
    Serial.println(" ");
  }
  Serial.println(" ");
  delay(2000);
  }*/
Movimiento *mover;
void setup() {
  Serial.begin(9600);
  attachInterrupt(5, encoder1, RISING);
  
  SensarRealidad sensarr;
  SensarRealidad *sensar = &sensarr;
  Movimiento robot(150, 150, 10, sensar);
  mover = &robot;
  Mapear mapa(sensar, mover);
  Tile tMapa[3][10][10];
  //char cMapa[21][21];
  uint8_t iRow = 0, iCol = 0, iPiso = 0 ;
  char cDir = 'n';
  tMapa[iPiso][iCol][iRow].inicio(true);
  tMapa[iPiso][iCol][iRow].visitado(true);
  tMapa[iPiso][iCol][iRow].existe(true);
  //INICIALIZAR MAPA VISUAL
  /*for(int i = 0; i < 21; i++){
    for(int j = 0; j < 21; j++)
      cMapa[i][j] = 'x';
    }*/
  delay(5000);
  Serial.println("HOLA");
  //MOSTRAR DÓNDE Y HACIA DÓNDE ESTÁ
  /*Serial.print("Columna = "); Serial.print(iCol); Serial.print(" Row = "); Serial.print(iRow); Serial.print(" Piso = "); Serial.print(iPiso); Serial.print(" Direccion = "); Serial.println(cDir);
    muestra(tMapa, cMapa, iPiso);*/
  //GIRAR A LA DERECHA Y MAPEA
  while(true){
    mover->avanzar(iCol, iRow, cDir, tMapa, iPiso);
    delay(2000);
  }
}


void loop() {
  // put your main code here, to run repeatedly:

}

void encoder1() {
  mover->encoder();
}
