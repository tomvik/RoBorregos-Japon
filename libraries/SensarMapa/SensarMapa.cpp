/* Code made by the RoBorregos team in 2017 for the RoboCup JR. Rescue Maze category.
 * Tomás Lugo, Sebastián Esquer, Ernesto Cervantez, and Alexis Virgen.
 * "El Mariachi" Achieved a third place on the international RoboCup.
 *
 * This class has all the functions to check the map, and the "hard" part of path finding.
 * It's fun. And there are better ways to do it. Highly recommend A* algorithm, it's the best for this situation.
 *
 */
#include <Arduino.h>
#include <SensarMapa.h>

/*
   cDir Dirección
   n = north
   e = east
   s = south
   w = west
   cCase Caso a verificar
   r = right = derecha
   u = up = arriba
   l = left = izquierda
   d = down = abajo
*/
///////////Dimensions///////////////////
const uint8_t kMapSize = 15;
const uint8_t kMapFloors = 4;
//////////Value of the bumper (The bumper is equal to 8 moves of the robot)//////////////////
const uint8_t kBumper = 8;

//A literal empty constructor... what a time to be alive.
SensarMapa::SensarMapa() {
}

bool SensarMapa::sensa_Pared(Tile tMapa[kMapFloors][kMapSize][kMapSize], char cDir, uint8_t iCol, uint8_t iRow, uint8_t iPiso,  char cCase) {
	switch(cCase) {
	case 'r':                       //Right
		switch(cDir) {
		case 'n':
			return !(tMapa[iPiso][iRow][iCol].derecha());
		case 'e':
			return !(tMapa[iPiso][iRow][iCol].abajo());
		case 's':
			return !(tMapa[iPiso][iRow][iCol].izquierda());
		case 'w':
			return !(tMapa[iPiso][iRow][iCol].arriba());
		}
	case 'u':                       //Up
		switch(cDir) {
		case 'n':
			return !(tMapa[iPiso][iRow][iCol].arriba());
		case 'e':
			return !tMapa[iPiso][iRow][iCol].derecha();
		case 's':
			return !(tMapa[iPiso][iRow][iCol].abajo());
		case 'w':
			return !(tMapa[iPiso][iRow][iCol].izquierda());
		}
	case 'l':                       //Left
		switch(cDir) {
		case 'n':
			return !(tMapa[iPiso][iRow][iCol].izquierda());
		case 'e':
			return !(tMapa[iPiso][iRow][iCol].arriba());
		case 's':
			return !tMapa[iPiso][iRow][iCol].derecha();
		case 'w':
			return !(tMapa[iPiso][iRow][iCol].abajo());
		}
	case 'd':                       //Down
		switch(cDir) {
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
	return false;
}

bool SensarMapa::sensaVisitado(Tile tMapa[kMapFloors][kMapSize][kMapSize], char cDir, uint8_t iCol, uint8_t iRow, uint8_t iPiso,  char cCase) {
	switch(cCase) {
	case 'r':                                       //Right
		switch(cDir) {
		case 'n':
			return !(tMapa[iPiso][iRow][iCol+1].visitado());
		case 'e':
			return !(tMapa[iPiso][iRow+1][iCol].visitado());
		case 's':
			return !(tMapa[iPiso][iRow][iCol-1].visitado());
		case 'w':
			return !(tMapa[iPiso][iRow-1][iCol].visitado());
		}
	case 'u':                                       //Front
		switch(cDir) {
		case 'n':
			return !(tMapa[iPiso][iRow-1][iCol].visitado());
		case 'e':
			return !(tMapa[iPiso][iRow][iCol+1].visitado());
		case 's':
			return !(tMapa[iPiso][iRow+1][iCol].visitado());
		case 'w':
			return !(tMapa[iPiso][iRow][iCol-1].visitado());
		}
	case 'l':                                       //Left
		switch(cDir) {
		case 'n':
			return !(tMapa[iPiso][iRow][iCol-1].visitado());
		case 'e':
			return !(tMapa[iPiso][iRow-1][iCol].visitado());
		case 's':
			return !(tMapa[iPiso][iRow][iCol+1].visitado());
		case 'w':
			return !(tMapa[iPiso][iRow+1][iCol].visitado());
		}
	case 'd':                                       //Up
		switch(cDir) {
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
	return false;
}

// Regresa TRUE si EXISTE la coordenada
bool SensarMapa::sensaExiste(Tile tMapa[kMapFloors][kMapSize][kMapSize], char cDir, uint8_t iCol, uint8_t iRow, uint8_t iPiso,  char cCase) {
	switch(cCase) {
	case 'r':                                       //Right
		switch(cDir) {
		case 'n':
			return (tMapa[iPiso][iRow][iCol+1].existe());
		case 'e':
			return (tMapa[iPiso][iRow+1][iCol].existe());
		case 's':
			return (tMapa[iPiso][iRow][iCol-1].existe());
		case 'w':
			return (tMapa[iPiso][iRow-1][iCol].existe());
		}
	case 'u':                                       //Front
		switch(cDir) {
		case 'n':
			return (tMapa[iPiso][iRow-1][iCol].existe());
		case 'e':
			return (tMapa[iPiso][iRow][iCol+1].existe());
		case 's':
			return (tMapa[iPiso][iRow+1][iCol].existe());
		case 'w':
			return (tMapa[iPiso][iRow][iCol-1].existe());
		}
	case 'l':                                       //Left
		switch(cDir) {
		case 'n':
			return (tMapa[iPiso][iRow][iCol-1].existe());
		case 'e':
			return (tMapa[iPiso][iRow-1][iCol].existe());
		case 's':
			return (tMapa[iPiso][iRow][iCol+1].existe());
		case 'w':
			return (tMapa[iPiso][iRow+1][iCol].existe());
		}
	case 'd':                                       //Up
		switch(cDir) {
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
	return false;
}
/*
 Okay, this is the interesting part. We are using a kind of naive approach, using DFS search iteratively, the bad thing about it is that you can cause a stack overflow if the matrix is too big.
 Therefore, I highly recommend to change the algorithm to A* and do it iteratively as much as possible.
 The approach was the following: I can move to the next node ONLY IF:
	1. It's possible (not out of bounds)
	2. The node exists (I've sensed before there's a path to that node)
	3. There no wall to interfere (Because a node may exist but THAT path is not the way to it)
	4. Either the distance hasn't been calculated (infinity, in this case 0) or (my current distance + the moves I'll make) is less than the distance there currently is on that node.
 And, once I move, I do the following:
	1. If there's a bumper, add the weight of the bumper to the sum (In this way, if there's another path without the bumper, it'll take that one)
	2. THIS IS THE PART THAT WAS BUGGY. Buckle up your seatbelts, that this is going to be a great trip:
		So, I was trying to optimize the algorithm, and I thought "Well, when I'm filling up the map, I should be able to already know what the best path is, there's no need to go
		recursively checking again all the possible ways like backtracking." And I was correct up to some point, that's why I have the char map, I was writing the instructions to follow
		BUT BUT, the problem was when there were two paths of the same lenght and let's say the two paths were going from i to f:
			1. Left->Forward->Left->Forward->Left->Forward
			2. Right->Forward->Right->Forward->Right->Forward
			|	|	|
			| f | i |
			|	|	|
		This gives out two actual outcomes:
			| l | l |	|   |   |
			| r | i |	| l | i |
			|   |   |	| r | r |
		And this is mixed up, by following these instructions you would not get to the actual tile. I patched, didn't fixed, this on the getInstruction function with a goto.
		It was an improvement on most cases what I did, but of course it wasn't the best solution. I strongly believe A* would be best.
*/
void SensarMapa::llenaMapa(uint8_t iMapa[kMapSize][kMapSize], char cMapa[kMapSize][kMapSize], Tile tMapa[kMapFloors][kMapSize][kMapSize], char cDir, uint8_t iCol, uint8_t iRow, uint8_t iPiso) {
	//Right, Up, Left, and Down relative to the robot
	//The time limit is just in case it gets stucked, never happened.
	if(tInicio + 5000 >= millis()){
		switch(cDir) {
		case 'n':
			//Right
			if(iCol < kMapSize-1 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'r') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'r') && (iMapa[iRow][iCol+1] == 0 || iMapa[iRow][iCol+1] > iMapa[iRow][iCol]+2)) {
				iMapa[iRow][iCol+1] = ( tMapa[iPiso][iRow][iCol+1].bumper() ) ? iMapa[iRow][iCol]+2+kBumper : iMapa[iRow][iCol]+2;
				cMapa[iRow][iCol+1] = 'r';
				llenaMapa(iMapa, cMapa, tMapa, 'e', iCol+1, iRow, iPiso);
			}
			//UP
			if(iRow > 0 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'u') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'u') && (iMapa[iRow-1][iCol] == 0 || iMapa[iRow-1][iCol] > iMapa[iRow][iCol]+1)) {
				iMapa[iRow-1][iCol] = ( tMapa[iPiso][iRow-1][iCol].bumper() ) ? iMapa[iRow][iCol]+1+kBumper : iMapa[iRow][iCol]+1;
				cMapa[iRow-1][iCol] = 'u';
				llenaMapa(iMapa, cMapa, tMapa, 'n', iCol, iRow-1, iPiso);
			}
			//LEFT
			if(iCol > 0 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'l') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'l') && (iMapa[iRow][iCol-1] == 0 || iMapa[iRow][iCol-1] > iMapa[iRow][iCol]+2)) {
				iMapa[iRow][iCol-1] = ( tMapa[iPiso][iRow][iCol-1].bumper() ) ? iMapa[iRow][iCol]+2+kBumper : iMapa[iRow][iCol]+2;
				cMapa[iRow][iCol-1] = 'l';
				llenaMapa(iMapa, cMapa, tMapa, 'w', iCol-1, iRow, iPiso);
			}
			//DOWN
			if(iRow < kMapSize-1 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'd') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'd') && (iMapa[iRow+1][iCol] == 0 || iMapa[iRow+1][iCol] > iMapa[iRow][iCol]+3)) {
				iMapa[iRow+1][iCol] = ( tMapa[iPiso][iRow+1][iCol].bumper() ) ? iMapa[iRow][iCol]+3+kBumper : iMapa[iRow][iCol]+3;
				cMapa[iRow+1][iCol] = 'd';
				llenaMapa(iMapa, cMapa, tMapa, 's', iCol, iRow+1, iPiso);
			}
			break;
		case 'e':
			//Right
			if(iRow < kMapSize-1 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'r') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'r') && (iMapa[iRow+1][iCol] == 0 || iMapa[iRow+1][iCol] > iMapa[iRow][iCol]+2)) {
				iMapa[iRow+1][iCol] = ( tMapa[iPiso][iRow+1][iCol].bumper() ) ? iMapa[iRow][iCol]+2+kBumper : iMapa[iRow][iCol]+2;
				cMapa[iRow+1][iCol] = 'r';
				llenaMapa(iMapa, cMapa, tMapa, 's', iCol, iRow+1, iPiso);
			}
			//UP
			if(iCol < kMapSize-1 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'u') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'u') && (iMapa[iRow][iCol+1] == 0 || iMapa[iRow][iCol+1] > iMapa[iRow][iCol]+1)) {
				iMapa[iRow][iCol+1] = ( tMapa[iPiso][iRow][iCol+1].bumper() ) ? iMapa[iRow][iCol]+1+kBumper : iMapa[iRow][iCol]+1;
				cMapa[iRow][iCol+1] = 'u';
				llenaMapa(iMapa, cMapa, tMapa, 'e', iCol+1, iRow, iPiso);
			}
			//LEFT
			if(iRow > 0 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'l') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'l') && (iMapa[iRow-1][iCol] == 0 || iMapa[iRow-1][iCol] > iMapa[iRow][iCol]+2)) {
				iMapa[iRow-1][iCol] = ( tMapa[iPiso][iRow-1][iCol].bumper() ) ? iMapa[iRow][iCol]+2+kBumper : iMapa[iRow][iCol]+2;
				cMapa[iRow-1][iCol] = 'l';
				llenaMapa(iMapa, cMapa, tMapa, 'n', iCol, iRow-1, iPiso);
			}
			//DOWN
			if(iCol > 0 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'd') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'd') && (iMapa[iRow][iCol-1] == 0 || iMapa[iRow][iCol-1] > iMapa[iRow][iCol]+3)) {
				iMapa[iRow][iCol-1] = ( tMapa[iPiso][iRow][iCol-1].bumper() ) ? iMapa[iRow][iCol]+3+kBumper : iMapa[iRow][iCol]+3;
				cMapa[iRow][iCol-1] = 'd';
				llenaMapa(iMapa, cMapa, tMapa, 'w', iCol-1, iRow, iPiso);
			}
			break;
		case 's':
			//Right
			if(iCol > 0 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'r') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'r') && (iMapa[iRow][iCol-1] == 0 || iMapa[iRow][iCol-1] > iMapa[iRow][iCol]+2)) {
				iMapa[iRow][iCol-1] = ( tMapa[iPiso][iRow][iCol-1].bumper() ) ? iMapa[iRow][iCol]+2+kBumper : iMapa[iRow][iCol]+2;
				cMapa[iRow][iCol-1] = 'r';
				llenaMapa(iMapa, cMapa, tMapa, 'w', iCol-1, iRow, iPiso);
			}
			//UP
			if(iRow < kMapSize-1 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'u') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'u') && (iMapa[iRow+1][iCol] == 0 || iMapa[iRow+1][iCol] > iMapa[iRow][iCol]+1)) {
				iMapa[iRow+1][iCol] = ( tMapa[iPiso][iRow+1][iCol].bumper() ) ? iMapa[iRow][iCol]+1+kBumper : iMapa[iRow][iCol]+1;
				cMapa[iRow+1][iCol] = 'u';
				llenaMapa(iMapa, cMapa, tMapa, 's', iCol, iRow+1, iPiso);
			}
			//LEFT
			if(iCol < kMapSize-1 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'l') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'l') && (iMapa[iRow][iCol+1] == 0 || iMapa[iRow][iCol+1] > iMapa[iRow][iCol]+2)) {
				iMapa[iRow][iCol+1] = ( tMapa[iPiso][iRow][iCol+1].bumper() ) ? iMapa[iRow][iCol]+2+kBumper : iMapa[iRow][iCol]+2;
				cMapa[iRow][iCol+1] = 'l';
				llenaMapa(iMapa, cMapa, tMapa, 'e', iCol+1, iRow, iPiso);
			}
			//DOWN
			if(iRow > 0 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'd') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'd') && (iMapa[iRow-1][iCol] == 0 || iMapa[iRow-1][iCol] > iMapa[iRow][iCol]+3)) {
				iMapa[iRow-1][iCol] = ( tMapa[iPiso][iRow-1][iCol].bumper() ) ? iMapa[iRow][iCol]+3+kBumper : iMapa[iRow][iCol]+3;
				cMapa[iRow-1][iCol] = 'd';
				llenaMapa(iMapa, cMapa, tMapa, 'n', iCol, iRow-1, iPiso);
			}
			break;
		case 'w':
			//Right
			if(iRow > 0 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'r') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'r') && (iMapa[iRow-1][iCol] == 0 || iMapa[iRow-1][iCol] > iMapa[iRow][iCol]+2)) {
				iMapa[iRow-1][iCol] = ( tMapa[iPiso][iRow-1][iCol].bumper() ) ? iMapa[iRow][iCol]+2+kBumper : iMapa[iRow][iCol]+2;
				cMapa[iRow-1][iCol] = 'r';
				llenaMapa(iMapa, cMapa, tMapa, 'n', iCol, iRow-1, iPiso);
			}
			//UP
			if(iCol > 0 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'u') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'u') && (iMapa[iRow][iCol-1] == 0 || iMapa[iRow][iCol-1] > iMapa[iRow][iCol]+1)) {
				iMapa[iRow][iCol-1] = ( tMapa[iPiso][iRow][iCol-1].bumper() ) ? iMapa[iRow][iCol]+1+kBumper : iMapa[iRow][iCol]+1;
				cMapa[iRow][iCol-1] = 'u';
				llenaMapa(iMapa, cMapa, tMapa, 'w', iCol-1, iRow, iPiso);
			}
			//LEFT
			if(iRow < kMapSize-1 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'l') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'l') && (iMapa[iRow+1][iCol] == 0 || iMapa[iRow+1][iCol] > iMapa[iRow][iCol]+2)) {
				iMapa[iRow+1][iCol] = ( tMapa[iPiso][iRow+1][iCol].bumper() ) ? iMapa[iRow][iCol]+2+kBumper : iMapa[iRow][iCol]+2;
				cMapa[iRow+1][iCol] = 'l';
				llenaMapa(iMapa, cMapa, tMapa, 's', iCol, iRow+1, iPiso);
			}
			//DOWN
			if(iCol < kMapSize-1 && sensaExiste(tMapa, cDir, iCol, iRow, iPiso, 'd') && sensa_Pared(tMapa, cDir, iCol, iRow, iPiso, 'd') && (iMapa[iRow][iCol+1] == 0 || iMapa[iRow][iCol+1] > iMapa[iRow][iCol]+3)) {
				iMapa[iRow][iCol+1] = ( tMapa[iPiso][iRow][iCol+1].bumper() ) ? iMapa[iRow][iCol]+3+kBumper : iMapa[iRow][iCol]+3;
				cMapa[iRow][iCol+1] = 'd';
				llenaMapa(iMapa, cMapa, tMapa, 'e', iCol+1, iRow, iPiso);
			}
			break;
		}
	}
}

