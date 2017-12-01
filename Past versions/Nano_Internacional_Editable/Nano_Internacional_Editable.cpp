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
  if (mlxRight.readObjectTempC() > mlxRight.readAmbientTempC() + 4)
    re++;
  if (mlxLeft.readObjectTempC() > mlxLeft.readAmbientTempC() + 4)
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
      Serial.println("H");
      Serial2.print("H");
      cSendMega |= 0b10000000;
      break;
    case 'S':
      Serial.println("S");
      Serial2.println("S");
      cSendMega |= 0b01000000;
      break;
    case 'U':
      Serial.println("U");
      Serial2.println("U");
      cSendMega |= 0b00100000;
      break;
    case 'N':
      Serial.println("N");
      Serial2.println("N");
      cSendMega |= 0b00010000;
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