#define uno 16
#define dos 17

void setup() {
  // put your setup code here, to run once:
  pinMode(uno, OUTPUT);
  pinMode(dos, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(uno, HIGH);
  digitalWrite(dos, HIGH);
}
