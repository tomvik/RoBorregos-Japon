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

char cSend, cLee, cEs = 0;
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
//Color ////// 0 si es blanco, 1 si es negro, 2 si es checkpoint
//Temp  ////// 0 si no hay, 1 si está a la derecha, 2 si está a la izquierda
//H/Letra, S/Letra, U/Letra, checkpoint,  color, izq, victima/Letra, der
void loop() {
  cSend = 0;
  if(cEs == 'L'){
    while(cEs == 'L'){
      if(Serial2.available()){
        cEs = (char)Serial2.read();
        Serial.println("L");
        Serial3.print(0b11100110);
        Serial2.println("Cual");
      }
      Serial.println("L");
      Serial3.print(0b11100110);
      delay(10);
    }
    Serial2.println("Ya");
    while(Serial2.available()){
      (char)Serial2.read();
      Serial2.println("Ya");
    }
  }
  else if(cEs == 'D'){
    while(cEs == 'D'){
      if(Serial2.available()){
        cEs = (char)Serial2.read();
        Serial.println("D");
        Serial3.print(0b11100011);
        Serial2.println("Cual");
      }
      Serial.println("D");
      Serial3.print(0b11100011);
      delay(10);
    }
    Serial2.println("Ya");
    while(Serial2.available()){
      (char)Serial2.read();
      Serial2.println("Ya");
    }
  }
  else{
    while(Serial2.available())
      cEs = (char)Serial2.read();
  }
  switch(cEs){
    case 'L':
      Serial.println("L");
      cSend |= 0b11100110;
      break;
    case 'D':
      Serial.println("D");
      cSend |= 0b11100011;
      break;
    case 'H':
      Serial.println("H");
      cSend |= 0b10000000;
      break;
    case 'S':
      Serial.println("S");
      cSend |= 0b01000000;
      break;
    case 'U':
      Serial.println("U");
      cSend |= 0b00100000;
			while(Serial2.available())
		    cLee = (char)Serial2.read();
      break;
    case 'H':
      cSend |= 0b00110000;
			while(Serial2.available())
		    cLee = (char)Serial2.read();
      break;
    
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
