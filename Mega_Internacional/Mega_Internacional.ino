/////////// Librerias ///////////
#include <Arduino.h>
#include <Mapear.h>

/////////// Encoders e interrupciones///////////
#define BOTON_A 3
#define ENCODER_A 4
#define ENCODER_B 5
///////////Dimensiones///////////////////
const uint8_t kMapSize = 10;
const uint8_t kMapFloors = 3;

/////////// Variables, mapa y mover ///////////
uint8_t iRow = 4, iCol = 4, iPiso = 0, iPisoMax = 0;
char cDir = 'n';
Tile tMapa[kMapFloors][kMapSize][kMapSize];
Movimiento *mover;

///////////Variables para checkpoint//////////
uint8_t iRowL = iRow, iColL = iCol, iPisoL = iPiso, iPisoMaxLast = 0;
char cDirL = cDir;
Tile tBueno[kMapFloors][kMapSize][kMapSize], tBackUp[kMapFloors][kMapSize][kMapSize];

/////////// Apuntadores constantes a las variables ///////////
uint8_t *const iR = &iRow;
uint8_t *const iC = &iCol;
uint8_t *const iP = &iPiso;
uint8_t *const iPM = &iPisoMax;
char *const cD = &cDir;

uint8_t *const iRL = &iRowL;
uint8_t *const iCL = &iColL;
uint8_t *const iPL = &iPisoL;
uint8_t *const iPML = &iPisoMaxLast;
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


void setup() {
	//Serial
	Serial2.begin(9600);
	Serial.begin(115200);
	while(Serial.available()) {
		Serial.read();
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
	Movimiento robot(160, 160, sensar, cD, iC, iR, iP, cDL, iCL, iRL, iPL, tBueno, tMapa, iPM, iPML);
	mover = &robot;
	Mapear mapa(sensar, mover, cDL, iCL, iRL, iPL, iPM, iPML);
	mover->stop();

	// if(digitalRead(BOTON_A))
		// sensar->test();

	//Inicializamos el tile actual
	tMapa[iPiso][iRow][iCol].inicio(true);
	tMapa[iPiso][iRow][iCol].visitado(true);
	tMapa[iPiso][iRow][iCol].existe(true);
  tMapa[iPiso][iRow][iCol].checkpoint(true);
	if(sensar->caminoAtras()) {
		tMapa[iPiso][iRow + 1][iCol].existe(true);
	} else {
		tMapa[iPiso][iRow][iCol].abajo(true, &tMapa[iPiso][iRow + 1][iCol]);
	}

	mapa.llenaMapaSensor(tMapa, tBueno, cDir, iCol, iRow, iPiso);

	// Loop en el cual recorre todo el mapa
	while (mover->decidir()) {
		mover->stop();
    if(mover->getLack()){
      prueba:
      mapa.llenaMapaSensor(tMapa, tBueno, cDir, iCol, iRow, iPiso);
    }
    else
		  mapa.llenaMapaVariable(tMapa, tBueno, cDir, iCol, iRow, iPiso);
	}

	// Se regresa al inicio
	sensar->apantallanteLCD("Let's go home");
  mover->stop();
  for(int i = 0; i < kMapFloors; i++) {
    for(int j = 0; j < kMapSize; j++) {
      for(int z = 0; z < kMapSize; z++) {
        tBackUp[i][j][z] = tBueno[i][j][z];
      }
    }
  }
	while(!tMapa[iPiso][iRow][iCol].inicio())
		mover->goToVisitado('i');

	// Regresó al incio
	mover->stop();
	sensar->apantallanteLCD("      HE","    LLEGADO");
	delay(1500);
	sensar->apantallanteLCD("    V I V A", "  M E X I C O");

  while(true){
    if(mover->getLackReal()){
      for(int i = 0; i < kMapFloors; i++) {
        for(int j = 0; j < kMapSize; j++) {
          for(int z = 0; z < kMapSize; z++) {
            tBueno[i][j][z] = tBackUp[i][j][z];
          }
        }
      }
      goto prueba;
    }
  }
}

void loop() {
}
