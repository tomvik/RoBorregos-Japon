/*/////////// Librerias ///////////
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
  attachInterrupt(1, boton1, RISING);

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

	if(digitalRead(BOTON_A))
		sensar->test();

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
}*/

#include <Wire.h>
#include <Adafruit_MLX90614.h>


#define sensorOut 2
#define S0 3
#define S1 4
#define S2 5
#define S3 6
#define LED 13

#define MlxL 0x5A
#define MlxR 0x4C

Adafruit_MLX90614 mlxRight = Adafruit_MLX90614(MlxR);
Adafruit_MLX90614 mlxLeft = Adafruit_MLX90614(MlxL);

char cSend, cLee, cEs, cPrueba = 0;
//2 es checkpoint
//1 es negro
//0 es blanco
uint8_t sensorR() {
	int frequency = 0;
	uint8_t iR = 0;
	digitalWrite(S2, HIGH);
	digitalWrite(S3, HIGH);
  digitalWrite(LED, HIGH);
	for (uint8_t i = 0; i < 3; i++){
		frequency += pulseIn(sensorOut, LOW);
	}
	frequency /= 3;
  //Serial.println(frequency);
  digitalWrite(LED, LOW);
  if(frequency >= 250)
    iR = 1;
  else if(frequency <= 70)
    iR = 2;
  return iR;
}
//0 = nada
//1 = derecha
//2 = izq
//3 = ambos
uint8_t sensarTemperatura() {
	uint8_t re = 0;
	if (mlxRight.readObjectTempC() > mlxRight.readAmbientTempC() + 4)
		re++;
	if (mlxLeft.readObjectTempC() > mlxLeft.readAmbientTempC() + 4)
		re+=2;
	return re;
}

void setup() {
	Serial3.begin(9600);
  //Serial.begin(9600);
  Serial2.begin(9600);
	mlxRight.begin();
	mlxLeft.begin();
	pinMode(S0, OUTPUT);
	pinMode(S1, OUTPUT);
	pinMode(S2, OUTPUT);
	pinMode(S3, OUTPUT);
	pinMode(sensorOut, INPUT);
  pinMode(LED, OUTPUT);

	digitalWrite(S0, HIGH);
	digitalWrite(S1, LOW);
}
//Color ////// 0 si es blanco, 1 si es negro, 2 si es checkpoint
//Temp  ////// 0 si no hay, 1 si está a la derecha, 2 si está a la izquierda
//0, 0, Letra, checkpoint,  color, izq, victima, der
void loop() {
  //Serial2.println("TOMA");
	cPrueba = cSend = cLee = 0;
  while(Serial2.available())
    cLee = (char)Serial2.read();
  if(cLee != 0){
    cEs = cLee;
  }
  switch(cEs){
    case 'L':
      cSend |= 0b00100000;
			Serial3.print(cSend);
		  delay(20);
			while(true) {
				cEs = (char)Serial2.read();
				if(cEs == 'H')
				return;
			}
  }
	switch(sensarTemperatura()) {
	case 1:
    //Serial.println("DER");
		cSend|=0b00000011;
		break;
	case 2:
    //Serial.println("Izq");
		cSend|=0b00000110;
		break;
	case 3:
		cSend|=0b00000111;
		break;
	}
 switch(sensorR()){
  case 1:
    //Serial.println("NEGRO");
    cSend |= 0b00001000;
    break;
  case 2:
    //Serial.println("CHECK");
    cSend |= 0b00010000;
    break;
 }
	Serial3.print(cSend);
  delay(20);
}
