/* Code made by the RoBorregos team in 2017 for the RoboCup JR. Rescue Maze category.
 * Tomás Lugo, Sebastián Esquer, Ernesto Cervantez, and Alexis Virgen.
 * "El Mariachi" Achieved a third place on the international RoboCup.
 *
 * Since the size of the matrix is fixed, sometimes we need to shift the data
 * So we don't get out of bounds. Also, we have two modes of mapping.
 * The "normal" being with the sensors: The procedure is: Reach destination -> stop -> sense all at once (one or two times) -> map
 *
 * The other one (Which I prefer but doesn't apply always. See special cases below) is way faster and is redundant (that is good)
 * The procedure is: While reaching the destination, mark how many times there was a wall -> if n > threshold, there's a wall -> map with variables
 *
 * Special cases: When the robot doesn't move (first tile), or finishes going through a ramp (bug fixed)
 *
 * Also... once you say there's a wall, there's no way to say you were wrong (never implemented that on class Tile), sometimes it was needed, but it was rarely.
 */
#include <Arduino.h>
#include <Mapear.h>

///////////Dimensions///////////////////
const uint8_t kMapSize = 15;
const uint8_t kMapFloors = 4;
//////////Constant Limit of inclination for the ramp////////////////
const uint8_t kRampaLimit = 17;

/*
   cDir Direction
   n = North
   e = East
   s = South
   w = West
   cCase Case
   e = Front
   d = Right
   i = Left
   There's NEVER goint to be a "Back" case
*/
// I don't know if I should have a function "Fill visited" or that's better on the class "Movimiento"... In there is more practical, but in here
// it should be... hmmm...

///////////Constructors///////////////////
Mapear::Mapear() {
	iColor = 0;
}

Mapear::Mapear(SensarRealidad *ma, Movimiento *ro, uint8_t *iPM) {
	mapa = ma;
	robot = ro;
	iColor = 0;
	iPisoMax = iPM;
}

////////////////Shifting the data of the matrix//////////////////
//Returns true if there's space on the matrix (If you're on map[0][14] and want to put something on the right, there's no space)
// Micro-optimization, declare a constant for "kMapSize-1"
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

//Downwards
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

//Upwards
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

//Left
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

//Right
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

//Calls the above functions and updates the position
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
//Handles the new position after the ramp
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

//Depending on the boolean, writes if there's a wall or not. If true, there's a path. If false, there's a wall
//Only the FIRST TILE must be modified manually
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

//Fills the map with the variables sent by the object robot. It also handles the ramp thing, should be another function called "Before ramp"
//Only senses Left, Right and Forward. On the firs tile we need to manually check the sensor on the back.
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
						// I look where the ramp is and map it.
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
			// Modifies the max floor
			(*iPisoMax)++;
			if(mapa->sensarRampa() > kRampaLimit) {
				tMapa[iPiso][iRow][iCol].rampaArriba(true);
				tMapa[(*iPisoMax)][4][4].rampaAbajo(true);
			}
			else{
				tMapa[iPiso][iRow][iCol].rampaAbajo(true);
				tMapa[(*iPisoMax)][4][4].rampaArriba(true);
			}
			// IT maps where it connects
			tMapa[(*iPisoMax)][4][4].piso(iPiso);
			tMapa[(*iPisoMax)][4][4].existe(true);
			tMapa[(*iPisoMax)][4][4].visitado(true);
			tMapa[iPiso][iRow][iCol].piso((*iPisoMax));
			tMapa[iPiso][iRow][iCol].visitado(true);
			// It closes the path on the current floor
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', false);
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', false);
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', false);
			robot->pasaRampa();
			// Moves the position to where it's going to be
			iPiso = (*iPisoMax);
			iCol = iRow = 4;
			afterRampa(cDir, iCol, iRow);
		}
		//Fixes the bug of not the after ramp and variables
		//Front
		if(mapa->caminoEnfrente()) {
			if(!espacio(cDir, iCol, iRow, 'e'))
				desplazaDatos(tMapa, cDir, iCol, iRow, iPiso, 'e');
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', true);
		}
		else
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', false);
		//Right
		if(mapa->caminoDerecha()) {
			robot->llenaArreglo(false);
			if(!espacio(cDir, iCol, iRow, 'd'))
				desplazaDatos(tMapa, cDir, iCol, iRow, iPiso, 'd');
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', true);
		}
		else{
			robot->llenaArreglo(true);
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', false);
		}
		//Left
		if(mapa->caminoIzquierda()) {
			if(!espacio(cDir, iCol, iRow, 'i'))
				desplazaDatos(tMapa, cDir, iCol, iRow, iPiso, 'i');
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', true);
		}
		else
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', false);
	}
	//If everything's normal
	else{
		//Front
		if(!(robot->getParedes()&0b00000010)) {
			if(!espacio(cDir, iCol, iRow, 'e')) {
				desplazaDatos(tMapa, cDir, iCol, iRow, iPiso, 'e');
			}
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', true);
		}
		else
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', false);
		//Right
		if(!(robot->getParedes()&0b00000001)) {
			robot->llenaArreglo(false);
			if(!espacio(cDir, iCol, iRow, 'd')) {
				desplazaDatos(tMapa, cDir, iCol, iRow, iPiso, 'd');
			}
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', true);
		}
		else{
			robot->llenaArreglo(true);
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', false);
		}
		//Left
		if(!(robot->getParedes()&0b00000100)) {
			if(!espacio(cDir, iCol, iRow, 'i')) {
				desplazaDatos(tMapa, cDir, iCol, iRow, iPiso, 'i');
			}
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', true);
		}
		else
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', false);
	}
	// If it's a black tile
	iColor = robot->getColor();
	if(iColor == 1) {
		mapa->apantallanteLCD("NEGRO");
		// delay(200);
		// Make that tile closed
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', false);
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', false);
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', false);
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'a', false);
		tMapa[iPiso][iRow][iCol].cuadroNegro(true);
	}
}

