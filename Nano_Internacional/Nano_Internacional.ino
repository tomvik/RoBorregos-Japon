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
bool sensorR() {
	int frequency = 0;
	digitalWrite(S2, HIGH);
	digitalWrite(S3, HIGH);
	for (uint8_t i = 0; i < 3; i++)
		frequency += pulseIn(sensorOut, LOW);
	frequency /= 3;
	return (frequency >= 120);
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
	Serial.begin(115200);
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
//Color ////// 0 si es blanco, 1 si es negro
//Temp  ////// 0 si no hay, 1 si está a la derecha, 2 si está a la izquierda
//0, 0, 0, 0,  color, izq, victima, der
void loop() {
	cSend = 0;
	utemp = sensarTemperatura();
	switch(utemp) {
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
	cSend |= (sensorR() == 1) ? 0b00001000 : 0b00000000;
	Serial.print(cSend);
}
