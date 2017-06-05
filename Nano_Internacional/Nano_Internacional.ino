#include <Wire.h>
#include <Adafruit_MLX90614.h>

#define S0 2
#define S1 3
#define S2 4
#define S3 5
#define sensorOut 6

#define MlxR 0x1C
#define MlxL 0x4C

Adafruit_MLX90614 mlxRight = Adafruit_MLX90614(MlxR);
Adafruit_MLX90614 mlxLeft = Adafruit_MLX90614(MlxL);

unsigned long tiempoYa, tiempoAntes;
uint8_t utemp;
char cSend;
//1 es negro
//0 es blanco
uint8_t sensorR() {
  int frequency = 0;
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  for (uint8_t i = 0; i < 5; i++)
    frequency += pulseIn(sensorOut, LOW);
  frequency /= 5;
  if (frequency >= 120)
    return 1;
  return 0;
}
//0 = nada
//1 = derecha
//2 = izq
//3 = ambos
uint8_t sensarTemperatura() {
  uint8_t re = 0;
  if (mlxRight.readObjectTempC() > mlxRight.readAmbientTempC() + 3)
    re++;
  if (mlxLeft.readObjectTempC() > mlxLeft.readAmbientTempC() + 3)
    re+=2;
  return re;
}

void setup() {
  Serial.begin(9600);
  mlxRight.begin();
  mlxLeft.begin();
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  //for(int i= 0; i< 100; i++){
  // digitalWrite(victimaOut, LOW);
  // digitalWrite(colorOut, LOW); 
  //}
}
//Color
//0 si es blanco, 1 si es negro
//Temp
//0 si no hay, 1 si está a la derecha, 2 si está a la izquierda
//0, 0, 0, 0,  color, izq, victima, der
void loop() {
  cSend = 0;
  utemp = sensarTemperatura();
  switch(utemp){
    case 1:
      cSend|=0b00000011;
      break;
    case 2:
      cSend|=0b00000110;
      break;
    case 3:
      cSend|=0b00000111;
      break;
  }
  if(sensorR() == 1){
    cSend|=0b00001000;
  }
  Serial.print(cSend);
  /*
  if (sensarTemperatura() > 0) {
    digitalWrite(victimaOut, HIGH);
    tiempoAntes = millis();
    while (digitalRead(megaIn) == 0 && tiempoAntes + 200 > tiempoYa) {
      tiempoYa = millis();
    }
    if (tiempoAntes + 200 > tiempoYa) {
      //Serial.println("ENTRO");
      digitalWrite(victimaOut, LOW);
      digitalWrite(colorOut, LOW);
      if (sensarTemperatura() == 1) {
        digitalWrite(victimaOut, HIGH);
      }
      else {
        digitalWrite(colorOut, HIGH);
      }
      delay(100);
      digitalWrite(victimaOut, LOW);
      digitalWrite(colorOut, LOW);
    }
    digitalWrite(victimaOut, LOW);
    digitalWrite(colorOut, LOW);
  }
  else{
    digitalWrite(victimaOut, LOW);
  }
  if (sensorR() == 1) {
    digitalWrite(colorOut, HIGH);
  }
  else {
    digitalWrite(colorOut, LOW);
  }*/
}
//MEGA
//Color 34
//Temp 33
//Escribir 32
