//#include <iostream>
//#include <fstream>
#include <StandardCplusplus.h>
#include <string>
#include <vector>

using namespace std;

//Mejorar el de recursividad (no de n a p, de p a todas n y comparar)
//Hay mucha funciones que se parecen un chorro, pueden mejorar
//Tal vez hacer de esto una libreria?
//Agregar rampa, checkpoints
//Animo

//n = no visitado
//v = sí visitado
//i = inicial?
//f = final?
///////////////////////////////////////////// Carga mapa y para mostrarlo ////////////////////////////////////////////

//Esta función recibe el mapa virtual y sus dimensiones para así llenarlo respecto al archivo de texto donde se encuentra
/*void carga_Mapa(vector < vector <char> > &mapaVirtual, int iRow, int iCol){          //No será necesaria en real, almenos que tengamos una sd para cargar el mapa
  mapaVirtual.resize(iRow);
  for(int i=0; i<iRow; i++)
    mapaVirtual[i].resize(iCol);

  ifstream iFile;
  iFile.open("mapa.txt");
  for(int i=0; i<iRow; i++)
    for(int j=0; j<iCol; j++)
      iFile >> mapaVirtual[i][j];
  iFile.close();  
}*/

//Esta función muestra el mapa de caracteres
/*void muestra_Mapa_C(vector < vector <char> > mapa){           //Solo para ver el mapeo
  for(int i=0; i<mapa.size(); i++){
    for(int j=0; j<mapa[i].size(); j++)
      cout << mapa[i][j] << " ";
    cout << endl;
  }
  cout << endl;
}

void muestra_Mapa_I(vector < vector <int> > mapa){            //Solo para ver el mapeo
  for(int i=0; i<mapa.size(); i++){
    for(int j=0; j<mapa[i].size(); j++)
      cout << mapa[i][j] << " ";
    cout << endl;
  }
  cout << endl;
}*/

void inicia_Mapa(vector < vector <char> > &mapaRobot, int iRow, int iCol, int iR, int iC){  //Puede que después no sirva
  mapaRobot.resize(iRow);
  for(int i=0; i<iRow; i++)
    mapaRobot[i].resize(iCol, 'x');

  mapaRobot[iR][iC] = 'i';                  //i = inicio
}

////////////////////////////////////////////////Sensa las paredes ////////////////////////////////////////////////////
bool sDerecha(vector < vector <char> > map, char cDir, int iPos[2]){          //Tendra que ser modificada por sensores US
  switch(cDir)
  {
    case 'n':
      return map[iPos[0]][iPos[1]+1] == '.';
    case 'e':
      return map[iPos[0]+1][iPos[1]] == '.';
    case 's':
      return map[iPos[0]][iPos[1]-1] == '.';
    case 'w':
      return map[iPos[0]-1][iPos[1]] == '.';
  }
}

bool sEnfrente(vector < vector <char> > map, char cDir, int iPos[2]){         //Tendra que ser modificada por sensores US
  switch(cDir)
  {
    case 'n':
      return map[iPos[0]-1][iPos[1]] == '.';
    case 'e':
      return map[iPos[0]][iPos[1]+1] == '.';
    case 's':
      return map[iPos[0]+1][iPos[1]] == '.';
    case 'w':
      return map[iPos[0]][iPos[1]-1] == '.';
  }
}

bool sIzquierda(vector < vector <char> > map, char cDir, int iPos[2]){          //Tendra que ser modificada por sensores US
  switch(cDir)
  {
    case 'n':
      return map[iPos[0]][iPos[1]-1] == '.';
    case 'e':
      return map[iPos[0]-1][iPos[1]] == '.';
    case 's':
      return map[iPos[0]][iPos[1]+1] == '.';
    case 'w':
      return map[iPos[0]+1][iPos[1]] == '.';
  }
}

bool sAtras(vector < vector <char> > map, char cDir, int iPos[2]){            //Tendra que ser modificada por sensores US
  switch(cDir)
  {
    case 'n':
      return map[iPos[0]+1][iPos[1]] == '.';
    case 'e':
      return map[iPos[0]][iPos[1]-1] == '.';
    case 's':
      return map[iPos[0]-1][iPos[1]] == '.';
    case 'w':
      return map[iPos[0]][iPos[1]+1] == '.';
  }
}

/////////////////////////////////////////////// Modifica mapa actual //////////////////////////////////////////////////

void agregaRow(vector < vector <char> > &mapaRobot, int &iRow){             //Agrega dos rows (pared y tile)
  vector <char> temp (mapaRobot[0].size(), 'x');
  for(int j=0; j<2; j++){
    mapaRobot.push_back(temp);
  }
  iRow += 2;
}

void agregaColumn(vector < vector <char> > &mapaRobot, int &iCol){            //Agrega dos columnas (pared y tile)
  for(int i = 0; i < mapaRobot.size(); i++){
    for(int j=0; j<2; j++)
      mapaRobot[i].push_back('x');
  }
  iCol += 2;
}

