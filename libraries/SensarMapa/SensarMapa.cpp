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
SensarMapa::SensarMapa(){ iTamano = 10; iBumper = 8; }
//Regresa TRUE si es que NO hay una pared a donde se quiere checar
bool SensarMapa::sensa_Pared(Tile tMapa[3][10][10], char cDir, uint8_t iCol, uint8_t iRow, uint8_t iPiso,  char cCase){
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
bool SensarMapa::sensaVisitado(Tile tMapa[3][10][10], char cDir, uint8_t iCol, uint8_t iRow, uint8_t iPiso,  char cCase){
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
bool SensarMapa::sensaExiste(Tile tMapa[3][10][10], char cDir, uint8_t iCol, uint8_t iRow, uint8_t iPiso,  char cCase){
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
void SensarMapa::llenaMapa(uint8_t iMapa[10][10], char cMapa[10][10], Tile tMapa[3][10][10], char cDir, uint8_t iCol, uint8_t iRow, uint8_t iPiso){
	//Si NO se sale del rango Y EXISTE el cuadro a verificar Y NO HAY PARED entra
	//hay que poner primero el del rango, es como una "seguridad" para lo demás. Luego no sé si sería mejor poner el de la pared o existe (Cual es falso más seguido?)
	//Verifica dependiendo hacia dónde está viendo el robot. Así sabe exactamente cuantos movimientos es para hacer cierta instrucción.
	//También verifica si es 0 Ó mayor al numero en el que estoy. Y le suma lo correspondiente.
	//Right, up, left, down, relativo al robot
	switch(cDir){
		case 'n':
			//Right
			if(iCol < iTamano-1 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'r') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'r') && (iMapa[iRow][iCol+1] == 0 || iMapa[iRow][iCol+1] > iMapa[iRow][iCol]+2)){
				iMapa[iRow][iCol+1] = ( tMapa[iPiso][iRow][iCol+1].bumper() ) ? iMapa[iRow][iCol]+2+iBumper : iMapa[iRow][iCol]+2;
				cMapa[iRow][iCol+1] = 'r';
				llenaMapa(iMapa, cMapa, tMapa, 'e', iCol+1, iRow, iPiso);
			}
			//UP
			if(iRow > 0 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'u') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'u') && (iMapa[iRow-1][iCol] == 0 || iMapa[iRow-1][iCol] > iMapa[iRow][iCol]+1)){
				iMapa[iRow-1][iCol] = ( tMapa[iPiso][iRow-1][iCol].bumper() ) ? iMapa[iRow][iCol]+1+iBumper : iMapa[iRow][iCol]+1;
				cMapa[iRow-1][iCol] = 'u';
				llenaMapa(iMapa, cMapa, tMapa, 'n', iCol, iRow-1, iPiso);
			}
			//LEFT
			if(iCol > 0 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'l') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'l') && (iMapa[iRow][iCol-1] == 0 || iMapa[iRow][iCol-1] > iMapa[iRow][iCol]+2)){
				iMapa[iRow][iCol-1] = ( tMapa[iPiso][iRow][iCol-1].bumper() ) ? iMapa[iRow][iCol]+2+iBumper : iMapa[iRow][iCol]+2;
				cMapa[iRow][iCol-1] = 'l';
				llenaMapa(iMapa, cMapa, tMapa, 'w', iCol-1, iRow, iPiso);
			}
			//DOWN
			if(iRow < iTamano-1 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'd') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'd') && (iMapa[iRow+1][iCol] == 0 || iMapa[iRow+1][iCol] > iMapa[iRow][iCol]+3)){
				iMapa[iRow+1][iCol] = ( tMapa[iPiso][iRow+1][iCol].bumper() ) ? iMapa[iRow][iCol]+3+iBumper : iMapa[iRow][iCol]+3;
				cMapa[iRow+1][iCol] = 'd';
				llenaMapa(iMapa, cMapa, tMapa, 's', iCol, iRow+1, iPiso);
			}
			break;
		case 'e':
			//Right
			if(iRow < iTamano-1 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'r') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'r') && (iMapa[iRow+1][iCol] == 0 || iMapa[iRow+1][iCol] > iMapa[iRow][iCol]+2)){
				iMapa[iRow+1][iCol] = ( tMapa[iPiso][iRow+1][iCol].bumper() ) ? iMapa[iRow][iCol]+2+iBumper : iMapa[iRow][iCol]+2;
				cMapa[iRow+1][iCol] = 'r';
				llenaMapa(iMapa, cMapa, tMapa, 's', iCol, iRow+1, iPiso);
			}
			//UP
			if(iCol < iTamano-1 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'u') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'u') && (iMapa[iRow][iCol+1] == 0 || iMapa[iRow][iCol+1] > iMapa[iRow][iCol]+1)){
				iMapa[iRow][iCol+1] = ( tMapa[iPiso][iRow][iCol+1].bumper() ) ? iMapa[iRow][iCol]+1+iBumper : iMapa[iRow][iCol]+1;
				cMapa[iRow][iCol+1] = 'u';
				llenaMapa(iMapa, cMapa, tMapa, 'e', iCol+1, iRow, iPiso);
			}
			//LEFT
			if(iRow > 0 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'l') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'l') && (iMapa[iRow-1][iCol] == 0 || iMapa[iRow-1][iCol] > iMapa[iRow][iCol]+2)){
				iMapa[iRow-1][iCol] = ( tMapa[iPiso][iRow-1][iCol].bumper() ) ? iMapa[iRow][iCol]+2+iBumper : iMapa[iRow][iCol]+2;
				cMapa[iRow-1][iCol] = 'l';
				llenaMapa(iMapa, cMapa, tMapa, 'n', iCol, iRow-1, iPiso);
			}
			//DOWN
			if(iCol > 0 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'd') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'd') && (iMapa[iRow][iCol-1] == 0 || iMapa[iRow][iCol-1] > iMapa[iRow][iCol]+3)){
				iMapa[iRow][iCol-1] = ( tMapa[iPiso][iRow][iCol-1].bumper() ) ? iMapa[iRow][iCol]+3+iBumper : iMapa[iRow][iCol]+3;
				cMapa[iRow][iCol-1] = 'd';
				llenaMapa(iMapa, cMapa, tMapa, 'w', iCol-1, iRow, iPiso);
			}
			break;
		case 's':
			//Right
			if(iCol > 0 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'r') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'r') && (iMapa[iRow][iCol-1] == 0 || iMapa[iRow][iCol-1] > iMapa[iRow][iCol]+2)){
				iMapa[iRow][iCol-1] = ( tMapa[iPiso][iRow][iCol-1].bumper() ) ? iMapa[iRow][iCol]+2+iBumper : iMapa[iRow][iCol]+2;
				cMapa[iRow][iCol-1] = 'r';
				llenaMapa(iMapa, cMapa, tMapa, 'w', iCol-1, iRow, iPiso);
			}
			//UP
			if(iRow < iTamano-1 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'u') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'u') && (iMapa[iRow+1][iCol] == 0 || iMapa[iRow+1][iCol] > iMapa[iRow][iCol]+1)){
				iMapa[iRow+1][iCol] = ( tMapa[iPiso][iRow+1][iCol].bumper() ) ? iMapa[iRow][iCol]+1+iBumper : iMapa[iRow][iCol]+1;
				cMapa[iRow+1][iCol] = 'u';
				llenaMapa(iMapa, cMapa, tMapa, 's', iCol, iRow+1, iPiso);
			}
			//LEFT
			if(iCol < iTamano-1 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'l') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'l') && (iMapa[iRow][iCol+1] == 0 || iMapa[iRow][iCol+1] > iMapa[iRow][iCol]+2)){
				iMapa[iRow][iCol+1] = ( tMapa[iPiso][iRow][iCol+1].bumper() ) ? iMapa[iRow][iCol]+2+iBumper : iMapa[iRow][iCol]+2;
				cMapa[iRow][iCol+1] = 'l';
				llenaMapa(iMapa, cMapa, tMapa, 'e', iCol+1, iRow, iPiso);
			}
			//DOWN
			if(iRow > 0 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'd') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'd') && (iMapa[iRow-1][iCol] == 0 || iMapa[iRow-1][iCol] > iMapa[iRow][iCol]+3)){
				iMapa[iRow-1][iCol] = ( tMapa[iPiso][iRow-1][iCol].bumper() ) ? iMapa[iRow][iCol]+3+iBumper : iMapa[iRow][iCol]+3;
				cMapa[iRow-1][iCol] = 'd';
				llenaMapa(iMapa, cMapa, tMapa, 'n', iCol, iRow-1, iPiso);
			}
			break;
		case 'w':
			//Right
			if(iRow > 0 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'r') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'r') && (iMapa[iRow-1][iCol] == 0 || iMapa[iRow-1][iCol] > iMapa[iRow][iCol]+2)){
				iMapa[iRow-1][iCol] = ( tMapa[iPiso][iRow-1][iCol].bumper() ) ? iMapa[iRow][iCol]+2+iBumper : iMapa[iRow][iCol]+2;
				cMapa[iRow-1][iCol] = 'r';
				llenaMapa(iMapa, cMapa, tMapa, 'n', iCol, iRow-1, iPiso);
			}
			//UP
			if(iCol > 0 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'u') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'u') && (iMapa[iRow][iCol-1] == 0 || iMapa[iRow][iCol-1] > iMapa[iRow][iCol]+1)){
				iMapa[iRow][iCol-1] = ( tMapa[iPiso][iRow][iCol-1].bumper() ) ? iMapa[iRow][iCol]+1+iBumper : iMapa[iRow][iCol]+1;
				cMapa[iRow][iCol-1] = 'u';
				llenaMapa(iMapa, cMapa, tMapa, 'w', iCol-1, iRow, iPiso);
			}
			//LEFT
			if(iRow < iTamano-1 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'l') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'l') && (iMapa[iRow+1][iCol] == 0 || iMapa[iRow+1][iCol] > iMapa[iRow][iCol]+2)){
				iMapa[iRow+1][iCol] = ( tMapa[iPiso][iRow+1][iCol].bumper() ) ? iMapa[iRow][iCol]+2+iBumper : iMapa[iRow][iCol]+2;
				cMapa[iRow+1][iCol] = 'l';
				llenaMapa(iMapa, cMapa, tMapa, 's', iCol, iRow+1, iPiso);
			}
			//DOWN
			if(iCol < iTamano-1 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'd') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'd') && (iMapa[iRow][iCol+1] == 0 || iMapa[iRow][iCol+1] > iMapa[iRow][iCol]+3)){
				iMapa[iRow][iCol+1] = ( tMapa[iPiso][iRow][iCol+1].bumper() ) ? iMapa[iRow][iCol]+3+iBumper : iMapa[iRow][iCol]+3;
				cMapa[iRow][iCol+1] = 'd';
				llenaMapa(iMapa, cMapa, tMapa, 'e', iCol+1, iRow, iPiso);
			}
			break;
	}
}
//Compara las distancias de un NO VISITADO al inicio con los demás y modifica las variables iNCol e iNRow para indicar a dónde ir.
//Esta función se puede modificar recibiendo un "caso" para decidir si busca No visitado ó Rampa ó Inicio ó lo que sea.
bool SensarMapa::comparaMapa(uint8_t iMapa[10][10], Tile tMapa[3][10][10], char cD, uint8_t iCol, uint8_t iRow, uint8_t &iNCol, uint8_t &iNRow, uint8_t iPiso){
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
String SensarMapa::getInstrucciones(uint8_t iMapa[10][10], char cMapa[10][10], Tile tMapa[3][10][10], uint8_t iNCol, uint8_t iNRow, uint8_t iPiso){
	String sIns = "";
	sIns += cMapa[iNRow][iNCol];
	uint8_t iR, iU, iL, iD;
	char cDir;
	//Sacar la distancia de cada una
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
		switch(cMapa[iNRow][iNCol]){
			case 'r':
				cDir = 's';
				break;
			case 'u':
				cDir = 'w';
				break;
			case 'l':
				cDir = 'n';
				break;
			case 'd':
				cDir = 'e';
				break;
		}
		iNCol++;
	}
	else if(iU <= iR && iU <= iL && iU <= iD){
		switch(cMapa[iNRow][iNCol]){
			case 'r':
				cDir = 'e';
				break;
			case 'u':
				cDir = 's';
				break;
			case 'l':
				cDir = 'w';
				break;
			case 'd':
				cDir = 'n';
				break;
		}
		iNRow--;
	}
	else if(iL <= iU && iL <= iR && iL <= iD){
		switch(cMapa[iNRow][iNCol]){
			case 'r':
				cDir = 'n';
				break;
			case 'u':
				cDir = 'e';
				break;
			case 'l':
				cDir = 's';
				break;
			case 'd':
				cDir = 'w';
				break;
		}
		iNCol--;
	}
	else{
		switch(cMapa[iNRow][iNCol]){
			case 'r':
				cDir = 'w';
				break;
			case 'u':
				cDir = 'n';
				break;
			case 'l':
				cDir = 'e';
				break;
			case 'd':
				cDir = 's';
				break;
		}
		iNRow++;
	}
	//Loop hasta que llegue a donde en realidad está
	while(cMapa[iNRow][iNCol] != 'i'){
		sIns += cMapa[iNRow][iNCol];
		switch(cDir){
			case 'n':
				switch(cMapa[iNRow][iNCol]){
					case 'r':
						cDir = 'w';
						break;
					case 'u':
						cDir = 'n';
						break;
					case 'l':
						cDir = 'e';
						break;
					case 'd':
						cDir = 's';
						break;
				}
				iNRow++;
				break;
			case 'e':
				switch(cMapa[iNRow][iNCol]){
					case 'r':
						cDir = 'n';
						break;
					case 'u':
						cDir = 'e';
						break;
					case 'l':
						cDir = 's';
						break;
					case 'd':
						cDir = 'w';
						break;
				}
				iNCol--;
				break;
			case 's':
				switch(cMapa[iNRow][iNCol]){
					case 'r':
						cDir = 'e';
						break;
					case 'u':
						cDir = 's';
						break;
					case 'l':
						cDir = 'w';
						break;
					case 'd':
						cDir = 'n';
						break;
				}
				iNRow--;
				break;
			case 'w':
				switch(cMapa[iNRow][iNCol]){
					case 'r':
						cDir = 's';
						break;
					case 'u':
						cDir = 'w';
						break;
					case 'l':
						cDir = 'n';
						break;
					case 'd':
						cDir = 'e';
						break;
				}
				iNCol++;
				break;
		}
	}
	return sIns;
}
