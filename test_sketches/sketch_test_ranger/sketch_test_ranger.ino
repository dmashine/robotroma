// Скетч для проверки работы ультразвукового дальномера
int echoPin = 9; // пин Echo
int trigPin = 8; // пин Trigger
 
void setup() { 
  Serial.begin (9600); 
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
} 
 
void loop() { 
  int duration, cm; 
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW);  // подали сигнал на триггер
  duration = pulseIn(echoPin, HIGH); // Считали с датчика длительность сигнала в мсек 
  cm = duration / 58; // перевели в см
  Serial.println(cm);
  delay(10);
}