#include "Arduino.h"
#include "Tile.h"
#include "SensarRealidad.h"
#include "Mapear.h"
#include <Movimiento.h>

/*
cDir Direccion
n = norte
e = este
s = sur
w = oeste

cCase Caso
e = enfrente
d = derecha
i = izquierdo
NUNCA va a haber un caso atras
*/

//No sé si aquí crear una función que sea "Llena visitado" o eso mejor que sea para la clase moverse... Creo que en la clase moverse es más práctico, aunque realmente
//Por el nombre debería estar aquí... hmmm...

//Constructores
Mapear::Mapear(){
	iTamano = 10;
	iPisoMax = 0;
}
Mapear::Mapear(SensarRealidad *ma, Movimiento *ro){
	iTamano = 10;
	mapa = ma;
	robot = ro;
	iPisoMax = 0;
}

void Mapear::afterRampa(char cDir, uint8_t &iCol, uint8_t &iRow){
	switch(cDir){
		case 'n':
			iRow--;
			break;
		case 'e':
			iCol++;
			break;
		case 's':
			iRow++;
			break;
		case 'w':
			iCol--;	
			break;
	}
}

//Regresa TRUE si HAY espacio para llenar el dato
bool Mapear::espacio(char cDir, uint8_t iCol, uint8_t iRow, char cCase){
	switch(cDir){
		case 'n':
			switch(cCase){
				case 'e':
					return iRow > 0;
				case 'd':
					return iCol < iTamano-1;
				case 'i':
					return iCol > 0;
			}
		case 'e':
			switch(cCase){
				case 'e':
					return iCol < iTamano-1;
				case 'd':
					return iRow < iTamano-1;
				case 'i':
					return iRow > 0;
			}
		case 's':
			switch(cCase){
				case 'e':
					return iRow < iTamano-1;
				case 'd':
					return iCol > 0;
				case 'i':
					return iCol < iTamano-1;
			}
		case 'w':
			switch(cCase){
				case 'e':
					return iCol > 0;
				case 'd':
					return iRow > 0;
				case 'i':
					return iRow < iTamano-1;
			}
	}
}

//Desplaza los datos HACIA ABAJO
void Mapear::moverRowAbajo(Tile tMapa[3][10][10], uint8_t &iPiso){
	Tile tTemp1, tTemp2;
	for (uint8_t iCol = 0; iCol < iTamano; ++iCol)
	{	
		tTemp1 = tMapa[iPiso][iTamano-1][iCol];
		tTemp2 = tMapa[iPiso][0][iCol];
		tMapa[iPiso][0][iCol] = tMapa[iPiso][iTamano-1][iCol];
		for (uint8_t iRow = 0; iRow < iTamano-1; ++iRow)
		{
			tTemp1 = tTemp2;
			tTemp2 = tMapa[iPiso][iRow+1][iCol];
			tMapa[iPiso][iRow+1][iCol] = tTemp1;
		}
	}
	for(uint8_t iCol = 0; iCol < iTamano; iCol++){
		if(tMapa[iPiso][1][iCol].arriba()){
			tMapa[iPiso][0][iCol].abajo(true, NULL);
		}
	}
}

//Desplaza los datos HACIA ARRIBA
void Mapear::moverRowArr(Tile tMapa[3][10][10], uint8_t &iPiso){
	Tile tTemp1, tTemp2;
	for (uint8_t iCol = 0; iCol < iTamano; ++iCol)
	{
		tTemp1 = tMapa[iPiso][0][iCol];
		tTemp2 = tMapa[iPiso][iTamano-1][iCol];
		tMapa[iPiso][iTamano-1][iCol] = tMapa[iPiso][0][iCol];
		for (uint8_t iRow = iTamano-1; iRow > 0; --iRow)
		{
			tTemp1 = tTemp2;
			tTemp2 = tMapa[iPiso][iRow-1][iCol];
			tMapa[iPiso][iRow-1][iCol] = tTemp1;
		}
	}
	for(uint8_t iCol = 0; iCol < iTamano; iCol++){
		if(tMapa[iPiso][iTamano-2][iCol].abajo()){
			tMapa[iPiso][iTamano-1][iCol].arriba(true, NULL);
		}
	}
}

