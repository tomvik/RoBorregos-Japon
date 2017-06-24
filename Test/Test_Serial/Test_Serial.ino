int iC = 1;
char input;
void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  delay(100);
  Serial2.println("Enciende");
}

void loop() {
  if(Serial2.available()){
    while(Serial2.available())
      input = (char)Serial2.read();
    if(iC == 1){
      Serial2.println("UNO2");
      Serial.print("LEO: "); Serial.println(input);
    }
    else if(iC == 2){
      Serial2.println("DOS2");
      Serial.print("LEO: "); Serial.println(input);
    }
    else if(iC == 3){
      Serial2.println("TRES2");
      Serial.print("LEO: "); Serial.println(input);
    }
    else{
      Serial2.println("MAYOR2");
      Serial.print("LEO: "); Serial.println(input);
      iC = 0;
    }
    iC++;
  }
  else{
    Serial.println("NO");
    Serial2.println("NO");
  }
  delay(500);
}
