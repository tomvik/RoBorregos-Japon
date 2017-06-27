#include <Arduino.h>
#include <Mapear.h>
#define ENCODER_A 4
#define ENCODER_B 5

uint8_t iRow = 4, iCol = 4, iPiso = 0;
uint8_t *iR = &iRow, *iC = &iCol, *iP = &iPiso;
char cDir = 'n';
char *cD = &cDir;
Movimiento *mover;

void encoder1() {
	mover->encoder1();
}

void encoder2() {
	mover->encoder2();
}

void setup() {
	Serial.begin(9600);
	Serial2.begin(115200);
	while(Serial2.available()) {
		Serial2.read();
	}
	attachInterrupt(ENCODER_A, encoder1, RISING);
	attachInterrupt(ENCODER_B, encoder2, RISING);
	SensarRealidad sensarr;
	SensarRealidad *sensar = &sensarr;
	sensar->apantallanteLCD("      El", "    MARIACHI");
	Movimiento robot(180, 180, sensar, cD, iC, iR, iP);
	mover = &robot;
	Mapear mapa(sensar, mover);
	Tile tMapa[3][10][10];

	tMapa[iPiso][iCol][iRow].inicio(true);
	tMapa[iPiso][iCol][iRow].visitado(true);
	tMapa[iPiso][iCol][iRow].existe(true);

	if(sensar->sensarAtras()) {
		tMapa[iPiso][iRow + 1][iCol].existe(true);
	} else {
		tMapa[iPiso][iRow][iCol].abajo(true, &tMapa[iPiso][iRow + 1][iCol]);
	}
	if(sensar->sensarEnfrente()) {
		tMapa[iPiso][iRow - 1][iCol].existe(true);
	} else {
		tMapa[iPiso][iRow][iCol].arriba(true, &tMapa[iPiso][iRow - 1][iCol]);
	}
	if(sensar->sensarDerecha()) {
		tMapa[iPiso][iRow][iCol + 1].existe(true);
	} else {
		tMapa[iPiso][iRow][iCol].derecha(true, &tMapa[iPiso][iRow][iCol + 1]);
	}
	if(sensar->sensarIzquierda()) {
		tMapa[iPiso][iRow][iCol - 1].existe(true);
	} else {
		tMapa[iPiso][iRow][iCol].izquierda(true, &tMapa[iPiso][iRow][iCol - 1]);
	}
	mover->stop();
	while (mover->decidir(tMapa)) {
		mover->stop();
		sensar->escribirLCD("R " + String(iRow) + " C " + String(iCol), "D " + String(cDir) + " ");
		// delay(2000);
		mapa.llenaMapa(tMapa, cDir, iCol, iRow, iPiso);
	}
	sensar->apantallanteLCD("Let's go home");
	while(true) {
		mover->goToVisitado(tMapa, 'i');
		mover->stop();
		sensar->apantallanteLCD("      HE","LLEGADO");
		delay(5000);
		sensar->apantallanteLCD("    V I V A", "  M E X I C O");
		break;
	}
}

void loop() {
}
