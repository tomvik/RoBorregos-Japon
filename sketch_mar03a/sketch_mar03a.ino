#define motor1 11
#define motor2 10

void setup() {
  // put your setup code here, to run once:
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  //delay(10000);
  analogWrite(motor1, 255);
 // analogWrite(motor2, 255);
}
