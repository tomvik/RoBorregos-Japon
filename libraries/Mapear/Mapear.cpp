#include <Arduino.h>
#include <Mapear.h>

///////////Dimensiones///////////////////
const uint8_t kMapSize = 15;
const uint8_t kMapFloors = 4;
//////////Constant Rampa////////////////
const uint8_t kRampaLimit = 17;

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

// No sé si aquí crear una función que sea "Llena visitado" o eso mejor que sea para la clase moverse... Creo que en la clase moverse es más práctico, aunque realmente
// Por el nombre debería estar aquí... hmmm...

// Constructores
Mapear::Mapear() {
	iColor = 0;
}

Mapear::Mapear(SensarRealidad *ma, Movimiento *ro, uint8_t *iPM) {
	mapa = ma;
	robot = ro;
	iColor = 0;
	iPisoMax = iPM;
}

void Mapear::afterRampa(char cDir, uint8_t &iCol, uint8_t &iRow) {
	switch(cDir) {
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

// Regresa TRUE si HAY espacio para llenar el dato
bool Mapear::espacio(char cDir, uint8_t iCol, uint8_t iRow, char cCase) {
	switch(cDir) {
	case 'n':
		switch(cCase) {
		case 'e':
			return iRow > 0;
		case 'd':
			return iCol < kMapSize-1;
		case 'i':
			return iCol > 0;
		}
	case 'e':
		switch(cCase) {
		case 'e':
			return iCol < kMapSize-1;
		case 'd':
			return iRow < kMapSize-1;
		case 'i':
			return iRow > 0;
		}
	case 's':
		switch(cCase) {
		case 'e':
			return iRow < kMapSize-1;
		case 'd':
			return iCol > 0;
		case 'i':
			return iCol < kMapSize-1;
		}
	case 'w':
		switch(cCase) {
		case 'e':
			return iCol > 0;
		case 'd':
			return iRow > 0;
		case 'i':
			return iRow < kMapSize-1;
		}
	}
	return false;
}

// Desplaza los datos HACIA ABAJO
void Mapear::moverRowAbajo(Tile tMapa[kMapFloors][kMapSize][kMapSize], uint8_t &iPiso) {
	Tile tTemp1, tTemp2;
	for (int iCol = 0; iCol < kMapSize; ++iCol){
		tTemp1 = tMapa[iPiso][kMapSize-1][iCol];
		tTemp2 = tMapa[iPiso][0][iCol];
		tMapa[iPiso][0][iCol] = tTemp1;
		if(tTemp2.arriba())
			tMapa[iPiso][0][iCol].abajo(true, NULL);
		for (int iRow = 1; iRow < kMapSize; ++iRow){
			tTemp1 = tTemp2;
			tTemp2 = tMapa[iPiso][iRow][iCol];
			tMapa[iPiso][iRow][iCol] = tTemp1;
		}
	}
}

// Desplaza los datos HACIA ARRIBA
void Mapear::moverRowArr(Tile tMapa[kMapFloors][kMapSize][kMapSize], uint8_t &iPiso) {
	Tile tTemp1, tTemp2;
	for (int iCol = 0; iCol < kMapSize; ++iCol){
		tTemp1 = tMapa[iPiso][0][iCol];
		tTemp2 = tMapa[iPiso][kMapSize-1][iCol];
		tMapa[iPiso][kMapSize-1][iCol] = tTemp1;
		if(tTemp2.abajo())
			tMapa[iPiso][kMapSize-1][iCol].arriba(true, NULL);
		for (int iRow = kMapSize-2; iRow >= 0; --iRow){
			tTemp1 = tTemp2;
			tTemp2 = tMapa[iPiso][iRow][iCol];
			tMapa[iPiso][iRow][iCol] = tTemp1;
		}
	}
}

// Desplaza los datos HACIA LA IZQUIERDA
void Mapear::moverColIzq(Tile tMapa[kMapFloors][kMapSize][kMapSize], uint8_t &iPiso) {
	Tile tTemp1, tTemp2;
	for (int iRow = 0; iRow < kMapSize; ++iRow){
		tTemp1 = tMapa[iPiso][iRow][0];
		tTemp2 = tMapa[iPiso][iRow][kMapSize-1];
		tMapa[iPiso][iRow][kMapSize-1] = tTemp1;
		if(tTemp2.derecha())
			tMapa[iPiso][iRow][kMapSize-1].izquierda(true, NULL);
		for (int iCol = kMapSize-2; iCol >= 0; --iCol){
			tTemp1 = tTemp2;
			tTemp2 = tMapa[iPiso][iRow][iCol];
			tMapa[iPiso][iRow][iCol] = tTemp1;
		}
	}
}

// Desplaza los datos HACIA LA DERECHA
void Mapear::moverColDer(Tile tMapa[kMapFloors][kMapSize][kMapSize], uint8_t &iPiso) {
	Tile tTemp1, tTemp2;
	for (int iRow = 0; iRow < kMapSize; ++iRow){
		tTemp1 = tMapa[iPiso][iRow][kMapSize-1];
		tTemp2 = tMapa[iPiso][iRow][0];
		tMapa[iPiso][iRow][0] = tTemp1;
		if(tTemp2.izquierda())
			tMapa[iPiso][iRow][0].derecha(true, NULL);
		for (int iCol = 1; iCol < kMapSize; ++iCol){
			tTemp1 = tTemp2;
			tTemp2 = tMapa[iPiso][iRow][iCol];
			tMapa[iPiso][iRow][iCol] = tTemp1;
		}
	}
}

// Llama a las cuatro funciones anteriores y decide cuál hacer y modifica la posición para que concuerde con el nuevo mapa->
void Mapear::desplazaDatos(Tile tMapa[kMapFloors][kMapSize][kMapSize], char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso, char cCase) {
	switch(cDir) {
	case 'n':
		switch(cCase) {
		case 'e':                 // Mover rows abajo
			moverRowAbajo(tMapa, iPiso);
			iRow++;
			break;
		case 'd':                 // Mover columnas a la izquierda
			moverColIzq(tMapa, iPiso);
			iCol--;
			break;
		case 'i':                 // Mover columnas a la derecha
			moverColDer(tMapa, iPiso);
			iCol++;
			break;
		}
		break;
	case 'e':
		switch(cCase) {
		case 'e':                 // Mover columnas a la izquierda
			moverColIzq(tMapa, iPiso);
			iCol--;
			break;
		case 'd':                 // Mover rows arriba
			moverRowArr(tMapa, iPiso);
			iRow--;
			break;
		case 'i':                 // Mover rows abajo
			moverRowAbajo(tMapa, iPiso);
			iRow++;
			break;
		}
		break;
	case 's':
		switch(cCase) {
		case 'e':                 // Mover rows arriba
			moverRowArr(tMapa, iPiso);
			iRow--;
			break;
		case 'd':                 // Mover columnas a la derecha
			moverColDer(tMapa, iPiso);
			iCol++;
			break;
		case 'i':                 // Mover columnas a la izquierda
			moverColIzq(tMapa, iPiso);
			iCol--;
			break;
		}
		break;
	case 'w':
		switch(cCase) {
		case 'e':                 // Mover columnas a la derecha
			moverColDer(tMapa, iPiso);
			iCol++;
			break;
		case 'd':                 // Mover rows abajo
			moverRowAbajo(tMapa, iPiso);
			iRow++;
			break;
		case 'i':                 // Mover rows arriba
			moverRowArr(tMapa, iPiso);
			iRow--;
			break;
		}
		break;
	}
}

// Si es true, pone a ese cuadro como existente, si es false, le pone pared.
// Lo unico es que EL PRIMER CUADRO donde empiece el robot, debe ser marcado como existente manualmente
void Mapear::escribeMapaLoP(Tile tMapa[kMapFloors][kMapSize][kMapSize], char cDir, uint8_t iCol, uint8_t iRow, uint8_t &iPiso, char cCase, bool bLoP) {
	switch(cDir) {
	case 'n':
		switch(cCase) {
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
				if(iCol < kMapSize-1)
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
				if(iRow < kMapSize-1)
					tMapa[iPiso][iRow][iCol].abajo(true, &tMapa[iPiso][iRow+1][iCol]);
				else
					tMapa[iPiso][iRow][iCol].abajo(true, NULL);
			}
			break;
		}
		break;
	case 'e':
		switch(cCase) {
		case 'e':
			if(bLoP)
				tMapa[iPiso][iRow][iCol+1].existe(true);
			else{
				if(iCol < kMapSize-1)
					tMapa[iPiso][iRow][iCol].derecha(true, &tMapa[iPiso][iRow][iCol+1]);
				else
					tMapa[iPiso][iRow][iCol].derecha(true, NULL);
			}
			break;
		case 'd':
			if(bLoP)
				tMapa[iPiso][iRow+1][iCol].existe(true);
			else{
				if(iRow < kMapSize-1)
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
		switch(cCase) {
		case 'e':
			if(bLoP)
				tMapa[iPiso][iRow+1][iCol].existe(true);
			else{
				if(iRow < kMapSize-1)
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
				if(iCol < kMapSize-1)
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
		switch(cCase) {
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
				if(iRow < kMapSize-1)
					tMapa[iPiso][iRow][iCol].abajo(true, &tMapa[iPiso][iRow+1][iCol]);
				else
					tMapa[iPiso][iRow][iCol].abajo(true, NULL);
			}
			break;
		case 'a':
			if(bLoP)
				tMapa[iPiso][iRow][iCol+1].existe(true);
			else{
				if(iCol < kMapSize-1)
					tMapa[iPiso][iRow][iCol].derecha(true, &tMapa[iPiso][iRow][iCol+1]);
				else
					tMapa[iPiso][iRow][iCol].derecha(true, NULL);
			}
			break;
		}
		break;
	}
}

// Llena el mapa dependiendo de los valores que mande la clase SensarMapa. Y en dado caso, desplaza los datos.
// Como sólo sensa derecha, enfrente y atrás, es necesario en el primer cuadro dar una vuelta de 90 para sensar el cuadro de atrás.
void Mapear::llenaMapaVariable(Tile tMapa[kMapFloors][kMapSize][kMapSize], char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso) {
	if(mapa->sensarRampa() > kRampaLimit || mapa->sensarRampa() < -kRampaLimit) {
		if(tMapa[iPiso][iRow][iCol].rampaAbajo() || tMapa[iPiso][iRow][iCol].rampaArriba()) {
			uint8_t iTemp = iPiso, i = 0, j;
			robot->pasaRampa();
			iPiso = tMapa[iPiso][iRow][iCol].piso();
			bool bT = false;
			while(i < kMapSize && !bT) {
				j = 0;
				while(j < kMapSize && !bT) {
					if( (tMapa[iPiso][i][j].rampaAbajo() || tMapa[iPiso][i][j].rampaArriba()) && tMapa[iPiso][i][j].piso() == iTemp) {
						// Aquí busca dónde está esa rampa en el mapa al que va y pone las coordenadas ahí
						iCol = j;
						iRow = i;
						bT = true;
					}
					j++;
				}
				i++;
			}
			afterRampa(cDir, iCol, iRow);
		}
		else{
			// Modifica el piso maximo
			(*iPisoMax)++;
			if(mapa->sensarRampa() > kRampaLimit) {
				tMapa[iPiso][iRow][iCol].rampaArriba(true);
				tMapa[(*iPisoMax)][4][4].rampaAbajo(true);
			}
			else{
				tMapa[iPiso][iRow][iCol].rampaAbajo(true);
				tMapa[(*iPisoMax)][4][4].rampaArriba(true);
			}
			// Pone a qué piso conectan
			tMapa[(*iPisoMax)][4][4].piso(iPiso);
			tMapa[(*iPisoMax)][4][4].existe(true);
			tMapa[(*iPisoMax)][4][4].visitado(true);
			tMapa[iPiso][iRow][iCol].piso((*iPisoMax));
			tMapa[iPiso][iRow][iCol].visitado(true);
			// Poner como camino cerrado del piso actual
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', false);
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', false);
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', false);
			robot->pasaRampa();
			// Pone la posición en after rampa dependiendo de la direccion
			iPiso = (*iPisoMax);
			iCol = iRow = 4;
			afterRampa(cDir, iCol, iRow);
		}
		// TODO NETO
		// robot->stop();
		if(mapa->caminoEnfrente()) {
			if(!espacio(cDir, iCol, iRow, 'e'))
				desplazaDatos(tMapa, cDir, iCol, iRow, iPiso, 'e');
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', true);
		}
		// Pared
		else
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', false);
		// Lectura Derecha
		// Libre
		if(mapa->caminoDerecha()) {
			if(!espacio(cDir, iCol, iRow, 'd'))
				desplazaDatos(tMapa, cDir, iCol, iRow, iPiso, 'd');
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', true);
		}
		// Pared
		else
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', false);
		// Lectura I<quierda
		// Libre
		if(mapa->caminoIzquierda()) {
			if(!espacio(cDir, iCol, iRow, 'i'))
				desplazaDatos(tMapa, cDir, iCol, iRow, iPiso, 'i');
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', true);
		}
		// Pared
		else
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', false);
	}
	else{
		if(!(robot->getParedes()&0b00000010)) {
			if(!espacio(cDir, iCol, iRow, 'e')) {
				desplazaDatos(tMapa, cDir, iCol, iRow, iPiso, 'e');
			}
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', true);
		}
		// Pared
		else
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', false);
		// Lectura Derecha
		// Libre
		if(!(robot->getParedes()&0b00000001)) {
			if(!espacio(cDir, iCol, iRow, 'd')) {
				desplazaDatos(tMapa, cDir, iCol, iRow, iPiso, 'd');
			}
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', true);
		}
		// Pared
		else
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', false);
		// Lectura I<quierda
		// Libre
		if(!(robot->getParedes()&0b00000100)) {
			if(!espacio(cDir, iCol, iRow, 'i')) {
				desplazaDatos(tMapa, cDir, iCol, iRow, iPiso, 'i');
			}
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', true);
		}
		// Pared
		else
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', false);
	}
	// Si es un cuadro negro
	iColor = robot->getColor();
	if(iColor == 1) {
		mapa->apantallanteLCD("NEGRO");
		// delay(200);
		// Poner pared a los cuatro lados
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', false);
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', false);
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', false);
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'a', false);
		tMapa[iPiso][iRow][iCol].cuadroNegro(true);
	}
}

void Mapear::llenaMapaSensor(Tile tMapa[kMapFloors][kMapSize][kMapSize], char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso) {
	if(mapa->sensarRampa() > kRampaLimit || mapa->sensarRampa() < -kRampaLimit) {
		// tMapa[iPiso][iRow][iCol].bumper(false);
		if(tMapa[iPiso][iRow][iCol].rampaAbajo() || tMapa[iPiso][iRow][iCol].rampaArriba()) {
			uint8_t iTemp = iPiso, i = 0, j;
			robot->pasaRampa();
			iPiso = tMapa[iPiso][iRow][iCol].piso();
			bool bT = false;
			while(i < kMapSize && !bT) { // CAMBIO AQUI DE bT == false a !bT
				j = 0;
				while(j < kMapSize && !bT) {
					if( (tMapa[iPiso][i][j].rampaAbajo() || tMapa[iPiso][i][j].rampaArriba()) && tMapa[iPiso][i][j].piso() == iTemp) {
						// Aquí busca dónde está esa rampa en el mapa al que va y pone las coordenadas ahí
						iCol = j;
						iRow = i;
						bT = true;
					}
					j++;
				}
				i++;
			}
			afterRampa(cDir, iCol, iRow);
		}
		else{
			// Modifica el piso maximo
			(*iPisoMax)++;
			if(mapa->sensarRampa() > kRampaLimit) {
				tMapa[iPiso][iRow][iCol].rampaArriba(true);
				tMapa[(*iPisoMax)][4][4].rampaAbajo(true);
			}
			if(mapa->sensarRampa() < -kRampaLimit) {
				tMapa[iPiso][iRow][iCol].rampaAbajo(true);
				tMapa[(*iPisoMax)][4][4].rampaArriba(true);
			}
			// Pone a qué piso conectan
			tMapa[(*iPisoMax)][4][4].piso(iPiso);
			tMapa[(*iPisoMax)][4][4].existe(true);
			tMapa[(*iPisoMax)][4][4].visitado(true);
			tMapa[iPiso][iRow][iCol].piso((*iPisoMax));
			tMapa[iPiso][iRow][iCol].visitado(true);
			// Poner como camino cerrado del piso actual
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', false);
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', false);
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', false);
			robot->pasaRampa();
			// Pone la posición en after rampa dependiendo de la direccion
			iPiso = (*iPisoMax);
			iCol = iRow = 4;
			afterRampa(cDir, iCol, iRow);
		}
		// TODO NETO
		// robot->stop();
	}
	/////////////////////////////NORMAL//////////////////////////////////
	if(mapa->caminoEnfrente()) {
		if(!espacio(cDir, iCol, iRow, 'e')) {
			desplazaDatos(tMapa, cDir, iCol, iRow, iPiso, 'e');
		}
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', true);
	}
	// Pared
	else
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', false);
	// Lectura Derecha
	// Libre
	if(mapa->caminoDerecha()) {
		if(!espacio(cDir, iCol, iRow, 'd')) {
			desplazaDatos(tMapa, cDir, iCol, iRow, iPiso, 'd');
		}
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', true);
	}
	// Pared
	else
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', false);
	// Lectura I<quierda
	// Libre
	if(mapa->caminoIzquierda()) {
		if(!espacio(cDir, iCol, iRow, 'i')) {
			desplazaDatos(tMapa, cDir, iCol, iRow, iPiso, 'i');
		}
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', true);
	}
	// Pared
	else
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', false);
	// Si es un cuadro negro
	iColor = mapa->color();
	// mapa->apantallanteLCD(String(iColor));
	if(iColor == 1) {
		// mapa->apantallanteLCD("NEGRO");
		// delay(200);
		// Poner pared a los cuatro lados
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', false);
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', false);
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', false);
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'a', false);
		tMapa[iPiso][iRow][iCol].cuadroNegro(true);
	}
	////////////////////////////// PRUEBAS DE LOGICA/////////////////////////////////////////
	/*char inChar;
	   Serial.println("Sensa Enfrente");
	   // Delay(2000);
	   inChar = (char)Serial.read();
	   if(inChar == 'l') {
	        Serial.println("LIBRE ENFRENTE");
	        if(!espacio(cDir, iCol, iRow, 'e')) {
	                desplazaDatos(tMapa, cDir, iCol, iRow, iPiso, 'e');
	        }
	        escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', true);
	   }
	   else// Pone pared
	        escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', false);
	   Serial.println("Sensa Derecha");
	   // Delay(2000);
	   inChar = (char)Serial.read();
	   if(inChar == 'l') {
	        Serial.println("LIBRE DERECHA");
	        if(!espacio(cDir, iCol, iRow, 'd')) {
	                desplazaDatos(tMapa, cDir, iCol, iRow, iPiso, 'd');
	        }
	        escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', true);
	   }
	   else
	        escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', false);
	   Serial.println("Sensa Izquierda");
	   // Delay(2000);
	   inChar = (char)Serial.read();
	   if(inChar == 'l') {
	        Serial.println("LIBRE IZQUIERDA");
	        if(!espacio(cDir, iCol, iRow, 'i')) {
	                desplazaDatos(tMapa, cDir, iCol, iRow, iPiso, 'i');
	        }
	        escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', true);
	   }
	   else
	        escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', false);*/

	//mapa->escribirLCD(String(tMapa[iPiso][iRow][iCol].arriba()) + " " + String(tMapa[iPiso][iRow][iCol].derecha()) + " " + String(tMapa[iPiso][iRow][iCol].abajo()) + " " + String(tMapa[iPiso][iRow][iCol].izquierda()));
	// delay(500);
}
