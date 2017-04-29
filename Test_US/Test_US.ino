#define trig 22
#define ech 23

uint8_t calcDistanciaUS(uint8_t trigger, uint8_t echo){
   digitalWrite(trigger,LOW);
   delayMicroseconds(5);
   digitalWrite(trigger,HIGH);
   delayMicroseconds(10);
   long tiempo = pulseIn(echo,HIGH);
   return uint8_t(0.017 * tiempo);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  pinMode(ech, INPUT);
  pinMode(trig, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(calcDistanciaUS(trig, ech));
}
