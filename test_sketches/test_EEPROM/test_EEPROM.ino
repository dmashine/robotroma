#include <EEPROM.h>
#define MAX_I 3

int led[MAX_I] = {2, 3, 4};  // Светодиоды подключены к 2, 3, 4 пинам
int btn[MAX_I] = {8, 9, 10}; // Кнопки подключены к 8, 9, 10

// the setup routine runs once when you press reset:
void setup() {
  // инициализация светодиодов и кнопок
  for (int i = 0; i < MAX_I; i++)
    pinMode(led[i], OUTPUT);
  for (int i = 0; i < MAX_I; i++)
    pinMode(btn[i], INPUT);
  sig(); // При старте вызовем функциию показа состояния
}

// the loop routine runs over and over again forever:
void loop() {
  for (int i = 0; i < MAX_I; i++)
  { if (digitalRead(btn[i]) == HIGH) // Если кнопка нажата
      EEPROM[i] = !EEPROM[i];        // Переключим значение в памяти
  }
  sig();
}
void sig(void) {
  // Зажигаем и гасим светодиоды соответственно значению в памяти
  for (int i = 0; i < MAX_I; i++)
    digitalWrite(led[i], EEPROM[i]);
}