//Compares the distances and saves only the minimum one.
//This function return the position of the nearest not visited tile and returns true if there's an unvisited tile still (or possible ramp)
//Also, by changing the case, it could look up for a ramp or the initial tile.
//Never managed the case where there's a loop between the floors. Happened on the last round of the international.
bool SensarMapa::comparaMapa(uint8_t iMapa[kMapSize][kMapSize], Tile tMapa[kMapFloors][kMapSize][kMapSize], char cD, uint8_t iCol, uint8_t iRow, uint8_t &iNCol, uint8_t &iNRow, uint8_t iPiso) {
	//Just a big number
	uint8_t iC = 255;
	bool bT = false;
	/*
	In here the logic is as follows:
	The case will ALWAYS be 'n' except when you have finished the whole map. So, when in a floor there are no more unvisited tiles,
	You'll look instead for a ramp that connects to the previous floor (Because on that floor is the only place where a non visited tile can be)
	IF you didn't find any ramp that connects to a previous floor, it means you're on the initial floor and that you have finished to go through the whole map
	So, it returns false and in that way, we exit the main loop to now go to the beginning. That's the boolean bT for.
	*/
	if(cD == 'n') {
		for (uint8_t i = 0; i < kMapSize; ++i)
			for (uint8_t j = 0; j < kMapSize; ++j)
				if(iMapa[i][j] != 0 && iC >= iMapa[i][j] && !tMapa[iPiso][i][j].visitado()) {
					iC = iMapa[i][j];
					iNCol = j;
					iNRow = i;
					bT = true;
				}
	}
	else if(cD == 'i') {
		for (uint8_t i = 0; i < kMapSize; ++i)
			for (uint8_t j = 0; j < kMapSize; ++j)
				if(iMapa[i][j] != 0 && iC >= iMapa[i][j] && tMapa[iPiso][i][j].inicio()) {
					iC = iMapa[i][j];
					iNCol = j;
					iNRow = i;
					bT = true;
				}
	}
	if(bT == false) {
		for (uint8_t i = 0; i < kMapSize; ++i)
			for (uint8_t j = 0; j < kMapSize; ++j)
				if(iMapa[i][j] != 0 && ( tMapa[iPiso][i][j].rampaAbajo() || tMapa[iPiso][i][j].rampaArriba() ) && tMapa[iPiso][i][j].piso() < iPiso) {
					iC = iMapa[i][j];
					iNCol = j;
					iNRow = i;
					bT = true;
				}
	}
	return bT;
}