void realDesplaza(vector < vector <char> > &mapaRobot, int iCase, int iPos[2]){     //Si está bien, podría quedarse así, aunque la quiero optimizar
  vector <char> vTemp;
  vTemp.resize(mapaRobot[0].size(), 'x');
  switch(iCase){
    case 0:
      for(int i=0; i < mapaRobot.size(); i++)
        mapaRobot[i].erase(mapaRobot[i].begin(), mapaRobot[i].begin()+2);
      for(int i=0; i < mapaRobot.size(); i++)
        mapaRobot[i].resize(mapaRobot[i].size()+2, 'x');
      iPos[1] -= 2;
      break;
    case 1:
      mapaRobot.erase(mapaRobot.begin(), mapaRobot.begin()+2);
      mapaRobot.resize(mapaRobot.size()+2, vTemp);
      iPos[0] -= 2;
      break;
    case 2:
      for(int i=0; i < mapaRobot.size(); i++){
        mapaRobot[i].pop_back();
        mapaRobot[i].pop_back();
      }
      for(int i=0; i < mapaRobot.size(); i++){
        mapaRobot[i].insert(mapaRobot[i].begin(), 'x');
        mapaRobot[i].insert(mapaRobot[i].begin(), 'x');
      }
      iPos[1] += 2;
      break;
    case 3:
      mapaRobot.pop_back();
      mapaRobot.pop_back();
      mapaRobot.insert(mapaRobot.begin(), vTemp);
      mapaRobot.insert(mapaRobot.begin(), vTemp);
      iPos[0] += 2;
      break;
  }
}

void desplaza(vector < vector <char> > &mapaRobot, char cDir, char cChecar, int iPos[2]){//Si está bien, podría quedarse así, aunque la quiero optimizar, igual que la ant
  switch(cChecar)     //d derecha i izquierda a atras e enfrente
  {
    case 'd':     //checando derecha
      switch(cDir)
      {
        case 'n': //viendo al norte
          realDesplaza(mapaRobot, 0, iPos);
          break;
        case 'e': //viendo al este
          realDesplaza(mapaRobot, 1, iPos);
          break;
        case 's': //viendo al sur
          realDesplaza(mapaRobot, 2, iPos);
          break;
        case 'w': //viendo al oeste
          realDesplaza(mapaRobot, 3, iPos);
          break;
      }
      break;
    case 'e':     //checando enfrente
      switch(cDir)
      {
        case 'n': //viendo al norte
          realDesplaza(mapaRobot, 3, iPos);
          break;
        case 'e': //viendo al este
          realDesplaza(mapaRobot, 0, iPos);
          break;
        case 's': //viendo al sur
          realDesplaza(mapaRobot, 1, iPos);
          break;
        case 'w': //viendo al oeste
          realDesplaza(mapaRobot, 2, iPos);
          break;
      }
      break;
    case 'i':     //checando izquierda
      switch(cDir)
      {
        case 'n': //viendo al norte
          realDesplaza(mapaRobot, 2, iPos);
          break;
        case 'e': //viendo al este
          realDesplaza(mapaRobot, 3, iPos);
          break;
        case 's': //viendo al sur
          realDesplaza(mapaRobot, 0, iPos);
          break;
        case 'w': //viendo al oeste
          realDesplaza(mapaRobot, 1, iPos);
          break;
      }
      break;
    case 'a':     //checando atras
      switch(cDir)
      {
        case 'n': //viendo al norte
          realDesplaza(mapaRobot, 1, iPos);
          break;
        case 'e': //viendo al este
          realDesplaza(mapaRobot, 2, iPos);
          break;
        case 's': //viendo al sur
          realDesplaza(mapaRobot, 3, iPos);
          break;
        case 'w': //viendo al oeste
          realDesplaza(mapaRobot, 0, iPos);
          break;
      }
      break;
  }
}

void creaEspacio(vector < vector <char> > &mapaRobot, int iPos[2], char cDir, char cChecar, int &iRow, int &iCol){//Si está bien, podría quedarse así, aunque la quiero optimizar, igual que la anterior
  switch(cChecar)     //d derecha i izquierda a atras e enfrente
  {
    case 'd':     //checando derecha
      switch(cDir)
      {
        case 'n': //viendo al norte
          agregaColumn(mapaRobot, iCol);
          break;
        case 'e': //viendo al este
          agregaRow(mapaRobot, iRow);
          break;
        case 's': //viendo al sur
          agregaColumn(mapaRobot, iCol);
          desplaza(mapaRobot, cDir, 'd', iPos);
          break;
        case 'w': //viendo al oeste
          agregaRow(mapaRobot, iRow);
          desplaza(mapaRobot, cDir, 'd', iPos);
          break;
      }
      break;
    case 'e':     //checando enfrente
      switch(cDir)
      {
        case 'n': //viendo al norte
          agregaRow(mapaRobot, iRow);
          desplaza(mapaRobot, cDir, 'e', iPos);
          break;
        case 'e': //viendo al este
          agregaColumn(mapaRobot, iCol);
          break;
        case 's': //viendo al sur
          agregaRow(mapaRobot, iRow);
          break;
        case 'w': //viendo al oeste
          agregaColumn(mapaRobot, iCol);
          desplaza(mapaRobot, cDir, 'e', iPos);
          break;
      }
      break;
    case 'i':     //checando izquierda
      switch(cDir)
      {
        case 'n': //viendo al norte
          agregaColumn(mapaRobot, iCol);
          desplaza(mapaRobot, cDir, 'i', iPos);
          break;
        case 'e': //viendo al este
          agregaRow(mapaRobot, iRow);
          desplaza(mapaRobot, cDir, 'i', iPos);
          break;
        case 's': //viendo al sur
          agregaColumn(mapaRobot, iCol);
          break;
        case 'w': //viendo al oeste
          agregaRow(mapaRobot, iRow);
          break;
      }
      break;
    case 'a':     //checando atras
      switch(cDir)
      {
        case 'n': //viendo al norte
          agregaRow(mapaRobot, iRow);
          break;
        case 'e': //viendo al este
          agregaColumn(mapaRobot, iCol);
          desplaza(mapaRobot, cDir, 'a', iPos);
          break;
        case 's': //viendo al sur
          agregaRow(mapaRobot, iRow);
          desplaza(mapaRobot, cDir, 'a', iPos);
          break;
        case 'w': //viendo al oeste
          agregaColumn(mapaRobot, iCol);
          break;
      }
      break;
  }
}

