#include <NewPing.h>

// Моторы подключаются к клеммам M1+, M1-, M2+, M2-
// Если полюса моторов окажутся перепутаны при подключении,
// можно изменить соответствующие константы CON_MOTOR с 0 на 1
#define CON_MOTOR1     1
#define CON_MOTOR2     1

// Motor shield использует четыре контакта 4, 5, 6, 7 для управления моторами
// 4 и 7 — для направления, 5 и 6 — для скорости
#define SPEED_1      5
#define DIR_1        4

#define SPEED_2      6
#define DIR_2        7

// Возможные направления движения робота
#define FORWARD   0
#define BACKWARD  1
#define LEFT      2
#define RIGHT     3

#define TRIGGER_PIN  12
#define ECHO_PIN     11

NewPing sonar(TRIGGER_PIN, ECHO_PIN);

/*
   В функции `go` мы управляем направлением движения и скоростью
*/
void go(int newDirection, int speed)
{
  boolean motorDirection_1, motorDirection_2;

  switch ( newDirection ) {

    case FORWARD:
      motorDirection_1 = true;
      motorDirection_2 = true;
      break;
    case BACKWARD:
      motorDirection_1 = false;
      motorDirection_2 = false;
      break;
    case LEFT:
      motorDirection_1 = true;
      motorDirection_2 = false;
      break;
    case RIGHT:
      motorDirection_1 = false;
      motorDirection_2 = true;
      break;
  }

  // Если мы ошиблись с подключением - меняем направление на обратное
  motorDirection_1 = CON_MOTOR1 ^ motorDirection_1;
  motorDirection_2 = CON_MOTOR2 ^ motorDirection_2;

  // Поехали! Скорость может меняться в пределах от 0 до 255.
  analogWrite(SPEED_1, speed);
  analogWrite(SPEED_2, speed);

  digitalWrite(DIR_1, motorDirection_1);
  digitalWrite(DIR_2, motorDirection_2);
}
void setup()
{
  // Настраивает выводы платы 4, 5, 6, 7 на вывод сигналов
  for (int i = 4; i <= 7; i++)
    pinMode(i, OUTPUT);
  Serial.begin(9600);

}

void loop()
{


  // Едем секунду вперёд на максимальной скорости
  //go(FORWARD, 255);
  int cm = sonar.ping_median();
  Serial.println(cm);
  if ((cm < 1200) and (cm > 0)) {
    go(FORWARD, 0);
    go(LEFT, 200);
  }
  else
  {
    go(FORWARD, 255);
  }
  delay(50);


}
