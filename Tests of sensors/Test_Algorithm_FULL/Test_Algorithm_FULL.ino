/////////// Librerias ///////////
#include <Arduino.h>
#include <Mapear.h>

/////////// Encoders e interrupciones///////////
#define BOTON_A 1
#define ENCODER_A 4
#define ENCODER_B 5

/////////// Variables, mapa y mover ///////////
uint8_t iRow = 4, iCol = 4, iPiso = 0;
char cDir = 'n';
Tile tMapa[3][10][10];
Movimiento *mover;

///////////Variables para checkpoint//////////
uint8_t iRowL = iRow, iColL = iCol, iPisoL = iPiso;
char cDirL = cDir;
Tile tBueno[3][10][10];

/////////// Apuntadores constantes a las variables ///////////
uint8_t *const iR = &iRow;
uint8_t *const iC = &iCol;
uint8_t *const iP = &iPiso;
char *const cD = &cDir;

uint8_t *const iRL = &iRowL;
uint8_t *const iCL = &iColL;
uint8_t *const iPL = &iPisoL;
char *const cDL = &cDirL;

/////////// Funciones de encoders y boton ///////////
void encoder1() {
  mover->encoder1();
}

void encoder2() {
  mover->encoder2();
}

void boton1() {
  mover->boton1();
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

  // Interrupciones
  attachInterrupt(ENCODER_A, encoder1, RISING);
  attachInterrupt(ENCODER_B, encoder2, RISING);
  attachInterrupt(BOTON_A, boton1, RISING);

  // Resto de los objetos
  SensarRealidad sensarr;
  SensarRealidad *const sensar = &sensarr;

  // El Mariachi
  sensar->apantallanteLCD("      El", "    MARIACHI");

  // Resto de los objetos
  Movimiento robot(175, 175, sensar, cD, iC, iR, iP, cDL, iCL, iRL, iPL, tBueno, tMapa);
  mover = &robot;
  Mapear mapa(sensar, mover, cDL, iCL, iRL, iPL);
  mover->stop();

  //Mapa de caracteres y lo inicializamos
  char cMapa[21][21];
  for(int i = 0; i < 21; i++) {
    for(int j = 0; j < 21; j++)
      cMapa[i][j] = 'x';
  }

  //Inicializamos el tile actual
	tMapa[iPiso][iRow][iCol].inicio(true);
  tMapa[iPiso][iRow][iCol].visitado(true);
  tMapa[iPiso][iRow][iCol].existe(true);
  if(sensar->caminoAtras()) {
    tMapa[iPiso][iRow + 1][iCol].existe(true);
  } else {
    tMapa[iPiso][iRow][iCol].abajo(true, &tMapa[iPiso][iRow + 1][iCol]);
  }
  mapa.llenaMapaSensor(tMapa, tBueno, cDir, iCol, iRow, iPiso);
  //MOSTRAR DÓNDE Y HACIA DÓNDE ESTÁ
  Serial.println("Primer sensado");
  Serial.print("Columna = "); Serial.print(iCol); Serial.print(" Row = "); Serial.print(iRow); Serial.print(" Piso = "); Serial.print(iPiso); Serial.print(" Direccion = "); Serial.println(cDir);
  muestra(tMapa, cMapa, iPiso);
  Serial.println("A COMENZAR EL LOOP");
  mover->stop();

  //Loop en el cual recorre todo el mapa
  while (mover->decidir()) {
    mover->stop();
    if(mover->getLack())
      mapa.llenaMapaSensor(tMapa, tBueno, cDir, iCol, iRow, iPiso);
    else
      mapa.llenaMapaVariable(tMapa, tBueno, cDir, iCol, iRow, iPiso);
    Serial.print("Columna = "); Serial.print(iCol); Serial.print(" Row = "); Serial.print(iRow); Serial.print(" Piso = "); Serial.print(iPiso); Serial.print(" Direccion = "); Serial.println(cDir);
    muestra(tMapa, cMapa, iPiso);
    Serial.println("CHIDO ");
    Serial.print("Columna = "); Serial.print(iColL); Serial.print(" Row = "); Serial.print(iRowL); Serial.print(" Piso = "); Serial.print(iPisoL); Serial.print(" Direccion = "); Serial.println(cDirL);
    muestra(tBueno, cMapa, iPiso);
  }
  Serial.println("SALI");
  delay(5000);

  //Se regresa al inicio
  sensar->apantallanteLCD("Let's go home");
	while(!tMapa[iPiso][iRow][iCol].inicio()) {
		mover->goToVisitado('i');
		muestra(tMapa, cMapa, iPiso);
  }
  Serial.print("Columna = "); Serial.print(iCol); Serial.print(" Row = "); Serial.print(iRow); Serial.print(" Piso = "); Serial.print(iPiso); Serial.print(" Direccion = "); Serial.println(cDir);
  muestra(tMapa, cMapa, iPiso);
  Serial.println("CHIDO ");
  Serial.print("Columna = "); Serial.print(iColL); Serial.print(" Row = "); Serial.print(iRowL); Serial.print(" Piso = "); Serial.print(iPisoL); Serial.print(" Direccion = "); Serial.println(cDirL);
  muestra(tBueno, cMapa, iPiso);
  Serial.println("SALI");

  //Regresó al incio
  mover->stop();
  sensar->apantallanteLCD("      HE","    LLEGADO");
  delay(1500);
  sensar->apantallanteLCD("    V I V A", "  M E X I C O");
}

void loop() {
}
