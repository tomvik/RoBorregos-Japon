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

char cSendMega, cSendRaspD, cSendRaspI, cLeeRasp, cLeeMega = 0;
bool bLetra = true, bPorfavor = true;
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
  else if(frequency <= 55)
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
  Serial.begin(9600);
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
/*
//Serial
  0     compu
  2     rasp derecha
  3     mega
//Color
  0 si es blanco
  1 si es negro
  2 si es checkpoint
//Temp  
  0 si no hay
  1 si está a la derecha
  2 si está a la izquierda
//cSendMega   //   H/Letra, S/Letra, U/Letra, checkpoint,  color, izq, victima/Letra, der
///cSendRaspD e I   //   Busca, Identifica
///cLeeMega    //   Enviar (E), Identifica (I), Busca (B)
//TODO duplicar todo para la otra rasp
*/
void loop() {
  cSendMega = 0;
  cLeeRasp = 'N';
  ////////////////////Lee Serial Mega
  while(Serial3.available()){
    cLeeMega = (char)Serial3.read();
  }
  ////////////////////Lee Serial Rasp Derecha
  while(Serial2.available()){
    cLeeRasp = (char)Serial2.read();
  }
  ////////////////////Letra
  switch(cLeeRasp){
    case 'L':
      //Serial.println("Letra");
      cSendMega |= 0b11100110;
      break;
    case 'H':
      //Serial.println("H");
      cSendMega |= 0b10000000;
      break;
    case 'S':
      //Serial.println("S");
      cSendMega |= 0b01000000;
      break;
    case 'U':
      //Serial.println("U");
      cSendMega |= 0b00100000;
      break;
  }
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
    case 2:
      //Serial.println("CHECK");
      cSendMega |= 0b00010000;
      break;
  }
  ////////////////////Serial
  switch(cLeeMega){
    case 'E':
      Serial3.print(cSendMega);
      break;
    case 'I':
      Serial2.println("Identifica");
      break;
    case 'B':
      Serial2.println("Busca");
      break;
  }
}