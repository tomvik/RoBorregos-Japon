#define led 52
int iC = 1;
void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(9600);
  Serial2.begin(9600);
  while(Serial2.available()){
    Serial2.read();
  }
  delay(100);
  digitalWrite(led, LOW);
  Serial2.println("Enciende");
}

void loop() {
  if(Serial2.available()){
    char input = (char)Serial2.read();
    if(iC == 1){
      Serial2.println("UNO");
      Serial.print("LEO: "); Serial.println(input);
    }
    else if(iC == 2){
      Serial2.println("DOS");
       Serial.print("LEO: "); Serial.println(input);
    }
    else if(iC == 3){
      Serial2.println("TRES");
       Serial.print("LEO: "); Serial.println(input);
    }
    else{
      Serial2.println("MAYOR");
       Serial.print("LEO: "); Serial.println(input);
      iC = 0;
    }
    iC++;
  }
  else{
    Serial2.println("NO");
  }
  delay(500);
}/*
    Serial2.println("ENTRE");
    if((char)Serial.read() == 'e'){
      digitalWrite(led, HIGH);
      Serial2.println("Enciende");
    }
    else{
      digitalWrite(led, LOW);
      Serial2.println("Apaga");
    }
    Serial2.flush();
  }
  else{
    Serial2.println("NO ENTRE");
  }
  delay(1);
}*/
