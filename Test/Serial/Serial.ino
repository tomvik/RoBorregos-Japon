#include <Arduino.h>
//TX mega y RX rasp
//RX mega y TX rasp
int iC = 1;
void setup() {
	Serial.begin(9600);
	//Inicializamos el serial conectado a la rasp
	Serial2.begin(9600);
	//Mandamos la señal de que se prendió
	Serial2.println("Enciende");
}

void loop() {
	//Si hay algo en el serial, lo lee e imprime diferentes datos a la rasp (e imprime lo que lee a la pantalla)
	if(Serial2.available() > 0) {
		char input = (char)Serial2.read();
		if(iC == 1) {
			Serial2.println("UNO");
			Serial.print("LEO: "); Serial.println(input);
		}
		else if(iC == 2) {
			Serial2.println("DOS");
			Serial.print("LEO: "); Serial.println(input);
		}
		else if(iC == 3) {
			Serial2.println("TRES");
			Serial.print("LEO: "); Serial.println(input);
		}
		else{
			Serial2.println("MAYOR");
			Serial.print("LEO: "); Serial.println(input);
			iC = 0;
		}
		iC++;
	}
	//Si no hay nada, no lee nada e imprime No a la pantalla y a la rasp
	else{
		Serial.println("NO");
		Serial2.println("NO");
	}
	delay(500);
}