//Desplaza los datos HACIA LA IZQUIERDA
void Mapear::moverColIzq(Tile tMapa[3][10][10], uint8_t &iPiso){
	Tile tTemp1, tTemp2;
	for (uint8_t iRow = 0; iRow < iTamano; ++iRow)
	{
		tTemp1 = tMapa[iPiso][iRow][0];
		tTemp2 = tMapa[iPiso][iRow][iTamano-1];
		tMapa[iPiso][iRow][iTamano-1] = tMapa[iPiso][iRow][0];
		for (uint8_t iCol = iTamano-1; iCol > 0; --iCol)
		{
			tTemp1 = tTemp2;
			tTemp2 = tMapa[iPiso][iRow][iCol-1];
			tMapa[iPiso][iRow][iCol-1] = tTemp1;
		}
	}
	for(uint8_t iRow = 0; iRow < iTamano; iRow++){
		if(tMapa[iPiso][iRow][iTamano-2].derecha()){
			tMapa[iPiso][iRow][iTamano-1].izquierda(true, NULL);
		}
	}
}

//Desplaza los datos HACIA LA DERECHA
void Mapear::moverColDer(Tile tMapa[3][10][10], uint8_t &iPiso){
	Tile tTemp1, tTemp2;
	for (uint8_t iRow = 0; iRow < iTamano; ++iRow)
	{
		tTemp1 = tMapa[iPiso][iRow][iTamano-1];
		tTemp2 = tMapa[iPiso][iRow][0];
		tMapa[iPiso][iRow][0] = tMapa[iPiso][iRow][iTamano-1];
		for (uint8_t iCol = 0; iCol < iTamano-1; ++iCol)
		{
			tTemp1 = tTemp2;
			tTemp2 = tMapa[iPiso][iRow][iCol+1];
			tMapa[iPiso][iRow][iCol+1] = tTemp1;
		}
	}
	for(uint8_t iRow = 0; iRow < iTamano; iRow++){
		if(tMapa[iPiso][iRow][1].izquierda()){
			tMapa[iPiso][iRow][0].derecha(true, NULL);
		}
	}
}

//Llama a las cuatro funciones anteriores y decide cuál hacer y modifica la posición para que concuerde con el nuevo mapa->
void Mapear::desplazaDatos(Tile tMapa[3][10][10], char cDir, char cCase, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso){
	switch(cDir){
		case 'n':
			switch(cCase){
				case 'e': //Mover rows abajo
					moverRowAbajo(tMapa, iPiso);
					iRow++;
					break;
				case 'd': //Mover columnas a la izquierda
					moverColIzq(tMapa, iPiso);
					iCol--;
					break;
				case 'i': //Mover columnas a la derecha
					moverColDer(tMapa, iPiso);
					iCol++;
					break;
			}
			break;
		case 'e':
			switch(cCase){
				case 'e': //Mover columnas a la izquierda
					moverColIzq(tMapa, iPiso);
					iCol--;
					break;
				case 'd': //Mover rows arriba
					moverRowArr(tMapa, iPiso);
					iRow--;
					break;
				case 'i': //Mover rows abajo
					moverRowAbajo(tMapa, iPiso);
					iRow++;
					break;
			}
			break;
		case 's':
			switch(cCase){
				case 'e': //Mover rows arriba
					moverRowArr(tMapa, iPiso);
					iRow--;
					break;
				case 'd': //Mover columnas a la derecha
					moverColDer(tMapa, iPiso);
					iCol++;
					break;
				case 'i': //Mover columnas a la izquierda
					moverColIzq(tMapa, iPiso);
					iCol--;
					break;
			}
			break;
		case 'w':
			switch(cCase){
				case 'e': //Mover columnas a la derecha
					moverColDer(tMapa, iPiso);
					iCol++;
					break;
				case 'd': //Mover rows abajo
					moverRowAbajo(tMapa, iPiso);
					iRow++;
					break;
				case 'i': //Mover rows arriba
					moverRowArr(tMapa, iPiso);
					iRow--;
					break;
			}
			break;
	}
}

