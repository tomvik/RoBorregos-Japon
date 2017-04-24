#include "Arduino.h"
#include "SensarMapa.h"

//Esta clase usa mucho la clase Tile.h y es usada por la clase Movimiento.h

/*
cDir Dirección
n = norte
e = este
s = sur
w = oeste

cCase Caso a verificar
r = right = derecha
u = up = arriba
l = left = izquierda
d = down = abajo
*/

//Constructores, modificar el tamaño
SensarMapa::SensarMapa(){ iTamano = 10; }
SensarMapa::SensarMapa(uint8_t iT){ iTamano = iT; }

//Regresa TRUE si es que NO hay una pared a donde se quiere checar
//Se podría usar bitwise para combinar los dos char, pero se me hace mucho pedo e inecesario para ahorrarnos sólo 1 byte
bool SensarMapa::sensa_Pared(Tile tMapa[3][10][10], uint8_t iCol, uint8_t iRow, char cDir, char cCase, uint8_t &iPiso){
	switch(cCase){
		case 'r':		//Derecha
			switch(cDir){
				case 'n':
					return !(tMapa[iPiso][iRow][iCol].derecha());
				case 'e':
					return !(tMapa[iPiso][iRow][iCol].abajo());
				case 's':
					return !(tMapa[iPiso][iRow][iCol].izquierda());
				case 'w':
					return !(tMapa[iPiso][iRow][iCol].arriba());
			}
		case 'u':		//Arriba
			switch(cDir){
				case 'n':
					return !(tMapa[iPiso][iRow][iCol].arriba());
				case 'e':
					return !tMapa[iPiso][iRow][iCol].derecha();
				case 's':
					return !(tMapa[iPiso][iRow][iCol].abajo());
				case 'w':
					return !(tMapa[iPiso][iRow][iCol].izquierda());
			}		
		case 'l':		//Izquierda
			switch(cDir){
				case 'n':
					return !(tMapa[iPiso][iRow][iCol].izquierda());
				case 'e':
					return !(tMapa[iPiso][iRow][iCol].arriba());
				case 's':
					return !tMapa[iPiso][iRow][iCol].derecha();
				case 'w':
					return !(tMapa[iPiso][iRow][iCol].abajo());
			}
		case 'd':		//Abajo
			switch(cDir){
				case 'n':
					return !(tMapa[iPiso][iRow][iCol].abajo());
				case 'e':
					return !(tMapa[iPiso][iRow][iCol].izquierda());
				case 's':
					return !(tMapa[iPiso][iRow][iCol].arriba());
				case 'w':
					return !tMapa[iPiso][iRow][iCol].derecha();
			}

	}
}
//Regresa TRUE si NO ha sido visitado
bool SensarMapa::sensaVisitado(Tile tMapa[3][10][10], uint8_t iCol, uint8_t iRow, char cDir, char cCase, uint8_t &iPiso){
	switch(cCase){
		case 'r':				//Derecha
			switch(cDir){
				case 'n':
					return !(tMapa[iPiso][iRow][iCol+1].visitado());
				case 'e':
					return !(tMapa[iPiso][iRow+1][iCol].visitado());
				case 's':
					return !(tMapa[iPiso][iRow][iCol-1].visitado());
				case 'w':
					return !(tMapa[iPiso][iRow-1][iCol].visitado());
			}
		case 'u':				//Enfrente
			switch(cDir){
				case 'n':
					return !(tMapa[iPiso][iRow-1][iCol].visitado());
				case 'e':
					return !(tMapa[iPiso][iRow][iCol+1].visitado());
				case 's':
					return !(tMapa[iPiso][iRow+1][iCol].visitado());
				case 'w':
					return !(tMapa[iPiso][iRow][iCol-1].visitado());
			}
		case 'l':				//Izquierda
			switch(cDir){
				case 'n':
					return !(tMapa[iPiso][iRow][iCol-1].visitado());
				case 'e':
					return !(tMapa[iPiso][iRow-1][iCol].visitado());
				case 's':
					return !(tMapa[iPiso][iRow][iCol+1].visitado());
				case 'w':
					return !(tMapa[iPiso][iRow+1][iCol].visitado());
			}
		case 'd':				//Atras
			switch(cDir){
				case 'n':
					return !(tMapa[iPiso][iRow+1][iCol].visitado());
				case 'e':
					return !(tMapa[iPiso][iRow][iCol-1].visitado());
				case 's':
					return !(tMapa[iPiso][iRow-1][iCol].visitado());
				case 'w':
					return !(tMapa[iPiso][iRow][iCol+1].visitado());
			}
	}
}
//Regresa TRUE si EXISTE la coordenada
bool SensarMapa::sensaExiste(Tile tMapa[3][10][10], uint8_t iCol, uint8_t iRow, char cDir, char cCase, uint8_t &iPiso){
	switch(cCase){
		case 'r':				//Derecha
			switch(cDir){
				case 'n':
					return (tMapa[iPiso][iRow][iCol+1].existe());
				case 'e':
					return (tMapa[iPiso][iRow+1][iCol].existe());
				case 's':
					return (tMapa[iPiso][iRow][iCol-1].existe());
				case 'w':
					return (tMapa[iPiso][iRow-1][iCol].existe());
			}
		case 'u':				//Enfrente
			switch(cDir){
				case 'n':
					return (tMapa[iPiso][iRow-1][iCol].existe());
				case 'e':
					return (tMapa[iPiso][iRow][iCol+1].existe());
				case 's':
					return (tMapa[iPiso][iRow+1][iCol].existe());
				case 'w':
					return (tMapa[iPiso][iRow][iCol-1].existe());
			}
		case 'l':				//Izquierda
			switch(cDir){
				case 'n':
					return (tMapa[iPiso][iRow][iCol-1].existe());
				case 'e':
					return (tMapa[iPiso][iRow-1][iCol].existe());
				case 's':
					return (tMapa[iPiso][iRow][iCol+1].existe());
				case 'w':
					return (tMapa[iPiso][iRow+1][iCol].existe());
			}
		case 'd':				//Atras
			switch(cDir){
				case 'n':
					return (tMapa[iPiso][iRow+1][iCol].existe());
				case 'e':
					return (tMapa[iPiso][iRow][iCol-1].existe());
				case 's':
					return (tMapa[iPiso][iRow-1][iCol].existe());
				case 'w':
					return (tMapa[iPiso][iRow][iCol+1].existe());
			}
	}
}
//FUNCION RECURSIVA, USA ALGORITMO DEPTH FIRST SEARCH
void SensarMapa::llenaMapa(uint8_t iMapa[10][10], Tile tMapa[3][10][10], uint8_t iCol, uint8_t iRow, uint8_t &iPiso){
	//Si NO se sale del rango Y EXISTE el cuadro a verificar Y NO HAY PARED entra
	//hay que poner primero el del rango, es como una "seguridad" para lo demás. Luego no sé si sería mejor poner el de la pared o existe (Cual es falso más seguido?)
	if(iCol < iTamano-1 && sensaExiste(tMapa, iCol, iRow, 'n', 'r', iPiso) && sensa_Pared(tMapa, iCol, iRow, 'n', 'r', iPiso)){	//Derecha
		//Si el tile de la derecha ES 0 Ó es mayor al numero en el que estoy, llama recursivamente la función sumandole 1.
		if(iMapa[iRow][iCol+1] == 0 || iMapa[iRow][iCol+1] > iMapa[iRow][iCol])
		{
			iMapa[iRow][iCol+1] = iMapa[iRow][iCol]+1;
			llenaMapa(iMapa, tMapa, iCol+1, iRow, iPiso);
		}
	}
	//Si NO se sale del rango Y EXISTE el cuadro a verificar Y NO HAY PARED entra
	if(iRow > 0 && sensaExiste(tMapa, iCol, iRow, 'n', 'u', iPiso) && sensa_Pared(tMapa, iCol, iRow, 'n', 'u', iPiso)){	//Enfrente
		//Si el tile de enfrente ES 0 Ó es mayor al numero en el que estoy, llama recursivamente la función sumandole 1.
		if(iMapa[iRow-1][iCol] == 0 || iMapa[iRow-1][iCol] > iMapa[iRow][iCol])
		{
			iMapa[iRow-1][iCol] = iMapa[iRow][iCol]+1;
			llenaMapa(iMapa, tMapa, iCol, iRow-1, iPiso);
		}
	}
	//Si NO se sale del rango Y EXISTE el cuadro a verificar Y NO HAY PARED entra
	if(iCol > 0 && sensaExiste(tMapa, iCol, iRow, 'n', 'l', iPiso) && sensa_Pared(tMapa, iCol, iRow, 'n', 'l', iPiso)){	//izquierda
		//Si el tile de la izquierda ES 0 Ó es mayor al numero en el que estoy, llama recursivamente la función sumandole 1.
		if(iMapa[iRow][iCol-1] == 0 || iMapa[iRow][iCol-1] > iMapa[iRow][iCol])
		{
			iMapa[iRow][iCol-1] = iMapa[iRow][iCol]+1;
			llenaMapa(iMapa, tMapa, iCol-1, iRow, iPiso);
		}
	}
	//Si NO se sale del rango Y EXISTE el cuadro a verificar Y NO HAY PARED entra
	if(iRow < iTamano-1 && sensaExiste(tMapa, iCol, iRow, 'n', 'd', iPiso) && sensa_Pared(tMapa, iCol, iRow, 'n', 'd', iPiso)){	//Atras
		//Si el tile de atras ES 0 Ó es mayor al numero en el que estoy, llama recursivamente la función sumandole 1.
		if(iMapa[iRow+1][iCol] == 0 || iMapa[iRow+1][iCol] > iMapa[iRow][iCol])
		{
			iMapa[iRow+1][iCol] = iMapa[iRow][iCol]+1;
			llenaMapa(iMapa, tMapa, iCol, iRow+1, iPiso);
		}
	}
}
//Compara las distancias de un NO VISITADO al inicio con los demás y modifica las variables iNCol e iNRow para indicar a dónde ir.
//Esta función se puede modificar recibiendo un "caso" para decidir si busca No visitado ó Rampa ó Inicio ó lo que sea.
bool SensarMapa::comparaMapa(uint8_t iMapa[10][10], Tile tMapa[3][10][10], uint8_t iCol, uint8_t iRow, uint8_t &iNCol, uint8_t &iNRow, char cD, uint8_t &iPiso){
	//Lo declaré en un 100 porque es casi imposible que haya uno mayor a eso.
	uint8_t iC = 100;
	bool bT = false;
	//Loop por toda la matriz de numeros
	if(cD == 'n'){
		for (uint8_t i = 0; i < iTamano; ++i)
			for (uint8_t j = 0; j < iTamano; ++j)
				if(iMapa[i][j] != 0 && iC >= iMapa[i][j] && !tMapa[iPiso][i][j].visitado()){ //Aquí es donde sólo verifica si NO ha sido visitado y compara el numero para ver que es menor Ó 0
					iC = iMapa[i][j];
					iNCol = j;
					iNRow = i;
					bT = true;
				}
	}
	else if(cD == 'i'){
		for (uint8_t i = 0; i < iTamano; ++i)
			for (uint8_t j = 0; j < iTamano; ++j)
				if(iMapa[i][j] != 0 && iC >= iMapa[i][j] && tMapa[iPiso][i][j].inicio()){ //Aquí es donde sólo verifica si ES inicio y compara el numero para ver que es menor Ó 0
					iC = iMapa[i][j];
					iNCol = j;
					iNRow = i;
					bT = true;
				}
	}
	if(bT == false){
		for (uint8_t i = 0; i < iTamano; ++i) 
			for (uint8_t j = 0; j < iTamano; ++j)
				if(iMapa[i][j] != 0 && ( tMapa[iPiso][i][j].rampaAbajo() || tMapa[iPiso][i][j].rampaArriba() ) && tMapa[iPiso][i][j].piso() < iPiso){ 
					//Aquí se verifica que NO sea 0, que SEA MENOR al numero en donde estoy, que SEA UNA RAMPA y QUE CONECTE A UN PISO MENOR
					iC = iMapa[i][j];
					iNCol = j;
					iNRow = i;
					bT = true;
				}
	}
	return bT;
}
//Con el resultado de la función anterior, crea las instrucciones de cómo llegar hasta ese punto yendose en reversa.
//También para simplicidad, todo lo hace viendo a su "norte"
String SensarMapa::getInstrucciones(uint8_t iMapa[10][10], Tile tMapa[3][10][10], uint8_t iNCol, uint8_t iNRow, uint8_t &iPiso){
	String sIns = "";
	uint8_t iPos = iMapa[iNRow][iNCol];
	while(iPos > 1){
		Serial.print("QUIERO IR A: "); Serial.print(iNCol); Serial.print(", "); Serial.println(iNRow);
		if(sensa_Pared(tMapa, iNCol, iNRow, 'n', 'r', iPiso) && iMapa[iNRow][iNCol+1] == iPos-1){
			sIns += "i";
			iPos--;
			iNCol++;
		}
		if(sensa_Pared(tMapa, iNCol, iNRow, 'n', 'u', iPiso) && iMapa[iNRow-1][iNCol] == iPos-1){
			sIns += "a";
			iPos--;
			iNRow--;
		}
		if(sensa_Pared(tMapa, iNCol, iNRow, 'n', 'l', iPiso) && iMapa[iNRow][iNCol-1] == iPos-1){
			sIns += "d";
			iPos--;
			iNCol--;
		}
		if(sensa_Pared(tMapa, iNCol, iNRow, 'n', 'd', iPiso) && iMapa[iNRow+1][iNCol] == iPos-1){
			sIns += "e";
			iPos--;
			iNRow++;
		}
	}
	return sIns;
}