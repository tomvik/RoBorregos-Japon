#include "Arduino.h"
#include "Tile.h"

//Constructor
Tile::Tile(){
    dato1 = dato2 = dato3 = 0;
}


//Getters
/*
Con & and comprobamos si es 0 o 1
 Ponemos un valor literal ' ' de char para evitar conversiones inecesarias
Poniendo '\xhhh' se puede especificar el valor en hexadecimal del char ascii
*/  

//arr, der, aba, izq, victima, cuadro negro, checkpoint, visitada 
//0, 0, 0, Rampabajo, existe, inicio, rampaArriba, bumper
bool Tile::arriba(){
    return dato1&'\x80';//0b10000000;
}

bool Tile::derecha(){
    return dato1&'\x40';//0b01000000;
}

bool Tile::abajo(){
    return dato1&'\x20';//0b00100000;
}

bool Tile::izquierda(){
    return dato1&'\x10';//0b00010000;
}

bool Tile::victima(){
    return dato1&'\x8';//0b00001000;
}

bool Tile::cuadroNegro(){
    return dato1&'\x4';//0b00000100;
}

bool Tile::checkpoint(){
    return dato1&'\x2';//0b00000010;
}

bool Tile::visitado(){
    return dato1&'\x1';//0b00000001;
}

bool Tile::inicio(){
    return dato2&'\x4';//0b00000100;
}

bool Tile::rampaArriba(){
    return dato2&'\x2';//0b00000010;
}

bool Tile::bumper(){
    return dato2&'\x1';//0b00000001;
}

bool Tile::existe(){
    return dato2&'\x8';//0b00001000;
}

bool Tile::rampaAbajo(){
    return dato2&'\x10';//0b00010000;
}

uint8_t Tile::piso(){
    if(dato2&'\x20'){ //X1
        if(dato2&'\x40') //1X
            return 3;
        return 2;
    }
    if(dato2&'\x40') 
        return 1;
    return 0;
}

bool Tile::calorArriba(){
    return dato3&'\x80';
}

bool Tile::calorDerecha(){
    return dato3&'\x40';
}

bool Tile::calorAbajo(){
    return dato3&'\x20';
}

bool Tile::calorIzquierda(){
    return dato3&'\x10';
}

bool Tile::visualArriba(){
    return dato3&'\x8';
}

bool Tile::visualDerecha(){
    return dato3&'\x4';
}

bool Tile::visualAbajo(){
    return dato3&'\x2';
}

bool Tile::visualIzquierda(){
    return dato3&'\x1';
}


//Setters
/* 
Nota: Los datos booleanos si true=1
Con | or ponemos el 1 que manden, &and ponemos 0
*/
void Tile::victima(const bool &b){
    dato1 |= (b<<3);
}

void Tile::cuadroNegro(const bool &b){
    dato1 |= (b<<2);
}

void Tile::checkpoint(const bool &b){
    dato1 |= (b<<1);
}

void Tile::visitado(const bool &b){
    dato1 |= b;
}

void Tile::inicio(const bool &b){
    dato2 |= (b<<2);
}   

void Tile::rampaArriba(const bool &b){
    dato2 |= (b<<1);
}    

void Tile::bumper(const bool &b){
    dato2 |= b;
}

void Tile::rampaAbajo(const bool &b){
    dato2 |= (b<<4);
}

void Tile::piso(const int &i){
    switch(i){
        case 0:
            dato2&='\xbf'; //0X
            dato2&='\xdf'; //X0
            break;
        case 1:
            dato2|='\x40'; //1X
            dato2&='\xdf'; //X0
            break;
        case 2:
            dato2&='\xbf'; //0X
            dato2|='\x20'; //X1
            break;
        case 3:
            dato2|='\x40'; //1X
            dato2|='\x20'; //X1
            break;
    }
}


void Tile::arriba(const bool &b, Tile *laDeArriba){
    dato1 |= (b<<7);
    if(laDeArriba) 
        laDeArriba->dato1 |= (b<<5);
}

void Tile::derecha(const bool &b, Tile *laDeDerecha){
    dato1 |= (b<<6);
    if(laDeDerecha)
        laDeDerecha->dato1 |= (b<<4);
}

void Tile::abajo(const bool &b, Tile *laDeAabajo){
    dato1 |= (b<<5);
    if(laDeAabajo) 
        laDeAabajo->dato1 |= (b<<7);
}

void Tile::izquierda(const bool &b, Tile *laDeIzquierda){
    dato1 |= (b<<4);
    if(laDeIzquierda)
        laDeIzquierda->dato1 |= (b<<6);
}

void Tile::existe(const bool &b){
    dato2 |= (b<<3);
}

void Tile::calorArriba(const bool &b){
    dato3 |= (b<<7);
}

void Tile::calorDerecha(const bool &b){
    dato3 |= (b<<6);
}

void Tile::calorAbajo(const bool &b){
    dato3 |= (b<<5);
}

void Tile::calorIzquierda(const bool &b){
    dato3 |= (b<<4);
}

void Tile::visualArriba(const bool &b){
    dato3 |= (b<<3);
}

void Tile::visualDerecha(const bool &b){
    dato3 |= (b<<2);
}

void Tile::visualAbajo(const bool &b){
    dato3 |= (b<<1);
}

void Tile::visualIzquierda(const bool &b){
    dato3 |= b;
}