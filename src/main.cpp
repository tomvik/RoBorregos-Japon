/*/////////// Librerias ///////////
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

	if(digitalRead(BOTON_B) == LOW)
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
}*/


#include <Wire.h>
#include <Adafruit_MLX90614.h>

//Pines sensor y led
#define sensorOut 2
#define S0 3
#define S1 4
#define S2 5
#define S3 6
#define LED 13
//MLX adress
#define MlxL 0x1C
#define MlxR 0x5C
//Declaramos los objetos
Adafruit_MLX90614 mlxRight = Adafruit_MLX90614(MlxR), mlxLeft = Adafruit_MLX90614(MlxL);
//Variables para control de lecturas
char cSendMega, cSendRaspD, cSendRaspI, cLeeRaspD, cLeeRaspI, cLeeMega = 0;
bool bID = true, bII = true, bIoD;
//2 es checkpoint
//1 es negro
//0 es blanco
uint8_t sensorColor() {
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
  if(frequency >= 120)
    iR = 1;
  return iR;
}
//0 = nada
//1 = derecha
//2 = izq
//3 = ambos
uint8_t sensarTemperatura() {
  uint8_t re = 0;
  if (mlxRight.readObjectTempC() > mlxRight.readAmbientTempC() + 2.75)
    re++;
  if (mlxLeft.readObjectTempC() > mlxLeft.readAmbientTempC() + 2.75)
    re+=2;
  //Serial.print("Derecha: "); Serial.print((mlxRight.readObjectTempC()); Serial.print("Izquierda: "); Serial.println((mlxLeft.readObjectTempC());
  return re;
}

void queDijo(char c, bool &b, bool d){
  switch(c){
    case 'L':
      b = true;
      Serial.println("Letra");
      if(d)
        cSendMega |= 0b11100011;
      else
        cSendMega |= 0b11100110;
      break;
    case 'H':
      b = true;
      Serial.println("H");
      cSendMega = 0b10000000;
      Serial3.print(cSendMega);
      break;
    case 'S':
      b = true;
      Serial.println("S");
      cSendMega = 0b01000000;
      Serial3.print(cSendMega);
      break;
    case 'U':
      b = true;
      Serial.println("U");
      cSendMega = 0b00100000;
      Serial3.print(cSendMega);
      break;
    case 'N':
      b = true;
      Serial.println("N");
      cSendMega = 0b00010000;
      Serial3.print(cSendMega);
      break;
  }
}

void setup() {
  //Serial
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  //Iniciamos mlx
  mlxRight.begin();
  mlxLeft.begin();
  //Definimos pines
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  pinMode(LED, OUTPUT);
  //Los ponemosen un estado
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  //Ponemos a las rasp en buscar (no es necesario)
  Serial1.println("Busca");
  Serial2.println("Busca");
  //Serial.println("porfas");
}
/*
//Serial
  0     compu
  1     rasp izquierda
  2     rasp derecha
  3     mega
//Color
  0 si es blanco
  1 si es negro
//Temp
  0 si no hay
  1 si está a la derecha
  2 si está a la izquierda
//cSendMega   //   H/Letra, S/Letra, U/Letra, NADA,  color, izq, victima/Letra, der
///cSendRaspD e I   //   Busca, Identifica
///cLeeMega    //////
  Mandar (M)
  Derecha:    Identifica (I), Busca (B);
  Izquierda:  Reconocer (R), Encuentra (E);
///cLeeRasp
  Letra (L), H, S, U, No hay letra (N), No recibió nada (W)
//TODO duplicar todo para la otra rasp
*/
void loop() {
  //Ponemos en 0 lo que enviaremos al mega
  cSendMega = 0;
  //Si detectó una letra, no se calla hasta que lo escuchen
  cLeeRaspD = bID ? cLeeRaspD : 'W';
  cLeeRaspI = bII ? cLeeRaspI : 'W';
  //Si el mega le dice que identifique, identifica hasta nuevo aviso
  cLeeMega = (cLeeMega == 'I') ? 'I' : 'N';
  cLeeMega = (cLeeMega == 'R') ? 'R' : 'N';
  ////////////////////Lee Serial Mega
  while(Serial3.available()){
    cLeeMega = (char)Serial3.read();
  }
  ////////////////////Lee Serial Rasp Izquierda
  while(Serial1.available()){
    cLeeRaspI = (char)Serial1.read();
  }
  ////////////////////Lee Serial Rasp Derecha
  while(Serial2.available()){
    cLeeRaspD = (char)Serial2.read();
  }
  ////////////////////Letra Derecha
  queDijo(cLeeRaspD, bID, true);
  ////////////////////Letra Izquierda
  queDijo(cLeeRaspI, bII, false);
  ////////////////////Temperatura
  switch(sensarTemperatura()) {
    case 1:
      //Serial.println("DER");
      cSendMega|=0b00000011;
      break;
    case 2:
      //Serial.println("Izq");
      cSendMega|=0b00000110;
      break;
    case 3:
      //Serial.println("Amb");
      cSendMega|=0b00000111;
      break;
  }
  ////////////////////Color
  switch(sensorColor()){
    case 1:
      //Serial.println("NEGRO");
      cSendMega |= 0b00001000;
      break;
  }
  ////////////////////Serial
  switch(cLeeMega){
    case 'I':
      bID = false;
      Serial2.println("Identifica");
      if(cLeeRaspD != 'W' && cLeeRaspD != 'L')
        Serial3.print(cSendMega);
      break;
    case 'B':
      bID = false;
      //Serial.println("BUSCA");
      Serial2.println("Busca");
      break;
    case 'M':
      //Serial.println("MANDA");
      Serial3.print(cSendMega);
      break;
    case 'R':
      bII = false;
      Serial1.println("Identifica");
      if(cLeeRaspI != 'W' && cLeeRaspI != 'L')
        Serial3.print(cSendMega);
      break;
    case 'E':
      //Serial.println("BUSCA");
      Serial1.println("Busca");
      break;
    default:
      Serial3.print(cSendMega);
      break;
  }
}
