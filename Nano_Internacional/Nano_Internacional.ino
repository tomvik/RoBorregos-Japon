/* Code made by the RoBorregos team in 2017 for the RoboCup JR. Rescue Maze category.
 * Tomás Lugo, Sebastián Esquer, Ernesto Cervantez, and Alexis Virgen.
 * "El Mariachi" Achieved a third place on the international RoboCup.
 * 
 * This approach for the raspberry didn't work:
 * Master (Arduino Mega) <-> Slave (this arduino) <-> 2 Rapsberry's
 * 
 * We highly recommend this configuration:
 * Rapsberry (Only one, use mirrors) <-> Master (Arduino Mega) <-> Slave (this arduino)
 * 
 * Or another option is:
 * Slave (Arduino Mega and this arduino as in one) <-> Master (Raspberry)
 * The problem with this last configuration is that we have a lot of sensors, and they take A LOT OF TIME to read, 
 * so 2 microcontrollers and the rasp may be the perfect option (configuration 2)
 * 
 * Also, Serial communication may not be the best option. Some digital pins may be good enough, and in coding may be easier and faster
 */
/////////// Libraries ///////////
#include <Wire.h>
#include <Adafruit_MLX90614.h>

/////////// Defining the constants of the pins ///////////
#define sensorOut 2
#define S0 3
#define S1 4
#define S2 5
#define S3 6
#define LED 13
/////////// Address of MLX's ///////////
#define MlxL 0x1C
#define MlxR 0x5C
/////////// Objects of the MLX's ///////////
Adafruit_MLX90614 mlxRight = Adafruit_MLX90614(MlxR), mlxLeft = Adafruit_MLX90614(MlxL);
/////////// Variables to control the lecture ///////////
char cSendMega, cSendRaspD, cLeeRaspD, cLeeMega = 0;
bool bID = true, bIoD;

/////////// Function that returns a number depending on the color ///////////
//2 is checkpoint, 1 is black, 0 is white
uint8_t sensorColor() {
  int frequency = 0;
  uint8_t iR = 0;
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  digitalWrite(LED, HIGH);
  for (uint8_t i = 0; i < 3; i++){
    frequency += pulseIn(sensorOut, LOW);
  }
  frequency /= 3;
  //Serial.println(frequency);
  digitalWrite(LED, LOW);
  if(frequency >= 120)
    iR = 1;
  return iR;
}
/////////// Function that returns a number depending on where is the heat victim ///////////
//0 = nothing, 1 = right, 2 = left, 3 = both
uint8_t sensarTemperatura() {
  uint8_t re = 0;
  if (mlxRight.readObjectTempC() > mlxRight.readAmbientTempC() + 2.75)
    re++;
  if (mlxLeft.readObjectTempC() > mlxLeft.readAmbientTempC() + 2.75)
    re+=2;
  //Serial.print("Derecha: "); Serial.print((mlxRight.readObjectTempC()); Serial.print("Izquierda: "); Serial.println((mlxLeft.readObjectTempC());
  return re;
}
/////////// Function to know that to send to the arduino regarding that said the Raspberry ///////////
//Couldn't really get it to work on round... it's a mess the serial communication
void queDijo(char c, bool &b){
  switch(c){
    case 'L':
      b = true;
      Serial.println("Letra");
      cSendMega |= 0b11100011;
      break;
    case 'H':
      b = true;
      Serial.println("H");
      cSendMega = 0b10000000;
      Serial3.print(cSendMega);
      break;
    case 'S':
      b = true;
      Serial.println("S");
      cSendMega = 0b01000000;
      Serial3.print(cSendMega);
      break;
    case 'U':
      b = true;
      Serial.println("U");
      cSendMega = 0b00100000;
      Serial3.print(cSendMega);
      break;
    case 'N':
      b = true;
      Serial.println("N");
      cSendMega = 0b00010000;
      Serial3.print(cSendMega);
      break;
  }
}

void setup() {
  //Serial ports
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  //Initialize the MLX's
  mlxRight.begin();
  mlxLeft.begin();
  //Defining the pins
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  pinMode(LED, OUTPUT);
  //This are always this way, so it's the right filter
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  //We tell the rasoberry to start looking for a letter
  Serial2.println("Busca");
}
/*
//Serial
  0     compu
  1     rasp left
  2     rasp right
  3     mega
//Color
  0 if it's white
  1 if it's black
//Temp
  0 if there's none
  1 if it's on the right
  2 if it's on the left
//cSendMega (This is the byte of data we sent to the mega)
  H/Letter
  S/Letter
  U/Letter
  NADA
  color
  Left
  Victim/Letter
  Right
///cSendRaspD e I (Info sent to the rasp)
  Busca (Search)
  Identifica (Identify)
///cLeeMega
  Send (M)
  Right:    Identify (I), Search (B);
  Left:  Identify (R), Search (E);
///cLeeRasp (info recieved by the rasp)
  Letter (L)
  H
  S
  U
  No letter (N)
  Didn't recieve anything (W)
*/
void loop() {
  //We always clear the data we're going to send
  cSendMega = 0;
  //If it detected a letter, it doesn't shut up until the mega realizes it. This is part of the problem of the communication
  cLeeRaspD = bID ? cLeeRaspD : 'W';
  //If the mega says to Identify which letter it is, it does so until the mega says another thing. This is part of the problem of the communication
  cLeeMega = (cLeeMega == 'I') ? 'I' : 'N';
  ////////////////////Read Serial Mega
  while(Serial3.available()){
    cLeeMega = (char)Serial3.read();
  }
  ////////////////////Read Serial Rasp Right
  if(Serial2.available()){
    cLeeRaspD = (char)Serial2.read();
  }
  ////////////////////Read Right
  queDijo(cLeeRaspD, bID);
  ////////////////////Temperature
  switch(sensarTemperatura()) {
    case 1:
      //Serial.println("DER");
      cSendMega|=0b00000011;
      break;
    case 2:
      //Serial.println("Izq");
      cSendMega|=0b00000110;
      break;
    case 3:
      //Serial.println("Amb");
      cSendMega|=0b00000111;
      break;
  }
  ////////////////////Color
  switch(sensorColor()){
    case 1:
      //Serial.println("NEGRO");
      cSendMega |= 0b00001000;
      break;
  }
  ////////////////////Serial, case of instructions. Big part of the problem
  switch(cLeeMega){
    case 'I':
      bID = false;
      Serial2.println("Identifica");
      if(cLeeRaspD != 'W' && cLeeRaspD != 'L')
        Serial3.print(cSendMega);
      break;
    case 'B':
      bID = false;
      //Serial.println("BUSCA");
      Serial2.println("Busca");
      break;
    case 'M':
      //Serial.println("MANDA");
      Serial3.print(cSendMega);
      break;
    default:
      Serial3.print(cSendMega);
      break;
  }
}
