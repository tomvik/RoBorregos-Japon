//definicion de motores
#define motorLBF 9 //
#define motorRBB 10 //ya
#define motorLFF 7
#define motorRFF 12 //ya
#define motorLBB 8
#define motorRBF 11 //ya
#define motorLFB 6 //ya
#define motorRFB 13 //ya
int iLeftPow = 100;
int iRightPow = 100;

//definicion de ultrasonicos
#define trigger1 23 //derecha frontal 
#define echo1 25
#define trigger2 31 //Frontal Central
#define echo2 33
#define trigger3 27 //frente derecho
#define echo3 29
#define trigger4 47 //frente izquierdo
#define echo4 49
#define trigger5 40 //izquierda posterior
#define echo5 38
#define trigger6 A15 //izquierda frontal
#define echo6 A14

//librerias del giroscopio
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU9150Lib.h"
#include "CalLib.h"
#include <dmpKey.h>
#include <dmpmap.h>
#include <inv_mpu.h>
#include <inv_mpu_dmp_motion_driver.h>
#include <EEPROM.h>

//definicion de giroscopio
#define  DEVICE_TO_USE          0
#define MPU_UPDATE_RATE         (20)
#define MAG_UPDATE_RATE         (10)
#define  MPU_MAG_MIX_GYRO_ONLY  0
#define MPU_LPF_RATE            40
MPU9150Lib MPU;

//PID Gyro
int KPG = 2;
int KIG = 0;
int KDG = 0;
int iLine;
int iErrorNow = 0;
int iErrorPasado = 0;
int iErrorAc = 0;
int iPowNow = 0;

float distanceFront(){
  digitalWrite(trigger3, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger3,HIGH);
  delayMicroseconds(5);
  digitalWrite(trigger3, LOW);
  float distance1 = pulseIn(echo3, HIGH);
  distance1 = distance1/29/2;
  digitalWrite(trigger4, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger4, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigger4,LOW);
  float distance2 = pulseIn(echo4,HIGH);
  distance2 = distance2/29/2;
  return (distance1 + distance2)/2;
}

int giroscopioZ(){
  while(!MPU.read());
  int zAngle = MPU.readZAxis(MPU.m_fusedEulerPose);
  if(zAngle < 0)
    return zAngle+=360;
  
  return zAngle;
}

int giroscopioY(){
  while(!MPU.read());
  int yAngle = MPU.readYAxis(MPU.m_fusedEulerPose);
  if(yAngle < 0)
    return yAngle+=360;
  
  return yAngle;
}


void Alaign()
{
  iRightPow = constrain(iRightPow, 0, 255);
  iLeftPow = constrain(iLeftPow, 0, 255);
  while(!MPU.read());
  iLine = giroscopioZ();
  /*if(iLine <= 5)
    iLine +=5;
  else if (iLine >= 355)
    iLine -=5;*/
  Serial.print("line ");Serial.println(iLine);
  while(distanceFront() > 10)
  {
    iErrorNow = iLine - giroscopioZ();
    iErrorAc += iErrorPasado;
    iPowNow = 0;
    iPowNow = KPG*iErrorNow + KIG*iErrorAc + KDG*(iErrorNow-iErrorPasado);
    iErrorPasado = iErrorNow;
    if(iErrorNow > 0)
    {
      iRightPow+=iPowNow;
      iLeftPow -= iPowNow;
      /*if((iRightPow + iPowNow) >= 255)
        iRightPow = 255;
      if((iLeftPow-iPowNow) <= 0)
        iLeftPow = 0;*/
      analogWrite(motorRFF, iRightPow);
      analogWrite(motorRBF, iRightPow);
      analogWrite(motorLFF, iLeftPow);
      analogWrite(motorLBF, iLeftPow);
    }
    else if (iErrorNow < 0)
    {
      iLeftPow += iPowNow;
      iRightPow -= iPowNow;
      /*if((iLeftPow + iPowNow) >= 255)
        iLeftPow = 255;
      if((iRightPow-iPowNow) <= 0)
        iRightPow = 0;*/
      analogWrite(motorRFF, iRightPow);
      analogWrite(motorRBF, iRightPow);
      analogWrite(motorLFF, iLeftPow);
      analogWrite(motorLBF, iLeftPow); 
    }
    else
    {
      iRightPow = 100;
      iLeftPow = 100;
      analogWrite(motorRFF, iRightPow);
      analogWrite(motorRBF, iRightPow);
      analogWrite(motorLFF, iLeftPow);
      analogWrite(motorLBF, iLeftPow); 
    }
    Serial.print("iErrorNow ");Serial.println(iErrorNow);
    Serial.print("iRightPow ");Serial.println(iRightPow);
    Serial.print("iLeftPow ");Serial.println(iLeftPow);
  }
  powerOff();
}

void powerOff(){
  analogWrite(motorRFF, 0);
  analogWrite(motorLFF, 0);
  analogWrite(motorRBF, 0);
  analogWrite(motorLBF, 0);
  analogWrite(motorRFB, 0);
  analogWrite(motorLFB, 0);
  analogWrite(motorRBB, 0);
  analogWrite(motorLBB, 0);
  delay(200);
}

void setup() {
  Serial.begin(9600);
  //setup de ultrasonicos
  pinMode(trigger1, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(trigger2, OUTPUT);
  pinMode(echo2, INPUT);
  pinMode(trigger3, OUTPUT);
  pinMode(echo3, INPUT);
  pinMode(trigger4, OUTPUT);
  pinMode(echo4, INPUT);
  pinMode(trigger5, OUTPUT);
  pinMode(echo5, INPUT);
  pinMode(trigger6, OUTPUT);
  pinMode(echo6, INPUT);

  //setup de motores
  pinMode(motorRFF, OUTPUT);
  pinMode(motorRBF, OUTPUT);
  pinMode(motorLFF, OUTPUT);
  pinMode(motorLBF, OUTPUT);
  pinMode(motorRFB, OUTPUT);
  pinMode(motorRBB, OUTPUT);
  pinMode(motorLFB, OUTPUT);
  pinMode(motorLBB, OUTPUT);

  //setup de giroscopio
  Wire.begin();
  MPU.selectDevice(DEVICE_TO_USE);
  MPU.init(MPU_UPDATE_RATE, MPU_MAG_MIX_GYRO_ONLY, MAG_UPDATE_RATE, MPU_LPF_RATE);

  delay(12000);
}

void loop() {
  analogWrite(motorRFF, 200);
  analogWrite(motorRBF, 200);
  analogWrite(motorLFF, 200);
  analogWrite(motorLBF, 200);
  delay(2000);
  powerOff();
}
