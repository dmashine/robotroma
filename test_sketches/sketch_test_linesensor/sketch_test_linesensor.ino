int Pin = 9; // Пин сенсора подключен к 9 пину Ардуино
 
void setup() { 
  Serial.begin (9600); // Откроем Сериал порт, чтобы проверить что считалось с сенсора
  pinMode(Pin, INPUT); 
} 
 
void loop() { 
  int in = analogRead(Pin); // считали значение с сенсора
  Serial.println(in);  // Тут же вывели
  delay(10); // И небольшая задержка
}