void llenaNoVisitado(vector < vector <char> > &mapaRobot, int iPos[2], char cDir, char cChecar){//Si está bien, podría quedarse así, aunque la quiero optimizar, igual que la anterior
  switch(cChecar)     //d derecha i izquierda a atras e enfrente
  {
    case 'd':     //checando derecha
      switch(cDir)
      {
        case 'n': //viendo al norte
          mapaRobot[iPos[0]][iPos[1]+2] = 'n';
          break;
        case 'e': //viendo al este
          mapaRobot[iPos[0]+2][iPos[1]] = 'n';
          break;
        case 's': //viendo al sur
          mapaRobot[iPos[0]][iPos[1]-2] = 'n';
          break;
        case 'w': //viendo al oeste
          mapaRobot[iPos[0]-2][iPos[1]] = 'n';
          break;
      }
      break;
    case 'e':     //checando enfrente
      switch(cDir)
      {
        case 'n': //viendo al norte
          mapaRobot[iPos[0]-2][iPos[1]] = 'n';
          break;
        case 'e': //viendo al este
          mapaRobot[iPos[0]][iPos[1]+2] = 'n';
          break;
        case 's': //viendo al sur
          mapaRobot[iPos[0]+2][iPos[1]] = 'n';
          break;
        case 'w': //viendo al oeste
          mapaRobot[iPos[0]][iPos[1]-2] = 'n';
          break;
      }
      break;
    case 'i':     //checando izquierda
      switch(cDir)
      {
        case 'n': //viendo al norte
          mapaRobot[iPos[0]][iPos[1]-2] = 'n';
          break;
        case 'e': //viendo al este
          mapaRobot[iPos[0]-2][iPos[1]] = 'n';
          break;
        case 's': //viendo al sur
          mapaRobot[iPos[0]][iPos[1]+2] = 'n';
          break;
        case 'w': //viendo al oeste
          mapaRobot[iPos[0]+2][iPos[1]] = 'n';
          break;
      }
      break;
    case 'a':     //checando atras
      switch(cDir)
      {
        case 'n': //viendo al norte
          mapaRobot[iPos[0]+2][iPos[1]] = 'n';
          break;
        case 'e': //viendo al este
          mapaRobot[iPos[0]][iPos[1]-2] = 'n';
          break;
        case 's': //viendo al sur
          mapaRobot[iPos[0]-2][iPos[1]] = 'n';
          break;
        case 'w': //viendo al oeste
          mapaRobot[iPos[0]][iPos[1]+2] = 'n';
          break;
      }
      break;
  }
}

void llenaPared(vector < vector <char> > &mapaRobot, int iPos[2], char cDir, char cChecar, char cPared){//Esta podría ser una misma funcion que llena no visitado
  switch(cChecar)     //d derecha i izquierda a atras e enfrente
  {
    case 'd':     //checando derecha
      switch(cDir)
      {
        case 'n': //viendo al norte
          mapaRobot[iPos[0]][iPos[1]+1] = cPared;
          break;
        case 'e': //viendo al este
          mapaRobot[iPos[0]+1][iPos[1]] = cPared;
          break;
        case 's': //viendo al sur
          mapaRobot[iPos[0]][iPos[1]-1] = cPared;
          break;
        case 'w': //viendo al oeste
          mapaRobot[iPos[0]-1][iPos[1]] = cPared;
          break;
      }
      break;
    case 'e':     //checando enfrente
      switch(cDir)
      {
        case 'n': //viendo al norte
          mapaRobot[iPos[0]-1][iPos[1]] = cPared;
          break;
        case 'e': //viendo al este
          mapaRobot[iPos[0]][iPos[1]+1] = cPared;
          break;
        case 's': //viendo al sur
          mapaRobot[iPos[0]+1][iPos[1]] = cPared;
          break;
        case 'w': //viendo al oeste
          mapaRobot[iPos[0]][iPos[1]-1] = cPared;
          break;
      }
      break;
    case 'i':     //checando izquierda
      switch(cDir)
      {
        case 'n': //viendo al norte
          mapaRobot[iPos[0]][iPos[1]-1] = cPared;
          break;
        case 'e': //viendo al este
          mapaRobot[iPos[0]-1][iPos[1]] = cPared;
          break;
        case 's': //viendo al sur
          mapaRobot[iPos[0]][iPos[1]+1] = cPared;
          break;
        case 'w': //viendo al oeste
          mapaRobot[iPos[0]+1][iPos[1]] = cPared;
          break;
      }
      break;
    case 'a':     //checando atras
      switch(cDir)
      {
        case 'n': //viendo al norte
          mapaRobot[iPos[0]+1][iPos[1]] = cPared;
          break;
        case 'e': //viendo al este
          mapaRobot[iPos[0]][iPos[1]-1] = cPared;
          break;
        case 's': //viendo al sur
          mapaRobot[iPos[0]-1][iPos[1]] = cPared;
          break;
        case 'w': //viendo al oeste
          mapaRobot[iPos[0]][iPos[1]+1] = cPared;
          break;
      }
      break;
  }
}

//////////////////////////////////////////Verifica el estado del mapa actual////////////////////////////////////////////////

