#include "Arduino.h"
#include "Mapear.h"
#define ENCODER_A 4
#define ENCODER_B 5
Movimiento *mover;

void encoderAlfa() {
  mover->encoder();
}

void encoderBeta() {
  mover->encoder();
}

void setup() {
  Serial.begin(9600);
  Serial2.begin(115200);
  while(Serial2.available())
      Serial2.read();

  SensarRealidad *sensar = new SensarRealidad;
  mover = new Movimiento(185, 185, 10, sensar);
  Mapear mapa(sensar, mover);
  Tile tMapa[3][10][10];
  mover->Stop();

  attachInterrupt(ENCODER_A, encoderAlfa, RISING);
  sensar->apantallanteLCD("      El", "    MARIACHI");
  uint8_t iRow = 4, iCol = 4, iPiso = 0;
  char cDir = 'n';

  tMapa[iPiso][iCol][iRow].existe(true);
  tMapa[iPiso][iCol][iRow].inicio(true);
  tMapa[iPiso][iCol][iRow].visitado(true);

  if(sensar->sensarAtras())
    tMapa[iPiso][iRow + 1][iCol].existe(true);
  else
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
    sensar->apantallanteLCD("      HE","LLEGADO");
    delay(5000);
    sensar->apantallanteLCD("    V I V A", "  M E X I C O");
    break;
  }
}

void loop() {
}
