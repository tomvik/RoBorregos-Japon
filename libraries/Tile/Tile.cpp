#include "Arduino.h"
#include "Tile.h"

//Constructor
Tile::Tile(){
    dato1= 0;
    dato2= 0;
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


//Setters
/* 
Nota: Los datos booleanos si true=1
Con | or ponemos el 1 que manden, &and ponemos 0
*/
void Tile::victima(const bool &b){
    dato1 |= (b<<3);
    //dato1= b? dato1|'\x8':dato1&'\xf7'; //0b00001000:0b11110111;  si solo se quisiera poner 1 mejor dato1|= (b<<3);
}

void Tile::cuadroNegro(const bool &b){
    dato1 |= (b<<2);
    //dato1= b? dato1|'\x4':dato1&'\xfb'; //0b00000100:0b11111011
}

void Tile::checkpoint(const bool &b){
    dato1 |= (b<<1);
    //dato1= b? dato1|'\x2':dato1&'\xfd';//0b00000010:0b11111101
}

void Tile::visitado(const bool &b){
    dato1 |= b;
    //dato1= b? dato1|'\x1':dato1&'\xfe';//0b00000001:0b1111 1110
}

void Tile::inicio(const bool &b){
    dato2 |= (b<<2);
    //dato2= b? dato2|'\x4':dato2&'\xfb';//0b00000100:0b1111 1011
}   

void Tile::rampaArriba(const bool &b){
    dato2 |= (b<<1);
    //dato2= b? dato2|'\x2':dato2&'\xfd';//0b00000010:0b1111 1101
}    

void Tile::bumper(const bool &b){
    dato2 |= b;
    //dato2= b? dato2|'\x1':dato2&'\xfe';//0b00000001:0b1111 1110
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
    //dato1= b? dato1|'\x80':dato1&'\x7f';//0b10000000 
    if(laDeArriba) 
        laDeArriba->dato1 |= (b<<5);
        //laDeArriba->dato1= b? (laDeArriba->dato1)|'\x20':(laDeArriba->dato1)&'\xdf';//Modificamos pared de abajo del de arriba
}

void Tile::derecha(const bool &b, Tile *laDeDerecha){
    dato1 |= (b<<6);
    //dato1= b? dato1|'\x40':dato1&'\xbf';//0b01000000 
    if(laDeDerecha)
        laDeDerecha->dato1 |= (b<<4);
        //laDeDerecha->dato1= b? (laDeDerecha->dato1)|'\x10':(laDeDerecha->dato1)&'\xef';//Modificamos pared de izq del de derecha
}

void Tile::abajo(const bool &b, Tile *laDeAabajo){
    dato1 |= (b<<5);
    //dato1= b? dato1|'\x20':dato1&'\xdf';//0b00100000 
    if(laDeAabajo) 
        laDeAabajo->dato1 |= (b<<7);
        //laDeAabajo->dato1= b? (laDeAabajo->dato1)|'\x80':(laDeAabajo->dato1)&'\x7f';//Modificamos pared de arriba del de abajo
}

void Tile::izquierda(const bool &b, Tile *laDeIzquierda){
    dato1 |= (b<<4);
    //dato1= b? dato1|'\x10':dato1&'\xef';//0b00010000 
    if(laDeIzquierda)
        laDeIzquierda->dato1 |= (b<<6);
        //laDeIzquierda->dato1= b? (laDeIzquierda->dato1)|'\x40':(laDeIzquierda->dato1)&'\xbf';//Modificamos pared de der del de izquierda
}

void Tile::existe(const bool &b){
    dato2 |= (b<<3);
    //dato2= b? dato1|'\x8':dato1&'\xf7'; //0b00001000:0b11110111;  si solo se quisiera poner 1 mejor dato1|= (b<<3);
}