bool espacioParaPoner(vector < vector <char> > mapaRobot, int iPos[2], char cDir, char cChecar){//Si está bien, podría quedarse así, aunque la quiero optimizar
  switch(cChecar)     //d derecha i izquierda a atras e enfrente
  {
    case 'd':     //checando derecha
      switch(cDir)
      {
        case 'n': //viendo al norte
          return iPos[1] < mapaRobot[0].size()-2;
        case 'e': //viendo al este
          return iPos[0] < mapaRobot.size()-2;
        case 's': //viendo al sur
          return iPos[1] > 1;
        case 'w': //viendo al oeste
          return iPos[0] > 1;
      }
    case 'e':     //checando enfrente
      switch(cDir)
      {
        case 'n': //viendo al norte
          return iPos[0] > 1;
        case 'e': //viendo al este
          return iPos[1] < mapaRobot[0].size()-2;
        case 's': //viendo al sur
          return iPos[0] < mapaRobot.size()-2;
        case 'w': //viendo al oeste
          return iPos[1] > 1;
      }
    case 'i':     //checando izquierda
      switch(cDir)
      {
        case 'n': //viendo al norte
          return iPos[1] > 1;
        case 'e': //viendo al este
          return iPos[0] > 1;
        case 's': //viendo al sur
          return iPos[1] < mapaRobot[0].size()-2;
        case 'w': //viendo al oeste
          return iPos[0] < mapaRobot.size()-2;
      }
    case 'a':     //checando atras
      switch(cDir)
      {
        case 'n': //viendo al norte
          return iPos[0] < mapaRobot.size()-2;
        case 'e': //viendo al este
          return iPos[1] > 1;
        case 's': //viendo al sur
          return iPos[0] > 1;
        case 'w': //viendo al oeste
          return iPos[1] < mapaRobot[0].size()-2;
      }
  }
}

bool checarEspacioDesplazar(vector < vector <char> > mapaRobot, int iCase){//Si está bien, podría quedarse así, aunque podría optimizarse
  int iC=1;
  switch(iCase){
    case 0:
      while(iC < mapaRobot.size()-1){
        if(mapaRobot[iC][1] != 'x')
          return false;
        iC+=2;
      }
      return true;
    case 1:
      while(iC < mapaRobot.size()-1){
        if(mapaRobot[1][iC] != 'x')
          return false;
        iC+=2;
      }
      return true;
    case 2:
      while(iC < mapaRobot.size()-1){
        if(mapaRobot[iC][mapaRobot[0].size()-2] != 'x')
          return false;
        iC+=2;
      }
      return true;
    case 3:
      while(iC < mapaRobot.size()-1){
        if(mapaRobot[mapaRobot.size()-2][iC] != 'x')
          return false;
        iC+=2;
      }
      return true;
  }
}

bool espacioParaDesplazar(vector < vector <char> > mapaRobot, char cDir, char cChecar){//Si está bien, podría quedarse así, aunque la quiero optimizar, igual que la ant
  switch(cChecar)     //d derecha i izquierda a atras e enfrente
  {
    case 'd':     //checando derecha
      switch(cDir)
      {
        case 'n': //viendo al norte
          return checarEspacioDesplazar(mapaRobot, 0);
        case 'e': //viendo al este
          return checarEspacioDesplazar(mapaRobot, 1);
        case 's': //viendo al sur
          return checarEspacioDesplazar(mapaRobot, 2);
        case 'w': //viendo al oeste
          return checarEspacioDesplazar(mapaRobot, 3);
      }
    case 'e':     //checando enfrente
      switch(cDir)
      {
        case 'n': //viendo al norte
          return checarEspacioDesplazar(mapaRobot, 3);
        case 'e': //viendo al este
          return checarEspacioDesplazar(mapaRobot, 0);
        case 's': //viendo al sur
          return checarEspacioDesplazar(mapaRobot, 1);
        case 'w': //viendo al oeste
          return checarEspacioDesplazar(mapaRobot, 2);
      }
    case 'i':     //checando izquierda
      switch(cDir)
      {
        case 'n': //viendo al norte
          return checarEspacioDesplazar(mapaRobot, 2);
        case 'e': //viendo al este
          return checarEspacioDesplazar(mapaRobot, 3);
        case 's': //viendo al sur
          return checarEspacioDesplazar(mapaRobot, 0);
        case 'w': //viendo al oeste
          return checarEspacioDesplazar(mapaRobot, 1);
      }
    case 'a':     //checando atras
      switch(cDir)
      {
        case 'n': //viendo al norte
          return checarEspacioDesplazar(mapaRobot, 1);
        case 'e': //viendo al este
          return checarEspacioDesplazar(mapaRobot, 2);
        case 's': //viendo al sur
          return checarEspacioDesplazar(mapaRobot, 3);
        case 'w': //viendo al oeste
          return checarEspacioDesplazar(mapaRobot, 0);
      }
  }
}

bool yaLlenado(vector < vector <char> > mapaRobot, int iPos[2], char cDir, char cChecar){//Si está bien, podría quedarse así, aunque la quiero optimizar, igual que la ant
  switch(cChecar)     //d derecha i izquierda a atras e enfrente
  {
    case 'd':     //checando derecha
      switch(cDir)
      {
        case 'n': //viendo al norte
          return mapaRobot[iPos[0]][iPos[1]+2] != 'x';
        case 'e': //viendo al este
          return mapaRobot[iPos[0]+2][iPos[1]] != 'x';
        case 's': //viendo al sur
          return mapaRobot[iPos[0]][iPos[1]-2] != 'x';
        case 'w': //viendo al oeste
          return mapaRobot[iPos[0]-2][iPos[1]] != 'x';
      }
      break;
    case 'e':     //checando enfrente
      switch(cDir)
      {
        case 'n': //viendo al norte
          return mapaRobot[iPos[0]-2][iPos[1]] != 'x';
        case 'e': //viendo al este
          return mapaRobot[iPos[0]][iPos[1]+2] != 'x';
        case 's': //viendo al sur
          return mapaRobot[iPos[0]+2][iPos[1]] != 'x';
        case 'w': //viendo al oeste
          return mapaRobot[iPos[0]][iPos[1]-2] != 'x';
      }
      break;
    case 'i':     //checando izquierda
      switch(cDir)
      {
        case 'n': //viendo al norte
          return mapaRobot[iPos[0]][iPos[1]-2] != 'x';
        case 'e': //viendo al este
          return mapaRobot[iPos[0]-2][iPos[1]] != 'x';
        case 's': //viendo al sur
          return mapaRobot[iPos[0]][iPos[1]+2] != 'x';
        case 'w': //viendo al oeste
          return mapaRobot[iPos[0]+2][iPos[1]] != 'x';
      }
      break;
    case 'a':     //checando atras
      switch(cDir)
      {
        case 'n': //viendo al norte
          return mapaRobot[iPos[0]+2][iPos[1]] != 'x';
        case 'e': //viendo al este
          return mapaRobot[iPos[0]][iPos[1]-2] != 'x';
        case 's': //viendo al sur
          return mapaRobot[iPos[0]-2][iPos[1]] != 'x';
        case 'w': //viendo al oeste
          return mapaRobot[iPos[0]][iPos[1]+2] != 'x';
      }
      break;
  }
}