//Si es true, pone a ese cuadro como existente, si es false, le pone pared.
//Lo unico es que EL PRIMER CUADRO donde empiece el robot, debe ser marcado como existente manualmente
void Mapear::escribeMapaLoP(Tile tMapa[3][10][10], char cDir, uint8_t iCol, uint8_t iRow, char cCase, bool bLoP, uint8_t &iPiso){
	switch(cDir){
		case 'n':
			switch(cCase){
				case 'e':
					if(bLoP)
						tMapa[iPiso][iRow-1][iCol].existe(true);
					else{
						if(iRow > 0)
							tMapa[iPiso][iRow][iCol].arriba(true, &tMapa[iPiso][iRow-1][iCol]);
						else
							tMapa[iPiso][iRow][iCol].arriba(true, NULL);
					}
					break;
				case 'd':
					if(bLoP)
						tMapa[iPiso][iRow][iCol+1].existe(true);
					else{
						if(iCol < iTamano-1)
							tMapa[iPiso][iRow][iCol].derecha(true, &tMapa[iPiso][iRow][iCol+1]);
						else
							tMapa[iPiso][iRow][iCol].derecha(true, NULL);
					}
					break;
				case 'i':
					if(bLoP)
						tMapa[iPiso][iRow][iCol-1].existe(true);
					else{
						if(iCol > 0)
							tMapa[iPiso][iRow][iCol].izquierda(true, &tMapa[iPiso][iRow][iCol-1]);
						else
							tMapa[iPiso][iRow][iCol].izquierda(true, NULL);
					}
					break;
				case 'a':
					if(bLoP)
						tMapa[iPiso][iRow+1][iCol].existe(true);
					else{
						if(iRow < iTamano-1)
							tMapa[iPiso][iRow][iCol].abajo(true, &tMapa[iPiso][iRow+1][iCol]);
						else
							tMapa[iPiso][iRow][iCol].abajo(true, NULL);
					}
					break;
			}
			break;
		case 'e':
			switch(cCase){
				case 'e':
					if(bLoP)
						tMapa[iPiso][iRow][iCol+1].existe(true);
					else{
						if(iCol < iTamano-1)
							tMapa[iPiso][iRow][iCol].derecha(true, &tMapa[iPiso][iRow][iCol+1]);
						else
							tMapa[iPiso][iRow][iCol].derecha(true, NULL);
					}
					break;
				case 'd':
					if(bLoP)
						tMapa[iPiso][iRow+1][iCol].existe(true);
					else{
						if(iRow < iTamano-1)
							tMapa[iPiso][iRow][iCol].abajo(true, &tMapa[iPiso][iRow+1][iCol]);
						else
							tMapa[iPiso][iRow][iCol].abajo(true, NULL);
					}
					break;
				case 'i':
					if(bLoP)
						tMapa[iPiso][iRow-1][iCol].existe(true);
					else{
						if(iRow > 0)
							tMapa[iPiso][iRow][iCol].arriba(true, &tMapa[iPiso][iRow-1][iCol]);
						else
							tMapa[iPiso][iRow][iCol].arriba(true, NULL);
					}
					break;
				case 'a':
					if(bLoP)
						tMapa[iPiso][iRow][iCol-1].existe(true);
					else{
						if(iCol > 0)
							tMapa[iPiso][iRow][iCol].izquierda(true, &tMapa[iPiso][iRow][iCol-1]);
						else
							tMapa[iPiso][iRow][iCol].izquierda(true, NULL);
					}
					break;
			}
			break;
		case 's':
			switch(cCase){
				case 'e':
					if(bLoP)
						tMapa[iPiso][iRow+1][iCol].existe(true);
					else{
						if(iRow < iTamano-1)
							tMapa[iPiso][iRow][iCol].abajo(true, &tMapa[iPiso][iRow+1][iCol]);
						else
							tMapa[iPiso][iRow][iCol].abajo(true, NULL);
					}
					break;
				case 'd':
					if(bLoP)
						tMapa[iPiso][iRow][iCol-1].existe(true);
					else{
						if(iCol > 0)
							tMapa[iPiso][iRow][iCol].izquierda(true, &tMapa[iPiso][iRow][iCol-1]);
						else
							tMapa[iPiso][iRow][iCol].izquierda(true, NULL);
					}
					break;
				case 'i':
					if(bLoP)
						tMapa[iPiso][iRow][iCol+1].existe(true);
					else{
						if(iCol < iTamano-1)
							tMapa[iPiso][iRow][iCol].derecha(true, &tMapa[iPiso][iRow][iCol+1]);
						else
							tMapa[iPiso][iRow][iCol].derecha(true, NULL);
					}
					break;
				case 'a':
					if(bLoP)
						tMapa[iPiso][iRow-1][iCol].existe(true);
					else{
						if(iRow > 0)
							tMapa[iPiso][iRow][iCol].arriba(true, &tMapa[iPiso][iRow-1][iCol]);
						else
							tMapa[iPiso][iRow][iCol].arriba(true, NULL);
					}
					break;
			}
			break;
		case 'w':
			switch(cCase){
				case 'e':
					if(bLoP)
						tMapa[iPiso][iRow][iCol-1].existe(true);
					else{
						if(iCol > 0)
							tMapa[iPiso][iRow][iCol].izquierda(true, &tMapa[iPiso][iRow][iCol-1]);
						else
							tMapa[iPiso][iRow][iCol].izquierda(true, NULL);
					}
					break;
				case 'd':
					if(bLoP)
						tMapa[iPiso][iRow-1][iCol].existe(true);
					else{
						if(iRow > 0)
							tMapa[iPiso][iRow][iCol].arriba(true, &tMapa[iPiso][iRow-1][iCol]);
						else
							tMapa[iPiso][iRow][iCol].arriba(true, NULL);
					}
					break;
				case 'i':
					if(bLoP)
						tMapa[iPiso][iRow+1][iCol].existe(true);
					else{
						if(iRow < iTamano-1)
							tMapa[iPiso][iRow][iCol].abajo(true, &tMapa[iPiso][iRow+1][iCol]);
						else
							tMapa[iPiso][iRow][iCol].abajo(true, NULL);
					}
					break;
				case 'a':
					if(bLoP)
						tMapa[iPiso][iRow][iCol+1].existe(true);
					else{
						if(iCol < iTamano-1)
							tMapa[iPiso][iRow][iCol].derecha(true, &tMapa[iPiso][iRow][iCol+1]);
						else
							tMapa[iPiso][iRow][iCol].derecha(true, NULL);
					}
					break;
			}
			break;
	}
}

