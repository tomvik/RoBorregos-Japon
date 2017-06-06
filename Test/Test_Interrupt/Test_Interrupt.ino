
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  attachInterrupt(5, prueba, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("NORMAL");
  delay(500);
}

void prueba(){
  Serial.println("ME INTERRUMPI");
}

