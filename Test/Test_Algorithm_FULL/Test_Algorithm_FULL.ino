///////////Librerias////////////
#include <Arduino.h>
#include <Mapear.h>

//////////Encoders//////////////
#define ENCODER_A 4
#define ENCODER_B 5

/////////Variables, mapa y mover//////////
uint8_t iRow = 8, iCol = 9, iPiso = 0;
char cDir = 'n';
Tile tMapa[3][10][10];
Movimiento *mover;

///////////Apuntadores constantes a las variables/////////
uint8_t *const iR = &iRow;
uint8_t *const iC = &iCol;
uint8_t *const iP = &iPiso;
char *const cD = &cDir;

//////////////Funciones de encoders//////////////////
void encoder1() {
  mover->encoder1();
}

void encoder2() {
  mover->encoder2();
}
//////////////Funcion de impresión de mapa//////////////
void muestra(Tile tMapa[3][10][10], char cMapa[21][21], uint8_t iPiso){
  int iRowC = 1;
  for(int iRowT = 0; iRowT < 10; iRowT++) {
    int iColC = 1;
    for(int iColT = 0; iColT < 10; iColT++) {
      if(tMapa[iPiso][iRowT][iColT].inicio()) {
        cMapa[iRowC][iColC] = 'i';
      }
      else if(tMapa[iPiso][iRowT][iColT].visitado()) {
        cMapa[iRowC][iColC] = 'v';
      }
      else if(tMapa[iPiso][iRowT][iColT].existe()) {
        cMapa[iRowC][iColC] = 'n';
      }
      else{
        cMapa[iRowC][iColC] = 'x';
      }
      if(tMapa[iPiso][iRowT][iColT].arriba()) {
        cMapa[iRowC-1][iColC] = 'w';
      }
      else{
        cMapa[iRowC-1][iColC] = '.';
      }
      if(tMapa[iPiso][iRowT][iColT].derecha()) {
        cMapa[iRowC][iColC+1] = 'w';
      }
      else{
        cMapa[iRowC][iColC+1] = '.';
      }
      if(tMapa[iPiso][iRowT][iColT].abajo()) {
        cMapa[iRowC+1][iColC] = 'w';
      }
      else{
        cMapa[iRowC+1][iColC] = '.';
      }
      if(tMapa[iPiso][iRowT][iColT].izquierda()) {
        cMapa[iRowC][iColC-1] = 'w';
      }
      else{
        cMapa[iRowC][iColC-1] = '.';
      }
      iColC+=2;
    }
    iRowC+=2;
  }
  for(int i = 0; i < 21; i++) {
    for(int j = 0; j < 21; j++) {
      Serial.print(cMapa[i][j]);
      Serial.print(" ");
    }
    Serial.println(" ");
  }
  Serial.println(" ");
  delay(2000);
}



void setup() {
  //Serial
  Serial.begin(9600);
  Serial2.begin(115200);
  while(Serial2.available()) {
    Serial2.read();
  }

  //Interrupciones
  attachInterrupt(ENCODER_A, encoder1, RISING);
  attachInterrupt(ENCODER_B, encoder2, RISING);

  //Resto de los objetos
  SensarRealidad sensarr;
  SensarRealidad *const sensar = &sensarr;
  Movimiento robot(180, 180, sensar, cD, iC, iR, iP);
  mover = &robot;
  Mapear mapa(sensar, mover);

  //Mapa de caracteres y lo inicializamos
  char cMapa[21][21];
  for(int i = 0; i < 21; i++) {
    for(int j = 0; j < 21; j++)
      cMapa[i][j] = 'x';
  }
  //El Mariachi
  sensar->apantallanteLCD("      El", "    MARIACHI");
  mover->stop();

  //Inicializamos el tile actual
	tMapa[iPiso][iRow][iCol].inicio(true);
  tMapa[iPiso][iRow][iCol].visitado(true);
  tMapa[iPiso][iRow][iCol].existe(true);
	Serial.println("Primero ");
	Serial.print("Columna = "); Serial.print(iCol); Serial.print(" Row = "); Serial.print(iRow); Serial.print(" Piso = "); Serial.print(iPiso); Serial.print(" Direccion = "); Serial.println(cDir);
  muestra(tMapa, cMapa, iPiso);
	delay(1000);
  if(sensar->caminoAtras()) {
    tMapa[iPiso][iRow + 1][iCol].existe(true);
  } else {
    tMapa[iPiso][iRow][iCol].abajo(true, &tMapa[iPiso][iRow + 1][iCol]);
  }
  mapa.llenaMapaSensor(tMapa, cDir, iCol, iRow, iPiso);
  //MOSTRAR DÓNDE Y HACIA DÓNDE ESTÁ
  Serial.println("Primer sensado");
  Serial.print("Columna = "); Serial.print(iCol); Serial.print(" Row = "); Serial.print(iRow); Serial.print(" Piso = "); Serial.print(iPiso); Serial.print(" Direccion = "); Serial.println(cDir);
  muestra(tMapa, cMapa, iPiso);
  Serial.println("A COMENZAR EL LOOP");
  mover->stop();

  //Loop en el cual recorre todo el mapa
  while (mover->decidir(tMapa)) {
    mover->stop();
    mapa.llenaMapaSensor(tMapa, cDir, iCol, iRow, iPiso);
    Serial.print("Columna = "); Serial.print(iCol); Serial.print(" Row = "); Serial.print(iRow); Serial.print(" Piso = "); Serial.print(iPiso); Serial.print(" Direccion = "); Serial.println(cDir);
    muestra(tMapa, cMapa, iPiso);
  }
  Serial.println("SALI");
  delay(5000);

  //Se regresa al inicio
  sensar->apantallanteLCD("Let's go home");
	unsigned int i = 0;
	while(!tMapa[iPiso][iRow][iCol].inicio()) {
		mover->goToVisitado(tMapa, 'i');
		sensar->apantallanteLCD("ATORADO " + String(iCol) + " " + String(iRow), "ATORADO " + String(i++));
		muestra(tMapa, cMapa, iPiso);
  }
  Serial.print("Columna = "); Serial.print(iCol); Serial.print(" Row = "); Serial.print(iRow); Serial.print(" Piso = "); Serial.print(iPiso); Serial.print(" Direccion = "); Serial.println(cDir);
  muestra(tMapa, cMapa, iPiso);
  Serial.println("SALI");

  //Regresó al incio
  mover->stop();
  sensar->apantallanteLCD("      HE","    LLEGADO");
  delay(3500);
  sensar->apantallanteLCD("    V I V A", "  M E X I C O");
}

void loop() {
}
