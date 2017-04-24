int cnt=0;
void setup() {
 
  Serial.begin(9600);
  
}
 
void loop() {
  
  Serial.print(5);
  Serial.print(" ");
  Serial.println(4);
  cnt=cnt+1;
  delay(100);
  
}
