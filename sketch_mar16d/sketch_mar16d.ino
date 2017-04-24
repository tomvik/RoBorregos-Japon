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

  //  Motors
    #define motorRFF 8                                      //  Right Front Forward
    #define motorRFB 9                                      //  Right Front Backward
    #define motorRBF 6                                      //  Right Back Forward
    #define motorRBB 7                                      //  Right Back Backward
    #define motorLFF 12                                     //  Left Front Forward
    #define motorLFB 4                                      //  Left Front Backward
    #define motorLBF 10                                     //  Left Back Forward
    #define motorLBB 11                                     //  Left Back Backward
    
      int rotationXaxis(){                              //returns angle in Z axis (inclination)
    while(!MPU.read());
      int xAngle = round(MPU.readXAxis(MPU.m_fusedEulerPose));
      return xAngle;
  }

    int rotationZaxis(){                              //returns angle in Z axis (normal)
    while(!MPU.read());
      int zAngle = round(MPU.readZAxis(MPU.m_fusedEulerPose));
      return zAngle + 180;
  }

  int promedioX(){
    int promX = 0;
    for(int i = 0; i < 8; i++)
    {
      promX += rotationXaxis();
    }
    promX/=8;
    return promX;
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
      delay(100);
  }

  void bajarRampa(){
      analogWrite (motorLFF, 100);
      analogWrite (motorLBF, 100);
      analogWrite (motorRFF, 100);
      analogWrite (motorRBF, 100);
      while(rotationXaxis() > 1.5)
        Serial.print("");
      delay(500);
      powerOff();
      analogWrite (motorLFB, 93);
      analogWrite (motorLBB, 100);
      analogWrite (motorRFB, 100);
      analogWrite (motorRBB, 100);
      delay(300);
      powerOff();
      analogWrite (motorLFF, 200);
      analogWrite (motorLBF, 200);
      analogWrite (motorRFF, 200);
      analogWrite (motorRBF, 200);
      delay(500);
      powerOff();
      analogWrite (motorLFB, 93);
      analogWrite (motorLBB, 100);
      analogWrite (motorRFB, 100);
      analogWrite (motorRBB, 100);
      delay(300);
      powerOff();
      analogWrite (motorLFF, 200);
      analogWrite (motorLBF, 200);
      analogWrite (motorRFF, 200);
      analogWrite (motorRBF, 200);
      delay(500);
      powerOff();
  }

  void subirRampa(){
    analogWrite (motorLFF, 255);
    analogWrite (motorLBF, 255);
    analogWrite (motorRFF, 255);
    analogWrite (motorRBF, 255);
    while(rotationXaxis() < -10)
      Serial.print("");
    delay(500);
    powerOff();
    analogWrite (motorLFB, 93);
    analogWrite (motorLBB, 100);
    analogWrite (motorRFB, 100);
    analogWrite (motorRBB, 100);
    delay(350);
    powerOff();
  }

    void calibGyro(){                               //function to calibrate gyroscope during setup
    analogWrite (motorLFF, 140);
      analogWrite (motorLBF, 150);
      analogWrite (motorRFB, 150);
      analogWrite (motorRBB, 150);
      delay(100);
      powerOff();
      analogWrite (motorLFB, 140);
      analogWrite (motorLBB, 150);
      analogWrite (motorRFF, 150);
      analogWrite (motorRBF, 150);
      delay(100);
      powerOff();
      while(rotationZaxis() < 150);
        Serial.print("");
      delay(100);
  }
  
  void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
      //  Motors
      pinMode(motorRFF, OUTPUT);
      pinMode(motorLFF, OUTPUT);
      pinMode(motorRBF, OUTPUT);
      pinMode(motorLBF, OUTPUT);
      pinMode(motorRFB, OUTPUT);
      pinMode(motorLFB, OUTPUT);
      pinMode(motorRBB, OUTPUT);
      pinMode(motorLBB, OUTPUT);
      
  //  Gyroscope
      Wire.begin();
      MPU.selectDevice(DEVICE_TO_USE);
      //MPU.init(MPU_UPDATE_RATE, MPU_MAG_MIX_GYRO_ONLY, MAG_UPDATE_RATE, MPU_LPF_RATE);
      MPU.init(MPU_UPDATE_RATE, MPU_MAG_MIX_GYRO_AND_MAG, MAG_UPDATE_RATE, MPU_LPF_RATE);

      //calibGyro();
        
    analogWrite (motorLFF, 130);
    analogWrite (motorLBF, 130);
    analogWrite (motorRFF, 130);
    analogWrite (motorRBF, 130);
}

void loop() {
  // put your main code here, to run repeatedly:
      while(!MPU.read());
      //MPU.readYAcAxis(MPU.m_rawAccel);
      Serial.println(MPU.readYAcAxis(MPU.m_rawAccel));
      MPU.printVector(MPU.m_rawAccel); 
      Serial.println();
/*    while(true){
      if(promedioX() < -14)
        {subirRampa();
        delay(4000);}
      else if(promedioX() > 14)
        {bajarRampa();
        delay(4000);}
    }*/
}