bool mapaSensa(vector < vector <char> > mapaRobot, int iPos[2], char cDir, int iCase){// D E I A
  switch(iCase){
    case 0:       //Derecha
      switch(cDir){
        case 'n':
          return mapaRobot[iPos[0]][iPos[1]+1] == '.';
        case 'e':
          return mapaRobot[iPos[0]+1][iPos[1]] == '.';
        case 's':
          return mapaRobot[iPos[0]][iPos[1]-1] == '.';
        case 'w':
          return mapaRobot[iPos[0]-1][iPos[1]] == '.';
      }
    case 1:       //Enfrente
      switch(cDir){
        case 'n':
          return mapaRobot[iPos[0]-1][iPos[1]] == '.';
        case 'e':
          return mapaRobot[iPos[0]][iPos[1]+1] == '.';
        case 's':
          return mapaRobot[iPos[0]+1][iPos[1]] == '.';
        case 'w':
          return mapaRobot[iPos[0]][iPos[1]-1] == '.';
      }
    case 2:       //Izquierda
      switch(cDir){
        case 'n':
          return mapaRobot[iPos[0]][iPos[1]-1] == '.';
        case 'e':
          return mapaRobot[iPos[0]-1][iPos[1]] == '.';
        case 's':
          return mapaRobot[iPos[0]][iPos[1]+1] == '.';
        case 'w':
          return mapaRobot[iPos[0]+1][iPos[1]] == '.';
      }
    case 3:       //Atras
      switch(cDir){
        case 'n':
          return mapaRobot[iPos[0]+1][iPos[1]] == '.';
        case 'e':
          return mapaRobot[iPos[0]][iPos[1]-1] == '.';
        case 's':
          return mapaRobot[iPos[0]-1][iPos[1]] == '.';
        case 'w':
          return mapaRobot[iPos[0]][iPos[1]+1] == '.';
      }
  }
}

bool mapaSensaVisitado(vector < vector <char> > mapaRobot, int iPos[2], char cDir, int iCase){// D E I A
  switch(iCase){
    case 0:       //Derecha
      switch(cDir){
        case 'n':
          return mapaRobot[iPos[0]][iPos[1]+2] == 'n';
        case 'e':
          return mapaRobot[iPos[0]+2][iPos[1]] == 'n';
        case 's':
          return mapaRobot[iPos[0]][iPos[1]-2] == 'n';
        case 'w':
          return mapaRobot[iPos[0]-2][iPos[1]] == 'n';
      }
    case 1:       //Enfrente
      switch(cDir){
        case 'n':
          return mapaRobot[iPos[0]-2][iPos[1]] == 'n';
        case 'e':
          return mapaRobot[iPos[0]][iPos[1]+2] == 'n';
        case 's':
          return mapaRobot[iPos[0]+2][iPos[1]] == 'n';
        case 'w':
          return mapaRobot[iPos[0]][iPos[1]-2] == 'n';
      }
    case 2:       //Izquierda
      switch(cDir){
        case 'n':
          return mapaRobot[iPos[0]][iPos[1]-2] == 'n';
        case 'e':
          return mapaRobot[iPos[0]-2][iPos[1]] == 'n';
        case 's':
          return mapaRobot[iPos[0]][iPos[1]+2] == 'n';
        case 'w':
          return mapaRobot[iPos[0]+2][iPos[1]] == 'n';
      }
    case 3:       //Atras
      switch(cDir){
        case 'n':
          return mapaRobot[iPos[0]+2][iPos[1]] == 'n';
        case 'e':
          return mapaRobot[iPos[0]][iPos[1]-2] == 'n';
        case 's':
          return mapaRobot[iPos[0]-2][iPos[1]] == 'n';
        case 'w':
          return mapaRobot[iPos[0]][iPos[1]+2] == 'n';
      }
  }
}

bool mapaaSensa(vector < vector <char> > mapaRobot, vector <int> iPos, char cDir, int iCase){// D E I A
  switch(iCase){
    case 0:       //Derecha
      switch(cDir){
        case 'n':
          return mapaRobot[iPos[0]][iPos[1]+1] == '.';
        case 'e':
          return mapaRobot[iPos[0]+1][iPos[1]] == '.';
        case 's':
          return mapaRobot[iPos[0]][iPos[1]-1] == '.';
        case 'w':
          return mapaRobot[iPos[0]-1][iPos[1]] == '.';
      }
    case 1:       //Enfrente
      switch(cDir){
        case 'n':
          return mapaRobot[iPos[0]-1][iPos[1]] == '.';
        case 'e':
          return mapaRobot[iPos[0]][iPos[1]+1] == '.';
        case 's':
          return mapaRobot[iPos[0]+1][iPos[1]] == '.';
        case 'w':
          return mapaRobot[iPos[0]][iPos[1]-1] == '.';
      }
    case 2:       //Izquierda
      switch(cDir){
        case 'n':
          return mapaRobot[iPos[0]][iPos[1]-1] == '.';
        case 'e':
          return mapaRobot[iPos[0]-1][iPos[1]] == '.';
        case 's':
          return mapaRobot[iPos[0]][iPos[1]+1] == '.';
        case 'w':
          return mapaRobot[iPos[0]+1][iPos[1]] == '.';
      }
    case 3:       //Atras
      switch(cDir){
        case 'n':
          return mapaRobot[iPos[0]+1][iPos[1]] == '.';
        case 'e':
          return mapaRobot[iPos[0]][iPos[1]-1] == '.';
        case 's':
          return mapaRobot[iPos[0]-1][iPos[1]] == '.';
        case 'w':
          return mapaRobot[iPos[0]][iPos[1]+1] == '.';
      }
  }
}

