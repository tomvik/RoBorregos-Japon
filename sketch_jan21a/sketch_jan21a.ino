#include <Tile.h>
#include <SensarMapa.h>
#include <Movimiento.h>
#include <SensarRealidad.h>
#include <Mapear.h>


void setup() {
  //put your setup code here, to run once:
  Serial.begin(9600);
  Tile tMapa[10][10];
  uint8_t iRow = 5, iCol = 5;
  char cDir = 'n';

  Movimiento ro;
  tMapa[iRow][iCol].visitado(true);
  tMapa[5][5].derecha(true, &tMapa[5][6]);
  tMapa[5][6].arriba(true, &tMapa[4][6]);
  tMapa[4][6].derecha(true, &tMapa[4][7]);
  tMapa[4][6].arriba(true, &tMapa[3][6]);
  Serial.print(iCol); Serial.print(" "); Serial.print(iRow); Serial.println(cDir);
  ro.decidir(tMapa, iCol, iRow, cDir);
  Serial.print(iCol); Serial.print(" "); Serial.print(iRow); Serial.println(cDir);
  ro.decidir(tMapa, iCol, iRow, cDir);
  Serial.print(iCol); Serial.print(" "); Serial.print(iRow); Serial.println(cDir);
  ro.decidir(tMapa, iCol, iRow, cDir);
  Serial.print(iCol); Serial.print(" "); Serial.print(iRow); Serial.println(cDir);
}


void loop() {
  // put your main code here, to run repeatedly:

}
