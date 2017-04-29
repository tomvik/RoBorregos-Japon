void setup() {
  Serial.begin(9600);
  unsigned long tiempo = millis();
  while(tiempo < 5000){
    Serial.print("MENOR A 5s, "); Serial.println(tiempo);
    tiempo = millis();
  }
  Serial.print("MAYOR A 5s, "); Serial.println(tiempo);
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