void Mapear::llenaMapaSensor(Tile tMapa[kMapFloors][kMapSize][kMapSize], char cDir, uint8_t &iCol, uint8_t &iRow, uint8_t &iPiso) {
	//Actually, after we started using the other function all the time, this is useless... but oh well.
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
						// I look where the ramp is and map it.
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
			// Modifies max floor
			(*iPisoMax)++;
			if(mapa->sensarRampa() > kRampaLimit) {
				tMapa[iPiso][iRow][iCol].rampaArriba(true);
				tMapa[(*iPisoMax)][4][4].rampaAbajo(true);
			}
			if(mapa->sensarRampa() < -kRampaLimit) {
				tMapa[iPiso][iRow][iCol].rampaAbajo(true);
				tMapa[(*iPisoMax)][4][4].rampaArriba(true);
			}
			// Maps where it connects
			tMapa[(*iPisoMax)][4][4].piso(iPiso);
			tMapa[(*iPisoMax)][4][4].existe(true);
			tMapa[(*iPisoMax)][4][4].visitado(true);
			tMapa[iPiso][iRow][iCol].piso((*iPisoMax));
			tMapa[iPiso][iRow][iCol].visitado(true);
			// Closes the path on current floor
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', false);
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', false);
			escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', false);
			robot->pasaRampa();
			// Maps the new position
			iPiso = (*iPisoMax);
			iCol = iRow = 4;
			afterRampa(cDir, iCol, iRow);
		}
	}
	//Front
	if(mapa->caminoEnfrente()) {
		if(!espacio(cDir, iCol, iRow, 'e')) {
			desplazaDatos(tMapa, cDir, iCol, iRow, iPiso, 'e');
		}
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', true);
	}
	else
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', false);
	//Right
	if(mapa->caminoDerecha()) {
		robot->llenaArreglo(false);
		if(!espacio(cDir, iCol, iRow, 'd')) {
			desplazaDatos(tMapa, cDir, iCol, iRow, iPiso, 'd');
		}
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', true);
	}
	else{
		robot->llenaArreglo(false);
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', false);
	}
	//Left
	if(mapa->caminoIzquierda()) {
		if(!espacio(cDir, iCol, iRow, 'i')) {
			desplazaDatos(tMapa, cDir, iCol, iRow, iPiso, 'i');
		}
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', true);
	}
	else
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', false);
	iColor = mapa->color();
	if(iColor == 1) {
		// mapa->apantallanteLCD("NEGRO");
		// delay(200);
		// Close the black Tile
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'e', false);
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'd', false);
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'i', false);
		escribeMapaLoP(tMapa, cDir, iCol, iRow, iPiso, 'a', false);
		tMapa[iPiso][iRow][iCol].cuadroNegro(true);
	}
	////////////////////////////// Logic testes at home, I would introduce some letters so it would map according to them ////////////////////////////////////////
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
