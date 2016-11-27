#include <EEPROM.h>
#define MAX_I 3

int led[MAX_I] = {2, 3, 4};
int btn[MAX_I] = {8, 9, 10};

// the setup routine runs once when you press reset:
void setup() {
  for(int i=0; i< MAX_I; i++)
    pinMode(led[i], OUTPUT);
  for(int i=0; i< MAX_I; i++)
    pinMode(btn[i], INPUT);
  sig();
}

// the loop routine runs over and over again forever:
void loop() {
  for(int i=0; i< MAX_I; i++)
  { if(digitalRead(btn[i]) == HIGH){
      EEPROM[i] = !EEPROM[i];
  }}
  sig();

}
void sig(void){
  for(int i=0; i< MAX_I; i++)
  {
    if(EEPROM[i] == HIGH){
      digitalWrite(led[i], HIGH);
    }
    else{
      digitalWrite(led[i], LOW);
    }
  }
}
