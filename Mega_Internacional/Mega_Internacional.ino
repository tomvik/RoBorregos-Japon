/* Code made by the RoBorregos team in 2017 for the RoboCup JR. Rescue Maze category.
 * Tomás Lugo, Sebastián Esquer, Ernesto Cervantez, and Alexis Virgen.
 * "El Mariachi" Achieved a third place on the international RoboCup.
 */
/////////// Libraries ///////////
#include <Arduino.h>
#include <Mapear.h>

/////////// Encoders and Interrupts ///////////
#define BOTON_A 1
#define BOTON_B 31
#define ENCODER_A 4
#define ENCODER_B 5

/////////// Constants to modify de dimensions of the map ///////////////////
const uint8_t kMapSize = 15;
const uint8_t kMapFloors = 4;

/////////// Variables of location, Creating the map and the "Move" pointer ///////////
uint8_t iRow = 7, iCol = 7, iPiso = 0, iPisoMax = 0;
char cDir = 'n';
Tile tMapa[kMapFloors][kMapSize][kMapSize];
Movimiento *mover;

/////////// Constant Pointers to the variables of location ///////////
uint8_t *const iR = &iRow;
uint8_t *const iC = &iCol;
uint8_t *const iP = &iPiso;
uint8_t *const iPM = &iPisoMax;
char *const cD = &cDir;

/////////// Interrupts functions, encoders and reset button ///////////
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
	// Serial Communication
	Serial2.begin(9600);
	Serial.begin(9600);
  // Clean the bus
	while(Serial.available())
		Serial.read();

	// Initialize the interrupts and another button
	attachInterrupt(ENCODER_A, encoder1, RISING);
	attachInterrupt(ENCODER_B, encoder2, RISING);
  attachInterrupt(BOTON_A, boton1, RISING);
	pinMode(BOTON_B, INPUT);

	// Initialize the object to sense reality (All the sensors)
  // Could be done like SensarRealidad *const sensar = new SensarRealidad ? The problem may be that dynamic memory is used instead.
	SensarRealidad sensarr;
	SensarRealidad *const sensar = &sensarr;

	// El Mariachi, this message is to know that it initialized correctly (Several bugs with the i2c of the sensors... need to fix that)
	sensar->apantallanteLCD("      El", "    MARIACHI");

	// The other objects, "moving" and of the mapping.
	Movimiento robot(160, 160, sensar, cD, iC, iR, iP, tMapa, iPM);
	mover = &robot;
	Mapear mapa(sensar, mover, iPM);
  
  //We stop the motors at first, just in case
	mover->stop();
  //If the button is pressed, we enter the testing mode (Really useful)
	if(digitalRead(BOTON_B) == LOW)
		sensar->test();

	//Initializing of the map
	tMapa[iPiso][iRow][iCol].inicio(true);
	tMapa[iPiso][iRow][iCol].visitado(true);
	tMapa[iPiso][iRow][iCol].existe(true);
  tMapa[iPiso][iRow][iCol].checkpoint(true);
  //Checking the sensor on the back
	if(sensar->caminoAtras()) {
		tMapa[iPiso][iRow + 1][iCol].existe(true);
	} else {
		tMapa[iPiso][iRow][iCol].abajo(true, &tMapa[iPiso][iRow + 1][iCol]);
	}
 
  //It doesn't finish this loop until it's comming to the beginning
	mapa.llenaMapaSensor(tMapa, cDir, iCol, iRow, iPiso);
	while (mover->decidir()) {
		mover->stop();
		mapa.llenaMapaVariable(tMapa, cDir, iCol, iRow, iPiso);
	}
	// Returns to the beginning
	sensar->apantallanteLCD("Let's go home");
	while(!tMapa[iPiso][iRow][iCol].inicio())
    mover->goToVisitado('i');

	// It made it
	mover->stop();
	sensar->apantallanteLCD("      HE","    LLEGADO");
	delay(1000);
	sensar->apantallanteLCD("    V I V A", "  M E X I C O");
  //Just in case.
  delay(5000);
}

void loop() {
}
