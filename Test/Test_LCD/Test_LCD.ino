//  LCD Screen
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define I2C_ADDR    0x3F


LiquidCrystal_I2C lcd(I2C_ADDR, 16,2);

void setup() {
  lcd.begin();// Indicamos medidas de LCD
	lcd.backlight();
}

void loop() {
 // put your main code here, to run repeatedly:
 lcd.print("Erer");
}