///////////////////////////////////////// Busca en el mapa /////////////////////////////////////////////////////////////

bool buscaN(vector < vector <char> > mapaRobot){                    //Una variable sería mejor, consume menos tiempo
  for(int i = 1; i < mapaRobot.size()-1; i+=2)
    for(int j = 1; j < mapaRobot[i].size()-1; j+=2)
      if(mapaRobot[i][j] == 'n')
        return true;
  return false;
}
////////////////////////////////////////////Buscar x tile /////////////////////////////////////////////////////////////
void llenaTabla(vector < vector <int> > &mapaNumero, vector <int> vX, vector < vector <char> > mapaRobot){
  if(mapaaSensa(mapaRobot, vX, 'n', 0)){
    if(mapaNumero[vX[0]][vX[1]+2] == -1 || mapaNumero[vX[0]][vX[1]+2] > (mapaNumero[vX[0]][vX[1]] + 1)){
      mapaNumero[vX[0]][vX[1]+2] = (mapaNumero[vX[0]][vX[1]] + 1);
      vX[1]+=2;
      llenaTabla(mapaNumero, vX, mapaRobot);
      vX[1]-=2;
    }
  }
  if(mapaaSensa(mapaRobot, vX, 'n', 1)){
    if(mapaNumero[vX[0]-2][vX[1]] == -1 || mapaNumero[vX[0]-2][vX[1]] > (mapaNumero[vX[0]][vX[1]] + 1)){
      mapaNumero[vX[0]-2][vX[1]] = (mapaNumero[vX[0]][vX[1]] + 1);
      vX[0]-=2;
      llenaTabla(mapaNumero, vX, mapaRobot);
      vX[0]+=2;
    }
  }
  if(mapaaSensa(mapaRobot, vX, 'n', 2)){
    if(mapaNumero[vX[0]][vX[1]-2] == -1 || mapaNumero[vX[0]][vX[1]-2] > (mapaNumero[vX[0]][vX[1]] + 1)){
      mapaNumero[vX[0]][vX[1]-2] = (mapaNumero[vX[0]][vX[1]] + 1);
      vX[1]-=2;
      llenaTabla(mapaNumero, vX, mapaRobot);
      vX[1]+=2;
    }
  }
  if(mapaaSensa(mapaRobot, vX, 'n', 3)){
    if(mapaNumero[vX[0]+2][vX[1]] == -1 || mapaNumero[vX[0]+2][vX[1]] > (mapaNumero[vX[0]][vX[1]] + 1)){
      mapaNumero[vX[0]+2][vX[1]] = (mapaNumero[vX[0]][vX[1]] + 1);
      vX[0]+=2;
      llenaTabla(mapaNumero, vX, mapaRobot);
      vX[0]-=2;
    }
  }
}

string caminoRegreso(vector < vector <int> > mapaNumero, int iPos[2], vector < vector <char> > mapaRobot){
  string sCamino = "";
  int iPosI=mapaNumero[iPos[0]][iPos[1]];
  while(iPosI>0){
    if(mapaSensa(mapaRobot, iPos, 'n', 0) && mapaNumero[iPos[0]][iPos[1]+2] == iPosI-1){
      sCamino+="d";
      iPosI--;
      iPos[1]+=2;
    }
    if(mapaSensa(mapaRobot, iPos, 'n', 1) && mapaNumero[iPos[0]-2][iPos[1]] == iPosI-1){
      sCamino+="e";
      iPosI--;
      iPos[0]-=2;
    }
    if(mapaSensa(mapaRobot, iPos, 'n', 2) && mapaNumero[iPos[0]][iPos[1]-2] == iPosI-1){
      sCamino+="i";
      iPosI--;
      iPos[1]-=2;
    }
    if(mapaSensa(mapaRobot, iPos, 'n', 3) && mapaNumero[iPos[0]+2][iPos[1]] == iPosI-1){
      sCamino+="a";
      iPosI--;
      iPos[0]+=2;
    }
  }
  return sCamino;
}

string buscaCamino(vector < vector <char> > mapaRobot, vector <int> vX, int iPos[2], char cDirR){
  mapaRobot[iPos[0]][iPos[1]] =  'a';
  vector <int> vT (mapaRobot[0].size(), -1);
  vector < vector <int> > mapaNumero(mapaRobot.size(), vT);
  mapaNumero[vX[0]][vX[1]] = 0;
  llenaTabla(mapaNumero, vX, mapaRobot);
  int iPosR[2]={iPos[0],iPos[1]};
  return caminoRegreso(mapaNumero, iPosR, mapaRobot);
}

