/////////// Librerias ///////////
#include <Arduino.h>
#include <Mapear.h>

/////////// Encoders e interrupciones ///////////
#define BOTON_A 1
#define BOTON_B 31
#define ENCODER_A 4
#define ENCODER_B 5

/////////// Dimensiones ///////////////////
const uint8_t kMapSize = 15;
const uint8_t kMapFloors = 4;

/////////// Variables, mapa y mover ///////////
uint8_t iRow = 7, iCol = 7, iPiso = 0, iPisoMax = 0;
char cDir = 'n';
Tile tMapa[kMapFloors][kMapSize][kMapSize];
Movimiento *mover;

/////////// Apuntadores constantes a las variables ///////////
uint8_t *const iR = &iRow;
uint8_t *const iC = &iCol;
uint8_t *const iP = &iPiso;
uint8_t *const iPM = &iPisoMax;
char *const cD = &cDir;

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
	// Serial
	Serial2.begin(9600);
	Serial.begin(9600);
	while(Serial.available())
		Serial.read();

	// Interrupciones
	attachInterrupt(ENCODER_A, encoder1, RISING);
	attachInterrupt(ENCODER_B, encoder2, RISING);
  attachInterrupt(BOTON_A, boton1, RISING);
	pinMode(BOTON_B, INPUT);

	// Resto de los objetos
	SensarRealidad sensarr;
	SensarRealidad *const sensar = &sensarr;

	// El Mariachi
	sensar->apantallanteLCD("      El", "    MARIACHI");

	// Resto de los objetos
	Movimiento robot(160, 160, sensar, cD, iC, iR, iP, tMapa, iPM);
	mover = &robot;
	Mapear mapa(sensar, mover, iPM);
	mover->stop();

	if(digitalRead(BOTON_B) == LOW) {
		sensar->test();
		sensar->escribirLCD("      K I T");
		delay(1000);
		mover->dejarKit(1);
		delay(1000);
		mover->dejarKit(2);
		delay(5000);
	}

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

	mapa.llenaMapaSensor(tMapa, cDir, iCol, iRow, iPiso);
	// Loop en el cual recorre todo el mapa
	while (mover->decidir()) {
		mover->stop();
		mapa.llenaMapaVariable(tMapa, cDir, iCol, iRow, iPiso);
	}
	// Se regresa al inicio
	sensar->apantallanteLCD("Let's go home");
	while(!tMapa[iPiso][iRow][iCol].inicio())
    mover->goToVisitado('i');

	// Regresó al incio
	mover->stop();
	sensar->apantallanteLCD("      HE","    LLEGADO");
	delay(1000);
	sensar->apantallanteLCD("    V I V A", "  M E X I C O");
}

void loop() {
}
