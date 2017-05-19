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
bool SensarMapa::sensa_Pared(Tile tMapa[3][10][10], char cDir, uint8_t iCol, uint8_t iRow, uint8_t &iPiso,  char cCase){
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
bool SensarMapa::sensaVisitado(Tile tMapa[3][10][10], char cDir, uint8_t iCol, uint8_t iRow, uint8_t &iPiso,  char cCase){
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
bool SensarMapa::sensaExiste(Tile tMapa[3][10][10], char cDir, uint8_t iCol, uint8_t iRow, uint8_t &iPiso,  char cCase){
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
void SensarMapa::llenaMapa(uint8_t iMapa[10][10], Tile tMapa[3][10][10], char cDir, uint8_t iCol, uint8_t iRow, uint8_t &iPiso){
	//Si NO se sale del rango Y EXISTE el cuadro a verificar Y NO HAY PARED entra
	//hay que poner primero el del rango, es como una "seguridad" para lo demás. Luego no sé si sería mejor poner el de la pared o existe (Cual es falso más seguido?)
	//Verifica dependiendo hacia dónde está viendo el robot. Así sabe exactamente cuantos movimientos es para hacer cierta instrucción.
	switch(cDir){
		case 'n':
			if(iCol < iTamano-1 && sensaExiste(tMapa, 'n', iCol, iRow, iPiso, 'r') && sensa_Pared(tMapa, 'n', iCol, iRow, iPiso, 'r')){	//Derecha
				//Si el tile de la derecha ES 0 Ó es mayor al numero en el que estoy, llama recursivamente la función sumandole 1.
				if(iMapa[iRow][iCol+1] == 0 || iMapa[iRow][iCol+1] > iMapa[iRow][iCol])
				{
					iMapa[iRow][iCol+1] = iMapa[iRow][iCol]+2;
					llenaMapa(iMapa, tMapa, 'e', iCol+1, iRow, iPiso);
				}
			}
			//Si NO se sale del rango Y EXISTE el cuadro a verificar Y NO HAY PARED entra
			if(iRow > 0 && sensaExiste(tMapa, 'n', iCol, iRow, iPiso, 'u') && sensa_Pared(tMapa, 'n', iCol, iRow, iPiso, 'u')){	//Enfrente
				//Si el tile de enfrente ES 0 Ó es mayor al numero en el que estoy, llama recursivamente la función sumandole 1.
				if(iMapa[iRow-1][iCol] == 0 || iMapa[iRow-1][iCol] > iMapa[iRow][iCol])
				{
					iMapa[iRow-1][iCol] = iMapa[iRow][iCol]+1;
					llenaMapa(iMapa, tMapa, 'n', iCol, iRow-1, iPiso);
				}
			}
			//Si NO se sale del rango Y EXISTE el cuadro a verificar Y NO HAY PARED entra
			if(iCol > 0 && sensaExiste(tMapa, 'n', iCol, iRow, iPiso, 'l') && sensa_Pared(tMapa, 'n', iCol, iRow, iPiso, 'l')){	//izquierda
				//Si el tile de la izquierda ES 0 Ó es mayor al numero en el que estoy, llama recursivamente la función sumandole 1.
				if(iMapa[iRow][iCol-1] == 0 || iMapa[iRow][iCol-1] > iMapa[iRow][iCol])
				{
					iMapa[iRow][iCol-1] = iMapa[iRow][iCol]+2;
					llenaMapa(iMapa, tMapa, 'w', iCol-1, iRow, iPiso);
				}
			}
			//Si NO se sale del rango Y EXISTE el cuadro a verificar Y NO HAY PARED entra
			if(iRow < iTamano-1 && sensaExiste(tMapa, 'n', iCol, iRow, iPiso, 'd') && sensa_Pared(tMapa, 'n', iCol, iRow, iPiso, 'd')){	//Atras
				//Si el tile de atras ES 0 Ó es mayor al numero en el que estoy, llama recursivamente la función sumandole 1.
				if(iMapa[iRow+1][iCol] == 0 || iMapa[iRow+1][iCol] > iMapa[iRow][iCol])
				{
					iMapa[iRow+1][iCol] = iMapa[iRow][iCol]+3;
					llenaMapa(iMapa, tMapa, 's', iCol, iRow+1, iPiso);
				}
			}
			break;
		case 'e':
			if(iCol < iTamano-1 && sensaExiste(tMapa, 'e', iCol, iRow, iPiso, 'r') && sensa_Pared(tMapa, 'e', iCol, iRow, iPiso, 'r')){	//Derecha
				//Si el tile de la derecha ES 0 Ó es mayor al numero en el que estoy, llama recursivamente la función sumandole 1.
				if(iMapa[iRow+1][iCol] == 0 || iMapa[iRow+1][iCol] > iMapa[iRow][iCol])
				{
					iMapa[iRow+1][iCol] = iMapa[iRow][iCol]+2;
					llenaMapa(iMapa, tMapa, 's', iCol, iRow+1, iPiso);
				}
			}
			//Si NO se sale del rango Y EXISTE el cuadro a verificar Y NO HAY PARED entra
			if(iRow > 0 && sensaExiste(tMapa, 'e', iCol, iRow, iPiso, 'u') && sensa_Pared(tMapa, 'e', iCol, iRow, iPiso, 'u')){	//Enfrente
				//Si el tile de enfrente ES 0 Ó es mayor al numero en el que estoy, llama recursivamente la función sumandole 1.
				if(iMapa[iRow][iCol+1] == 0 || iMapa[iRow][iCol+1] > iMapa[iRow][iCol])
				{
					iMapa[iRow][iCol+1] = iMapa[iRow][iCol]+1;
					llenaMapa(iMapa, tMapa, 'e', iCol+1, iRow, iPiso);
				}
			}
			//Si NO se sale del rango Y EXISTE el cuadro a verificar Y NO HAY PARED entra
			if(iCol > 0 && sensaExiste(tMapa, 'e', iCol, iRow, iPiso, 'l') && sensa_Pared(tMapa, 'e', iCol, iRow, iPiso, 'l')){	//izquierda
				//Si el tile de la izquierda ES 0 Ó es mayor al numero en el que estoy, llama recursivamente la función sumandole 1.
				if(iMapa[iRow-1][iCol] == 0 || iMapa[iRow-1][iCol] > iMapa[iRow][iCol])
				{
					iMapa[iRow-1][iCol] = iMapa[iRow][iCol]+2;
					llenaMapa(iMapa, tMapa, 'n', iCol, iRow-1, iPiso);
				}
			}
			//Si NO se sale del rango Y EXISTE el cuadro a verificar Y NO HAY PARED entra
			if(iRow < iTamano-1 && sensaExiste(tMapa, 'e', iCol, iRow, iPiso, 'd') && sensa_Pared(tMapa, 'e', iCol, iRow, iPiso, 'd')){	//Atras
				//Si el tile de atras ES 0 Ó es mayor al numero en el que estoy, llama recursivamente la función sumandole 1.
				if(iMapa[iRow][iCol-1] == 0 || iMapa[iRow][iCol-1] > iMapa[iRow][iCol])
				{
					iMapa[iRow][iCol-1] = iMapa[iRow][iCol]+3;
					llenaMapa(iMapa, tMapa, 'w', iCol-1, iRow, iPiso);
				}
			}
			break;
		case 's':
			if(iCol < iTamano-1 && sensaExiste(tMapa, 's', iCol, iRow, iPiso, 'r') && sensa_Pared(tMapa, 's', iCol, iRow, iPiso, 'r')){	//Derecha
				//Si el tile de la derecha ES 0 Ó es mayor al numero en el que estoy, llama recursivamente la función sumandole 1.
				if(iMapa[iRow][iCol-1] == 0 || iMapa[iRow][iCol-1] > iMapa[iRow][iCol])
				{
					iMapa[iRow][iCol-1] = iMapa[iRow][iCol]+2;
					llenaMapa(iMapa, tMapa, 'w', iCol-1, iRow, iPiso);
				}
			}
			//Si NO se sale del rango Y EXISTE el cuadro a verificar Y NO HAY PARED entra
			if(iRow > 0 && sensaExiste(tMapa, 's', iCol, iRow, iPiso, 'u') && sensa_Pared(tMapa, 's', iCol, iRow, iPiso, 'u')){	//Enfrente
				//Si el tile de enfrente ES 0 Ó es mayor al numero en el que estoy, llama recursivamente la función sumandole 1.
				if(iMapa[iRow+1][iCol] == 0 || iMapa[iRow+1][iCol] > iMapa[iRow][iCol])
				{
					iMapa[iRow+1][iCol] = iMapa[iRow][iCol]+1;
					llenaMapa(iMapa, tMapa, 's', iCol, iRow+1, iPiso);
				}
			}
			//Si NO se sale del rango Y EXISTE el cuadro a verificar Y NO HAY PARED entra
			if(iCol > 0 && sensaExiste(tMapa, 's', iCol, iRow, iPiso, 'l') && sensa_Pared(tMapa, 's', iCol, iRow, iPiso, 'l')){	//izquierda
				//Si el tile de la izquierda ES 0 Ó es mayor al numero en el que estoy, llama recursivamente la función sumandole 1.
				if(iMapa[iRow][iCol+1] == 0 || iMapa[iRow][iCol+1] > iMapa[iRow][iCol])
				{
					iMapa[iRow][iCol+1] = iMapa[iRow][iCol]+2;
					llenaMapa(iMapa, tMapa, 'e', iCol+1, iRow, iPiso);
				}
			}
			//Si NO se sale del rango Y EXISTE el cuadro a verificar Y NO HAY PARED entra
			if(iRow < iTamano-1 && sensaExiste(tMapa, 's', iCol, iRow, iPiso, 'd') && sensa_Pared(tMapa, 's', iCol, iRow, iPiso, 'd')){	//Atras
				//Si el tile de atras ES 0 Ó es mayor al numero en el que estoy, llama recursivamente la función sumandole 1.
				if(iMapa[iRow-1][iCol] == 0 || iMapa[iRow-1][iCol] > iMapa[iRow][iCol])
				{
					iMapa[iRow-1][iCol] = iMapa[iRow][iCol]+3;
					llenaMapa(iMapa, tMapa, 'n', iCol, iRow-1, iPiso);
				}
			}
			break;
		case 'w':
			if(iCol < iTamano-1 && sensaExiste(tMapa, 'w', iCol, iRow, iPiso, 'r') && sensa_Pared(tMapa, 'w', iCol, iRow, iPiso, 'r')){	//Derecha
				//Si el tile de la derecha ES 0 Ó es mayor al numero en el que estoy, llama recursivamente la función sumandole 1.
				if(iMapa[iRow-1][iCol] == 0 || iMapa[iRow-1][iCol] > iMapa[iRow][iCol])
				{
					iMapa[iRow-1][iCol] = iMapa[iRow][iCol]+2;
					llenaMapa(iMapa, tMapa, 'n', iCol, iRow-1, iPiso);
				}
			}
			//Si NO se sale del rango Y EXISTE el cuadro a verificar Y NO HAY PARED entra
			if(iRow > 0 && sensaExiste(tMapa, 'w', iCol, iRow, iPiso, 'u') && sensa_Pared(tMapa, 'w', iCol, iRow, iPiso, 'u')){	//Enfrente
				//Si el tile de enfrente ES 0 Ó es mayor al numero en el que estoy, llama recursivamente la función sumandole 1.
				if(iMapa[iRow][iCol-1] == 0 || iMapa[iRow][iCol-1] > iMapa[iRow][iCol])
				{
					iMapa[iRow][iCol-1] = iMapa[iRow][iCol]+1;
					llenaMapa(iMapa, tMapa, 'w', iCol-1, iRow, iPiso);
				}
			}
			//Si NO se sale del rango Y EXISTE el cuadro a verificar Y NO HAY PARED entra
			if(iCol > 0 && sensaExiste(tMapa, 'w', iCol, iRow, iPiso, 'l') && sensa_Pared(tMapa, 'w', iCol, iRow, iPiso, 'l')){	//izquierda
				//Si el tile de la izquierda ES 0 Ó es mayor al numero en el que estoy, llama recursivamente la función sumandole 1.
				if(iMapa[iRow+1][iCol] == 0 || iMapa[iRow+1][iCol] > iMapa[iRow][iCol])
				{
					iMapa[iRow+1][iCol] = iMapa[iRow][iCol]+2;
					llenaMapa(iMapa, tMapa, 's', iCol, iRow+1, iPiso);
				}
			}
			//Si NO se sale del rango Y EXISTE el cuadro a verificar Y NO HAY PARED entra
			if(iRow < iTamano-1 && sensaExiste(tMapa, 'w', iCol, iRow, iPiso, 'd') && sensa_Pared(tMapa, 'w', iCol, iRow, iPiso, 'd')){	//Atras
				//Si el tile de atras ES 0 Ó es mayor al numero en el que estoy, llama recursivamente la función sumandole 1.
				if(iMapa[iRow][iCol+1] == 0 || iMapa[iRow][iCol+1] > iMapa[iRow][iCol])
				{
					iMapa[iRow][iCol+1] = iMapa[iRow][iCol]+3;
					llenaMapa(iMapa, tMapa, 'e', iCol+1, iRow, iPiso);
				}
			}
			break;
	}
}
//Compara las distancias de un NO VISITADO al inicio con los demás y modifica las variables iNCol e iNRow para indicar a dónde ir.
//Esta función se puede modificar recibiendo un "caso" para decidir si busca No visitado ó Rampa ó Inicio ó lo que sea.
bool SensarMapa::comparaMapa(uint8_t iMapa[10][10], Tile tMapa[3][10][10], char cD, uint8_t iCol, uint8_t iRow, uint8_t &iNCol, uint8_t &iNRow, uint8_t &iPiso){
	//Lo declaré en un 255 porque es casi imposible que haya uno mayor a eso.
	uint8_t iC = 255;
	//Esto es por si ya no hay ningun NO Visitado o un Inicio en el piso actual. Para buscar así la rampa que lo lleve a un piso pasado.
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
	uint8_t iPos = iMapa[iNRow][iNCol], iR, iU, iL, iD;
	while(iPos > 1){
		//Calcular adyacencias
		iR = iU = iL = iD = 255;
		if(sensa_Pared(tMapa, 'n', iNCol, iNRow, iPiso, 'r') && sensaExiste(tMapa, 'n', iNCol, iNRow, iPiso, 'r') && iMapa[iNRow][iNCol+1] != 0)
			iR = iMapa[iNRow][iNCol+1];
		if(sensa_Pared(tMapa, 'n', iNCol, iNRow, iPiso, 'u') && sensaExiste(tMapa, 'n', iNCol, iNRow, iPiso, 'u') && iMapa[iNRow-1][iNCol] != 0)
			iU = iMapa[iNRow-1][iNCol];
		if(sensa_Pared(tMapa, 'n', iNCol, iNRow, iPiso, 'l') && sensaExiste(tMapa, 'n', iNCol, iNRow, iPiso, 'l') && iMapa[iNRow][iNCol-1] != 0)
			iL = iMapa[iNRow][iNCol-1];
		if(sensa_Pared(tMapa, 'n', iNCol, iNRow, iPiso, 'd') && sensaExiste(tMapa, 'n', iNCol, iNRow, iPiso, 'd') && iMapa[iNRow+1][iNCol] != 0)
			iD = iMapa[iNRow+1][iNCol];
		//Compararlas
		if(iR <= iU && iR <= iL && iR <= iD){
			sIns += "i";
			iPos = iR;
			iNCol++;
		}
		else if(iU <= iR && iU <= iL && iU <= iD){
			sIns += "a";
			iPos = iU;
			iNRow--;
		}
		else if(iL <= iU && iL <= iR && iL <= iD){
			sIns += "d";
			iPos = iL;
			iNCol--;
		}
		else{
			sIns += "e";
			iPos = iD;
			iNRow++;
		}
	}
	return sIns;
}