string buscaXCerca(vector < vector <char> > mapaRobot, int iPos[2], char cDir, char cBusca){
  string sA = "", sB="";
  vector < vector <int> > vX;
  vector <int> vT(2, 0);
  for(int i=1; i<mapaRobot.size()-1; i+=2)
    for(int j=1; j<mapaRobot[i].size()-1; j+=2)
      if(mapaRobot[i][j] == cBusca){
        vT[0] = i;
        vT[1] = j;
        vX.push_back(vT);
      }
  if(!vX.empty()){
    sA = buscaCamino(mapaRobot, vX[vX.size()-1], iPos, cDir);
  }
  for(int i=0; i < vX.size(); i++){
    sB = buscaCamino(mapaRobot, vX[i], iPos, cDir);
    if(sA.length() > sB.length()){
      sA = sB;
    }
  }
//  cout << "este " << sA << endl;
  return sA;
}



///////////////////////////////////////// Mapea respecto al sensado ////////////////////////////////////////////////////

void mapea(vector < vector <char> > &mapaRobot, int iPos[2], char cDir, char cChecar, bool bPared, int &iRow, int &iCol){//Puede mejorar, pero funciona
  if(bPared){
    if(!espacioParaPoner(mapaRobot, iPos, cDir, cChecar)){
      if(!espacioParaDesplazar(mapaRobot, cDir, cChecar)){
        creaEspacio(mapaRobot, iPos, cDir, cChecar, iRow, iCol);
      }
      else
        desplaza(mapaRobot, cDir, cChecar, iPos);
    }
    if(!yaLlenado(mapaRobot, iPos, cDir, cChecar))
      llenaNoVisitado(mapaRobot, iPos, cDir, cChecar);
    llenaPared(mapaRobot, iPos, cDir, cChecar, '.');
  }
  else
    llenaPared(mapaRobot, iPos, cDir, cChecar, 'w');
}

void sensaDistancia(vector < vector <char> > &mapaRobot, int iPos[2], char cDir, int &iRow, int &iCol, vector < vector <char> > mapa, int iPosV[2]){//quitar virtual en mundo real
  mapea(mapaRobot, iPos, cDir, 'd', sDerecha(mapa, cDir, iPosV), iRow, iCol);
  mapea(mapaRobot, iPos, cDir, 'e', sEnfrente(mapa, cDir, iPosV), iRow, iCol);
  mapea(mapaRobot, iPos, cDir, 'i', sIzquierda(mapa, cDir, iPosV), iRow, iCol);
  mapea(mapaRobot, iPos, cDir, 'a', sAtras(mapa, cDir, iPosV), iRow, iCol);
}

////////////////////////////////////////// Moverse ////////////////////////////////////////////////////////////////////

void turnRight(char &cDir){                           //Modificarse en realidad
  switch(cDir)
  {
    case 'n':
      cDir='e';
      break;
    case 'e':
      cDir='s';
      break;
    case 's':
      cDir='w';
      break;
    case 'w':
      cDir='n';
      break;
  }
}

void turnLeft(char &cDir){                            //Modificarse en realidad
  switch(cDir)
  {
    case 'n':
      cDir='w';
      break;
    case 'e':
      cDir='n';
      break;
    case 's':
      cDir='e';
      break;
    case 'w':
      cDir='s';
      break;
  }
}

void moveFront(vector < vector <char> > &mapaRobot, int iPos[2], char cDir, int iPosV[2]){  //Modificar en realidad y quitar virtual
  switch(cDir)
  {
    case 'n': 
      iPos[0] -= 2;
      iPosV[0] -= 2;
      break;
    case 'e':
      iPos[1] += 2;
      iPosV[1] += 2;
      break;
    case 's':
      iPos[0] += 2;
      iPosV[0] += 2;
      break;
    case 'w':
      iPos[1] -= 2;
      iPosV[1] -= 2;
      break;
  }
}

void  Go_to(string sMov, char &cDir, int iPos[2], vector < vector <char> > &mapaRobot, int iPosV[2]){
  for(int i=0; i<sMov.length(); i++){
//    cout << cDir << endl;
    if(cDir == 'n')
      switch(sMov[i]){
        case 'd':
          turnRight(cDir);
          moveFront(mapaRobot, iPos, cDir, iPosV);
          break;
        case 'e':
          moveFront(mapaRobot, iPos, cDir, iPosV);
          break;
        case 'i':
          turnLeft(cDir);
          moveFront(mapaRobot, iPos, cDir, iPosV);
          break;
        case 'a':
          turnRight(cDir);
          turnRight(cDir);
          moveFront(mapaRobot, iPos, cDir, iPosV);
          break;
      }
    else if(cDir == 'e')
      switch(sMov[i]){
        case 'd':
          moveFront(mapaRobot, iPos, cDir, iPosV);
          break;
        case 'e':
          turnLeft(cDir);
          moveFront(mapaRobot, iPos, cDir, iPosV);
          break;
        case 'i':
          turnRight(cDir);
          turnRight(cDir);
          moveFront(mapaRobot, iPos, cDir, iPosV);
          break;
        case 'a':
          turnRight(cDir);
          moveFront(mapaRobot, iPos, cDir, iPosV);
          break;
      }
    else if(cDir == 's')
      switch(sMov[i]){
        case 'd':
          turnLeft(cDir);
          moveFront(mapaRobot, iPos, cDir, iPosV);
          break;
        case 'e':
          turnRight(cDir);
          turnRight(cDir);
          moveFront(mapaRobot, iPos, cDir, iPosV);
          break;
        case 'i':
          turnRight(cDir);
          moveFront(mapaRobot, iPos, cDir, iPosV);
          break;
        case 'a':
          moveFront(mapaRobot, iPos, cDir, iPosV);
          break;
      }
    else if(cDir == 'w')
      switch(sMov[i]){
        case 'd':
          turnRight(cDir);
          turnRight(cDir);
          moveFront(mapaRobot, iPos, cDir, iPosV);
          break;
        case 'e':
          turnRight(cDir);
          moveFront(mapaRobot, iPos, cDir, iPosV);
          break;
        case 'i':
          moveFront(mapaRobot, iPos, cDir, iPosV);
          break;
        case 'a':
          turnLeft(cDir);
          moveFront(mapaRobot, iPos, cDir, iPosV);
          break;
      }
  }
}