//Llena el mapa dependiendo de los valores que mande la clase SensarMapa. Y en dado caso, desplaza los datos.
//Como sólo sensa derecha, enfrente y atrás, es necesario en el primer cuadro dar una vuelta de 90 para sensar el cuadro de atrás.
void Mapear::llenaMapa(Tile tMapa[3][10][10], char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso){
	/*for(int i=0; i<100; i++){
    	Serial.println(mapa->sensarRampa());
	}
	delay(1000);*/
    //Serial.println("RAMPA?");
    //delay(5000);
    //char inChar = (char)Serial.read();
    //if(inChar == 'r'){
	if(mapa->sensarRampa() > 17 || mapa->sensarRampa() < -17){
		if(tMapa[iPiso][iRow][iCol].rampaAbajo() || tMapa[iPiso][iRow][iCol].rampaArriba()){
			uint8_t iTemp = iPiso;
			robot->pasaRampa(cDir);
			iPiso = tMapa[iPiso][iRow][iCol].piso();
			//Serial.print("ESTOY EN: "); Serial.println(iTemp);
			//Serial.print("QUIERO IR A: "); Serial.println(iPiso);
			uint8_t i = 0;
			uint8_t j = 0;
			bool bT = false;
			//Serial.print("Columna = "); Serial.print(iCol); Serial.print(" Row = "); Serial.print(iRow); Serial.print(" Piso = "); Serial.print(iPiso); Serial.print(" Direccion = "); Serial.println(cDir);
			while(i < iTamano && bT == false){
				j = 0;
				while(j < iTamano && bT == false){
					if( (tMapa[iPiso][i][j].rampaAbajo() || tMapa[iPiso][i][j].rampaArriba()) && tMapa[iPiso][i][j].piso() == iTemp){ 
						//Aquí busca dónde está esa rampa en el mapa al que va y pone las coordenadas ahí
						iCol = j;
						iRow = i;
						bT = true;
						//Serial.println("ENCONTRAMOS ALGO");
						//Serial.print("Columna = "); Serial.print(iCol); Serial.print(" Row = "); Serial.print(iRow); Serial.print(" Piso = "); Serial.print(iPiso); Serial.print(" Direccion = "); Serial.println(cDir);
					}
					//Serial.print(tMapa[iPiso][i][j].piso());
					j++;
				}
				//Serial.println(" ");
				i++;
			}
			afterRampa(cDir, iCol, iRow);
			//Serial.print("Columna = "); Serial.print(iCol); Serial.print(" Row = "); Serial.print(iRow); Serial.print(" Piso = "); Serial.print(iPiso); Serial.print(" Direccion = "); Serial.println(cDir);
			//Serial.println("ME SALGO");
		}
		else{
		//Modifica el piso maximo
		//Serial.print("iPisoMax = ");
		//Serial.println(iPisoMax);
		iPisoMax++;
		//Serial.print("iPisoMax = ");
		//Serial.println(iPisoMax);
		//delay(2000);
		//Pone actual como RampaArriba o Abajo e inicializa el segundo piso
		//Serial.println("RAMPA ARRIBA O ABAJO?");
		//delay(1000);
		//inChar = (char)Serial.read();
		//if(inChar == 'u'){
		if(mapa->sensarRampa() > 18){
			tMapa[iPiso][iRow][iCol].rampaArriba(true);
			tMapa[iPisoMax][4][4].rampaAbajo(true);
		}
		//if(inChar == 'd'){
		if(mapa->sensarRampa() < -20){
			tMapa[iPiso][iRow][iCol].rampaAbajo(true);
			tMapa[iPisoMax][4][4].rampaArriba(true);
		}
		//Pone a qué piso conectan
		tMapa[iPisoMax][4][4].piso(iPiso);
		tMapa[iPisoMax][4][4].existe(true);
		tMapa[iPisoMax][4][4].visitado(true);
		tMapa[iPiso][iRow][iCol].piso(iPisoMax);
		//Serial.print("CONECTO A: "); Serial.println(tMapa[iPiso][iRow][iCol].piso()); 
		tMapa[iPiso][iRow][iCol].visitado(true);
		//Poner como camino cerrado del piso actual
		escribeMapaLoP(tMapa, cDir, iCol, iRow, 'e', false, iPiso);
		escribeMapaLoP(tMapa, cDir, iCol, iRow, 'd', false, iPiso);
		escribeMapaLoP(tMapa, cDir, iCol, iRow, 'i', false, iPiso);
		//Pone como cerrado MENOS a donde está viendo
		/*escribeMapaLoP(tMapa, cDir, 4, 4, 'a', false, iPisoMax);
		escribeMapaLoP(tMapa, cDir, 4, 4, 'd', false, iPisoMax);
		escribeMapaLoP(tMapa, cDir, 4, 4, 'i', false, iPisoMax);*/
		//Prende motores y los apaga cuando acaba de subir o bajar
		//Serial.println("LISTO");
		//delay(5000);
		robot->pasaRampa(cDir);
		//Pone la posición en after rampa dependiendo de la direccion
		iPiso = iPisoMax;
		iCol = iRow = 4;
		afterRampa(cDir, iCol, iRow);
		}
	}
	/*Serial.println("Sensa Enfrente");
	delay(1000);
	inChar = (char)Serial.read();
	if(inChar == 'l'){
		Serial.println("LIBRE ENFRENTE");
		if(!espacio(cDir, iCol, iRow, 'e')){
			desplazaDatos(tMapa, cDir, 'e', iCol, iRow, iPiso);
		}
		escribeMapaLoP(tMapa, cDir, iCol, iRow, 'e', true, iPiso);
	}
	else//Pone pared
		escribeMapaLoP(tMapa, cDir, iCol, iRow, 'e', false, iPiso);
	Serial.println("Sensa Derecha");
	delay(1000);
	inChar = (char)Serial.read();
	if(inChar == 'l'){
		Serial.println("LIBRE DERECHA");
		if(!espacio(cDir, iCol, iRow, 'd')){
			Serial.println("NO HAY ESPACIO");
			desplazaDatos(tMapa, cDir, 'd', iCol, iRow, iPiso);
		}
		else
			Serial.println("HAY ESPACIO Y SOY UN MENTIROSO");
		escribeMapaLoP(tMapa, cDir, iCol, iRow, 'd', true, iPiso);
	}
	else
		escribeMapaLoP(tMapa, cDir, iCol, iRow, 'd', false, iPiso);
	Serial.println("Sensa Izquierda");
	delay(1000);
	inChar = (char)Serial.read();
	if(inChar == 'l'){
		Serial.println("LIBRE IZQUIERDA");
		if(!espacio(cDir, iCol, iRow, 'i')){
			desplazaDatos(tMapa, cDir, 'i', iCol, iRow, iPiso);
		}
		escribeMapaLoP(tMapa, cDir, iCol, iRow, 'i', true, iPiso);
	}
	else
		escribeMapaLoP(tMapa, cDir, iCol, iRow, 'i', false, iPiso);*/
	//Serial.println("Sensa Enfrente");
	////delay(2000);
	if(mapa->sensarEnfrente()){
		if(!espacio(cDir, iCol, iRow, 'e')){
			desplazaDatos(tMapa, cDir, 'e', iCol, iRow, iPiso);
		}
		escribeMapaLoP(tMapa, cDir, iCol, iRow, 'e', true, iPiso);
	}
	else//Pone pared
		escribeMapaLoP(tMapa, cDir, iCol, iRow, 'e', false, iPiso);
	//Serial.println("Sensa Derecha");
	////delay(2000);
	if(mapa->sensarDerecha()){
		if(!espacio(cDir, iCol, iRow, 'd')){
			desplazaDatos(tMapa, cDir, 'd', iCol, iRow, iPiso);
		}
		escribeMapaLoP(tMapa, cDir, iCol, iRow, 'd', true, iPiso);
	}
	else
		escribeMapaLoP(tMapa, cDir, iCol, iRow, 'd', false, iPiso);
	//Serial.println("Sensa Izquierda");
	////delay(2000);
	if(mapa->sensarIzquierda()){
		if(!espacio(cDir, iCol, iRow, 'i')){
			desplazaDatos(tMapa, cDir, 'i', iCol, iRow, iPiso);
		}
		escribeMapaLoP(tMapa, cDir, iCol, iRow, 'i', true, iPiso);
	}
	else
		escribeMapaLoP(tMapa, cDir, iCol, iRow, 'i', false, iPiso);
	//Si es un cuadro negro
	if(mapa->prueba() == 1){
		if(mapa->prueba() == 1){
			//Serial.println("COLOR");
			//Poner pared a los cuatro lados
			escribeMapaLoP(tMapa, cDir, iCol, iRow, 'e', false, iPiso);
			escribeMapaLoP(tMapa, cDir, iCol, iRow, 'd', false, iPiso);
			escribeMapaLoP(tMapa, cDir, iCol, iRow, 'i', false, iPiso);
			escribeMapaLoP(tMapa, cDir, iCol, iRow, 'a', false, iPiso);
			tMapa[iPiso][iRow][iCol].cuadroNegro(true);
		}
	}
}