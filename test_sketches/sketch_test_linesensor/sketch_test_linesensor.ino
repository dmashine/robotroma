int echoPin = 9; 
int trigPin = 8; 
 
void setup() { 
  Serial.begin (9600); 
  pinMode(echoPin, INPUT); 
} 
 
void loop() { 
  int in; 
  in = analogRead(echoPin); 
  Serial.println(in); 
  delay(10);
}