//In here we iterate backwards, from the goal node to the one in which I am following the instructions on cMapa.
String SensarMapa::getInstrucciones(uint8_t iMapa[kMapSize][kMapSize], char cMapa[kMapSize][kMapSize], Tile tMapa[kMapFloors][kMapSize][kMapSize], uint8_t iNCol, uint8_t iNRow, uint8_t iPiso) {
	uint8_t iNRSave = iNRow, iNCSave = iNCol;
	bool b1 = true, b2 = true, b3 = true, b4 = true;
	///////////PATCH :(
	fix:
	iNRow = iNRSave, iNCol = iNCSave;
	String sIns = "";
	sIns += cMapa[iNRow][iNCol];
	uint8_t iR, iU, iL, iD;
	char cDir;
	//Get's the distance from each one (Beginning at the last node, the one we want to visit)
	iR = iU = iL = iD = 255;
	if(sensa_Pared(tMapa, 'n', iNCol, iNRow, iPiso, 'r') && sensaExiste(tMapa, 'n', iNCol, iNRow, iPiso, 'r') && iMapa[iNRow][iNCol+1] != 0)
		iR = iMapa[iNRow][iNCol+1];
	if(sensa_Pared(tMapa, 'n', iNCol, iNRow, iPiso, 'u') && sensaExiste(tMapa, 'n', iNCol, iNRow, iPiso, 'u') && iMapa[iNRow-1][iNCol] != 0)
		iU = iMapa[iNRow-1][iNCol];
	if(sensa_Pared(tMapa, 'n', iNCol, iNRow, iPiso, 'l') && sensaExiste(tMapa, 'n', iNCol, iNRow, iPiso, 'l') && iMapa[iNRow][iNCol-1] != 0)
		iL = iMapa[iNRow][iNCol-1];
	if(sensa_Pared(tMapa, 'n', iNCol, iNRow, iPiso, 'd') && sensaExiste(tMapa, 'n', iNCol, iNRow, iPiso, 'd') && iMapa[iNRow+1][iNCol] != 0)
		iD = iMapa[iNRow+1][iNCol];
	iR = b1 ? iR : 255;
	iU = b2 ? iU : 255;
	iL = b3 ? iL : 255;
	iD = b4 ? iD : 255;
	//Comparing
	if(b1 && iR <= iU && iR <= iL && iR <= iD) {
		b1 = false;
		switch(cMapa[iNRow][iNCol]) {
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
	else if(b2 && iU <= iR && iU <= iL && iU <= iD) {
		b2 = false;
		switch(cMapa[iNRow][iNCol]) {
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
	else if(b3 && iL <= iU && iL <= iR && iL <= iD) {
		b3 = false;
		switch(cMapa[iNRow][iNCol]) {
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
	else if (b4){
		b4 = false;
		switch(cMapa[iNRow][iNCol]) {
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
	else{
		//TODO that nothing worked, never implemented what would happen if none would work. Can't think of a case where that would happen.
		return "K";
	}
	//Loop until it is where the robot is
	while(cMapa[iNRow][iNCol] != 'i') {
		//Patch, if I'm out of bounds, try another way.
		if(cMapa[iNRow][iNCol] == 'n' || iNRow > kMapSize || iNCol > kMapSize){
			goto fix;
		}
		sIns += cMapa[iNRow][iNCol];
		switch(cDir) {
		case 'n':
			switch(cMapa[iNRow][iNCol]) {
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
			switch(cMapa[iNRow][iNCol]) {
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
			switch(cMapa[iNRow][iNCol]) {
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
			switch(cMapa[iNRow][iNCol]) {
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

//Sets the initial time to the recursive function
void SensarMapa::tiempoI(unsigned long ul){
	tInicio = ul;
}
