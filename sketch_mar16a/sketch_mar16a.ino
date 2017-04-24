  //  Gyroscope
    #include <Wire.h>
    #include "I2Cdev.h"
    #include "MPU9150Lib.h"
    #include "CalLib.h"
    #include <dmpKey.h>
    #include <dmpmap.h>
    #include <inv_mpu.h>
    #include <inv_mpu_dmp_motion_driver.h>
    #include <EEPROM.h>
    #define  DEVICE_TO_USE 0
    #define MPU_UPDATE_RATE (20)
    #define MAG_UPDATE_RATE (10)
    //#define  MPU_MAG_MIX_GYRO_ONLY 0
    #define  MPU_MAG_MIX_GYRO_AND_MAG 10                  // a good mix value
    #define MPU_LPF_RATE 40
    MPU9150Lib MPU;

int rotationXaxis(){                              //returns angle in Z axis (inclination)
  while(!MPU.read());
    int xAngle = round(MPU.readXAxis(MPU.m_fusedEulerPose));
  return xAngle;
}

int promedioX(){
  int promX = 0;
  for(int i = 0; i < 25; i++)
  {
    promX += rotationXaxis();
  }
  promX/=25;
  return promX;
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

      //  Gyroscope
      Wire.begin();
      MPU.selectDevice(DEVICE_TO_USE);
      //MPU.init(MPU_UPDATE_RATE, MPU_MAG_MIX_GYRO_ONLY, MAG_UPDATE_RATE, MPU_LPF_RATE);
      MPU.init(MPU_UPDATE_RATE, MPU_MAG_MIX_GYRO_AND_MAG, MAG_UPDATE_RATE, MPU_LPF_RATE);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(promedioX());
}
