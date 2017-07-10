#include <Wire.h>
#include <Adafruit_MLX90614.h>


#define sensorOut 2
#define S0 3
#define S1 4
#define S2 5
#define S3 6

#define MlxL 0x1C
#define MlxR 0x4C

Adafruit_MLX90614 mlxRight = Adafruit_MLX90614(MlxR);
Adafruit_MLX90614 mlxLeft = Adafruit_MLX90614(MlxL);

unsigned long tiempoYa, tiempoAntes;
uint8_t utemp;
char cSend;
//2 es checkpoint
//1 es negro
//0 es blanco
uint8_t sensorR() {
	int frequency = 0, iR;
	digitalWrite(S2, HIGH);
	digitalWrite(S3, HIGH);
	for (uint8_t i = 0; i < 3; i++)
		frequency += pulseIn(sensorOut, LOW);
	frequency /= 3;
  //Serial.println(frequency);
  iR = (frequency >= 100) ? 1 : 0;
  iR = (frequency <= 40) ? 2 : 0;
  return iR;
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
	Serial3.begin(115200);
  //Serial.begin(9600);
  Serial2.begin(9600);
	mlxRight.begin();
	mlxLeft.begin();
	pinMode(S0, OUTPUT);
	pinMode(S1, OUTPUT);
	pinMode(S2, OUTPUT);
	pinMode(S3, OUTPUT);
	pinMode(sensorOut, INPUT);

	digitalWrite(S0, HIGH);
	digitalWrite(S1, LOW);
}
//Color ////// 0 si es blanco, 1 si es negro, 2 si es checkpoint
//Temp  ////// 0 si no hay, 1 si está a la derecha, 2 si está a la izquierda
//0, 0, 0, checkpoint,  color, izq, victima, der
void loop() {
  //Serial2.println("TOMA");
	cSend = 0;
	utemp = sensarTemperatura();
	switch(utemp) {
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
    cSend |= 0b00001000;
    break;
  case 2:
    cSend |= 0b00010000;
    break;
 }
	Serial3.print(cSend);
  delay(10);
}
