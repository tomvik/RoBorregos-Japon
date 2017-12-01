#include <Wire.h>
#include <Adafruit_MLX90614.h>

#define Mlxt 0x5A

Adafruit_MLX90614 mlx = Adafruit_MLX90614(Mlxt);
void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);
	mlx.begin();
	Serial.print("Ambiente "); Serial.println(mlx.readAmbientTempC());
}

void loop() {
	// put your main code here, to run repeatedly:
	Serial.println(mlx.readObjectTempC());
	delay(100);
}
