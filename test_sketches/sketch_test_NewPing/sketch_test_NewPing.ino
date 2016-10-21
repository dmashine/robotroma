#include <NewPing.h>

int echoPinR = A0; // Правый подключен к P7 и P8
int trigPinR = A1; 

int echoPinL = A2; // Левый подключен к P5 и P6
int trigPinL = A3; 

NewPing sonarL(trigPinL, echoPinL);
NewPing sonarR(trigPinR, echoPinR);
 
void setup() { 
  Serial.begin (9600); 
} 
 
void loop() { 
  int durationL = sonarL.ping(); 
  int durationR = sonarR.ping(); 
  int P = durationL - durationR;
  //Serial.print("L "); 
  //Serial.println(durationL); 
  //Serial.print("R "); 
  //Serial.println(durationR);
  float R = 0.0;
  if (durationR > durationL)
    R = durationR / durationL;
  else 
    R = - (durationL / durationR);
  Serial.println(R);
  delay(10);
}
