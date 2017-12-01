/* Code made by the RoBorregos team in 2017 for the RoboCup JR. Rescue Maze category.
 * Tomás Lugo, Sebastián Esquer, Ernesto Cervantez, and Alexis Virgen.
 * "El Mariachi" Achieved a third place on the international RoboCup.
 *
 * This is the core of the code, this is the class of how we save the data and manage it.
 * 
 * We used bitwise in here, this so instead of having a possibility on a boolean (a byte), we have 8 possibilities on a char or int (8 bits).
 * The operations are simple, pure | (or) and & (and) to assign or compare the data.
 *
 */
#include <Arduino.h>
#include <Tile.h>

//Constructor
//For more reference, check the Tile.h file
Tile::Tile() {
	dato1 = dato2 = 0;
}

// dato1: up, right, down, left, victim, black tile, checkpoint, visited
// dato2: (First tree used to assign the floor number), Down ramp, Exists, Beginning, UpRamp, Bumper

bool Tile::arriba() {
	return dato1 & '\x80'; // 0b10000000;
}

bool Tile::derecha() {
	return dato1 & '\x40'; // 0b01000000;
}

bool Tile::abajo() {
	return dato1 & '\x20'; // 0b00100000;
}

bool Tile::izquierda() {
	return dato1 & '\x10'; // 0b00010000;
}

bool Tile::victima() {
	return dato1 & '\x8'; // 0b00001000;
}

bool Tile::cuadroNegro() {
	return dato1 & '\x4'; // 0b00000100;
}

bool Tile::checkpoint() {
	return dato1 & '\x2'; // 0b00000010;
}

bool Tile::visitado() {
	return dato1 & '\x1'; // 0b00000001;
}


uint8_t Tile::piso() {
	if(dato2 & '\x20') { // X1
		if(dato2 & '\x40') // 1X
			return 3;
		return 2;
	}
	if(dato2 & '\x40')
		return 1;
	return 0;
}

bool Tile::rampaAbajo() {
	return dato2 & '\x10'; // 0b00010000;
}

bool Tile::existe() {
	return dato2 & '\x8'; // 0b00001000;
}

bool Tile::inicio() {
	return dato2 & '\x4'; // 0b00000100;
}

bool Tile::rampaArriba() {
	return dato2 & '\x2'; // 0b00000010;
}

bool Tile::bumper() {
	return dato2 & '\x1'; // 0b00000001;
}

//Setters
void Tile::victima(const bool &b) {
	dato1 |= (b<<3);
}

void Tile::cuadroNegro(const bool &b) {
	dato1 |= (b<<2);
}

void Tile::checkpoint(const bool &b) {
	dato1 |= (b<<1);
}

void Tile::visitado(const bool &b) {
	dato1 |= b;
}

void Tile::inicio(const bool &b) {
	dato2 |= (b<<2);
}

void Tile::rampaArriba(const bool &b) {
	dato2 |= (b<<1);
}

void Tile::bumper(const bool &b) {
	dato2 |= b;
}

void Tile::rampaAbajo(const bool &b) {
	dato2 |= (b<<4);
}

void Tile::piso(const int  & i) {
	switch(i) {
	case 0:
		dato2 &= '\xbf'; // 0X
		dato2 &= '\xdf'; // X0
		break;
	case 1:
		dato2 |= '\x40'; // 1X
		dato2 &= '\xdf'; // X0
		break;
	case 2:
		dato2 &= '\xbf'; // 0X
		dato2 |= '\x20'; // X1
		break;
	case 3:
		dato2 |= '\x40'; // 1X
		dato2 |= '\x20'; // X1
		break;
	}
}


void Tile::arriba(const bool &b, Tile *laDeArriba) {
	dato1 |= (b<<7);
	if(laDeArriba)
		laDeArriba->dato1 |= (b<<5);
}

void Tile::derecha(const bool &b, Tile *laDeDerecha) {
	dato1 |= (b<<6);
	if(laDeDerecha)
		laDeDerecha->dato1 |= (b<<4);
}

void Tile::abajo(const bool &b, Tile *laDeAabajo) {
	dato1 |= (b<<5);
	if(laDeAabajo)
		laDeAabajo->dato1 |= (b<<7);
}

void Tile::izquierda(const bool &b, Tile *laDeIzquierda) {
	dato1 |= (b<<4);
	if(laDeIzquierda)
		laDeIzquierda->dato1 |= (b<<6);
}

void Tile::existe(const bool &b) {
	dato2 |= (b<<3);
}