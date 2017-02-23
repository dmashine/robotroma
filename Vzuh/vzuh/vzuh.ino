void setup() {
  pinMode(PB3, OUTPUT);
  pinMode(PB2, OUTPUT);
  pinMode(PB1, INPUT);
  pinMode(PB1, INPUT);
  pinMode(PB0, INPUT);
  pinMode(PB4, INPUT);
}

void loop() {

      digitalWrite(PB3, 255); 
      digitalWrite(PB2, 255);
      delay(1000);
      digitalWrite(PB3, 0); 
      digitalWrite(PB2, 0);
      delay(1000);

}