void decidirMoverse(vector < vector <char> > &mapaRobot, int iPos[2], char &cDir, int iPosV[2]){
  if(mapaRobot[iPos[0]][iPos[1]] != 'i')
    mapaRobot[iPos[0]][iPos[1]] = 'v';
  if(mapaSensa(mapaRobot, iPos, cDir, 0) && mapaSensaVisitado(mapaRobot, iPos, cDir, 0)){   //Checa derecha y no visitado
    turnRight(cDir);
    moveFront(mapaRobot, iPos, cDir, iPosV);
  }
  else if(mapaSensa(mapaRobot, iPos, cDir, 1) && mapaSensaVisitado(mapaRobot, iPos, cDir, 1)){    //Checa enfrente y no visitado
    moveFront(mapaRobot, iPos, cDir, iPosV);
  }
  else if(mapaSensa(mapaRobot, iPos, cDir, 2) && mapaSensaVisitado(mapaRobot, iPos, cDir, 2)){    //Checa izquierda y no visitado
    turnLeft(cDir);
    moveFront(mapaRobot, iPos, cDir, iPosV);
  }
  else if(mapaSensa(mapaRobot, iPos, cDir, 3) && mapaSensaVisitado(mapaRobot, iPos, cDir, 3)){    //Checa atras sin pared y no visitado
    for(int i=0; i<2; i++)
      turnRight(cDir);
    moveFront(mapaRobot, iPos, cDir, iPosV);
  }
  else{                         //Busca n más cercano
    //mapaRobot[iPos[0]][iPos[1]] = 'a';
    //muestra_Mapa_C(mapaRobot);
    Go_to(buscaXCerca(mapaRobot, iPos, cDir, 'n'), cDir, iPos, mapaRobot, iPosV); //Recursividad y regresar lista de instrucciones o coordenada
  }
}


int main(){
  vector < vector <char> > mapaVirtual, mapaRobot;              //Solo se ocupa el mapaRobot cuando no sea virtual
  int iRowV = 19, iColV = 19;                         //Solo virtual
  int iPosV[2] = {5, 5};                            //Solo virtual
  int iRowI = 3, iColI = 1;                         //Modificar, es donde se iniciará el mapa
  int iPos[2] = {iRowI, iColI};
  char cDir = 'n';                              //Dirección a donde apunta el robot, no importa, pero por comodidad de donde inicia pongo norte
  int iRowR = 5, iColR = 5;                         //Modificar valores de tamaño

//  carga_Mapa(mapaVirtual, iRowV, iColV);                      //Solo virtual
  //muestra_Mapa_C(mapaVirtual);                            //Solo virtual

  inicia_Mapa(mapaRobot, iRowR, iColR, iRowI, iColI);               //Inicializa el mapa lleno de 'x' y una i donde incio
//  muestra_Mapa_C(mapaRobot);                            //Solo virtual para ver que funciona
  sensaDistancia(mapaRobot, iPos, cDir, iRowR, iColR, mapaVirtual, iPosV);  //quitar virtual en mundo real
  //muestra_Mapa_C(mapaRobot);                            //Solo virtual para ver que funciona
  char cCon = 's';
  while(buscaN(mapaRobot) && cCon == 's'){
    //muestra_Mapa_C(mapaRobot);
    decidirMoverse(mapaRobot, iPos, cDir, iPosV);             //Quitar virtual
    sensaDistancia(mapaRobot, iPos, cDir, iRowR, iColR, mapaVirtual, iPosV);//Quitar virtual
    //muestra_Mapa_C(mapaRobot);                          //Solo virtual para ver que funciona
    //cin >> cCon;
  }
//  cout << "let's go home " << endl;
  mapaRobot[iPos[0]][iPos[1]] = 'f';
  //muestra_Mapa_C(mapaRobot);
  Go_to(buscaXCerca(mapaRobot, iPos, cDir, 'i'), cDir, iPos, mapaRobot, iPosV);
  //muestra_Mapa_C(mapaRobot);
  mapaRobot[iPos[0]][iPos[1]] = 'f';
  //muestra_Mapa_C(mapaRobot);
//  cout << "I'm home " << endl;

  //Moverse mientras exista n   //Checar el mapa o llevar un contador?
    //Derecha
    //Enfrente
    //Izquierda
    //Atras
    //Buscar n más cercano e ir
  //Poner f donde acabó
  //Buscar camino más cercano de f a i
}

//x= espacio vacio
//n= tile
//w= wall
//.= nowall

/*x w x w x w x w x w x w x w x w x w x
w n . n . n . n . n . n . n . n . n w 
x . x . x . x . x . x . x . x . x . x
w n . n . n . n . n . n . n . n . n w  
x . x . x . x . x . x . x . x . x . x
w n . n . n . n . n . n . n . n . n w 
x . x . x . x . x . x . x . x . x . x
w n . n . n . n . n . n . n . n . n w 
x . x . x . x . x . x . x . x . x . x
w n . n . n . n . n . n . n . n . n w 
x . x . x . x . x . x . x . x . x . x
w n . n . n . n . n . n . n . n . n w 
x . x . x . x . x . x . x . x . x . x
w n . n . n . n . n . n . n . n . n w 
x . x . x . x . x . x . x . x . x . x
w n . n . n . n . n . n . n . n . n w 
x . x . x . x . x . x . x . x . x . x
w n . n . n . n . n . n . n . n . n w 
x w x w x w x w x w x w x w x w x w x*/
void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
