#include <Wire.h>
#include <Adafruit_MLX90614.h>

#define S0 2
#define S1 3
#define S2 4
#define S3 5
#define sensorOut 6
#define victimaOut 8
#define colorOut 9
#define megaIn 7

#define MlxR 0x1C
#define MlxL 0x4C

Adafruit_MLX90614 mlxRight = Adafruit_MLX90614(MlxR);
Adafruit_MLX90614 mlxLeft = Adafruit_MLX90614(MlxL);

unsigned long tiempoYa, tiempoAntes;

uint8_t sensorR() {
  int frequency;

  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  frequency = 0;
  for (uint8_t i = 0; i < 5; i++) {
    frequency += pulseIn(sensorOut, LOW);
  }
  frequency /= 5;
  //Serial.println(frequency);
  if (frequency >= 120)
    return 1;
  return 0;
}

uint8_t sensarTemperatura() {
  Serial.print("RIGHT: "); Serial.print(mlxRight.readObjectTempC()); Serial.print(" LEFT: "); Serial.println(mlxLeft.readObjectTempC());
  if (mlxRight.readObjectTempC() > mlxRight.readAmbientTempC() + 3) {
    return 1;
  }
  if (mlxLeft.readObjectTempC() > mlxLeft.readAmbientTempC() + 3)
    return 2;
  return 0;
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

  pinMode(victimaOut, OUTPUT);
  pinMode(colorOut, OUTPUT);
  pinMode(megaIn, INPUT);

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  for(int i= 0; i< 100; i++){
   digitalWrite(victimaOut, LOW);
   digitalWrite(colorOut, LOW); 
  }
}
//Color
//0 si es blanco, 1 si es negro
//Temp
//0 si no hay, 1 si está a la derecha, 2 si está a la izquierda
void loop() {
  if (sensarTemperatura() > 0) {
    digitalWrite(victimaOut, HIGH);
    tiempoAntes = millis();
    while (digitalRead(megaIn) == 0 && tiempoAntes + 200 > tiempoYa) {
      tiempoYa = millis();
      Serial.println("ESTOY ATORADO");
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
    Serial.println("HOLLA");
  }
  else {
    digitalWrite(colorOut, LOW);
  }
}
//MEGA
//Color 34
//Temp 33
//Escribir 32
