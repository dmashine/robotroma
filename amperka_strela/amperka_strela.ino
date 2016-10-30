//Arduino Bluetooth RC Car in Strela
//Original App:
//https://play.google.com/store/apps/details?id=braulio.calle.bluetoothRCcontroller
#include <NewPing.h>
#include <Wire.h> // Библиотека для работы с I2C
#include <Strela.h> // Библиотека для работы со Стрелой
#include <PID_v1.h> //Библиотека пид-регулятора

#define LINE_L A4
#define LINE_R A5

enum State {
  RC_CONTROL,
  SONAR_CONTROL,
  LINE_FOLLOW
};

int velocity = 0;  //Здесь будет храниться значение скорости

int defaultSpeed = 100; // это число мы будем использовать в логике поворотов

NewPing sonarL(A3, A2); // Левый подключен к P5 и P6
NewPing sonarR(A1, A0); // Правый подключен к P7 и P8
int durationL;
int durationR;

int brightnL;
int brightnR;

double Setpoint, Input, Output;
int d;

State state;

PID myPID(&Input, &Output, &Setpoint, 5, 0, 100, DIRECT); //Пид-регулятор

void setup()
{
  Serial1.begin(9600);     //Bluetooth Bee по умолчанию использует эту скорость
  motorConnection(1, 0);   // Я неправильно прикрутил один мотор
  //поэтому, чтобы их не перекручивать
  //можно воспользоваться этой функцией.
  //Направление вращения мотора 1 будет изменено.

  Serial.begin(9600);
  pinMode(LINE_L, INPUT);
  pinMode(LINE_R, INPUT);
  Setpoint = 0; // Пид-регулятор разворачивает на серидину коридора
  Input = 0;
  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  //velocity = 255; //скорость максимальна
  state = RC_CONTROL;
}

void loop()
{
  readSensors();
  d = abs(durationL - durationR);
  int Kv = 0;
  char dataIn;
  switch (state) {
    case SONAR_CONTROL:
      Kv = velocity;
      Input = (float)(durationL - durationR); //duration типа int

      if (Serial1.available() > 0)
      {
        drive(0, 0);
        state = RC_CONTROL;
      }
      if (d > 30) { // Велика разница между сенсорами. Поворачиваем
        myPID.Compute();
        Kv = (int)(Output - 126) * 2;
        drive(-Kv, Kv);
        delay(50);
      }
      else if ((durationL < 18) && (durationR < 18)) { // Впереди препятствие?
        drive(-230, -230); //едем назад
        delay(50);
      }
      else if (durationL < 18) { // Слева угол?
        drive(-230, 0); //едем назад
        delay(50);
      }
      else if (durationL < 18) { // Справа угол?
        drive(0, -230); //едем назад
      }
      else // Угла нет, препятствия нет, едем вперед
      {
        drive(230, 230); //едем вперед
      }
      //}
      break;
    case RC_CONTROL:
      if (Serial1.available() > 0)    //Если появились новые команды
      {
        control();                    //вызываем функцию управления
      }
      break;
  }
  Serial1.print("*G");
  Serial1.print(durationL);
  Serial1.print(",");
  Serial1.print(durationR);
  Serial1.println();
  Serial1.print("*L");
  Serial1.println(brightnL);
  Serial1.print("*R");
  Serial1.println(brightnR);

  Serial.print("dL-");
  Serial.print(durationL);
  Serial.print("\tdR-");
  Serial.print(durationR);
  Serial.print("\td-");
  Serial.print(d);
  Serial.print("\tO-");
  Serial.print(Output);
  Serial.print("\tKv-");
  Serial.print(Kv);
  Serial.println();
  Serial.print("bL ");
  Serial.println(brightnL);
  Serial.print("bR ");
  Serial.println(brightnR);
  //delay(1000);
}
void control()  // функция управления
{
  char dataIn = Serial1.read();  //Считаем значение пришедшей команды

  if (dataIn == 'F')         //Если пришла команда "F"
    drive(velocity, velocity);   //едем вперёд

  else if (dataIn == 'B')    //или если пришла команда "B"
    drive(-velocity, -velocity); //едем назад

  else if (dataIn == 'L')    //или если пришла команда "L"
    drive(-velocity, velocity);  //поворачиваем налево на месте

  else if (dataIn == 'R')    //или если пришла команда "R"
    drive(velocity, -velocity);  //поворачиваем направо на месте

  else if (dataIn == 'I')    //или если пришла команда "I", едем вперёд и направо
    drive(defaultSpeed + velocity, defaultSpeed - velocity);

  else if (dataIn == 'J')    //или если пришла команда "J", едем назад и направо
    drive(-defaultSpeed - velocity, -defaultSpeed + velocity);

  else if (dataIn == 'G')   //или если пришла команда "I", едем вперёд и налево
    drive(defaultSpeed - velocity, defaultSpeed + velocity);

  else if (dataIn == 'H')   //или если пришла команда "H", едем назад и налево
    drive(-defaultSpeed + velocity, -defaultSpeed - velocity);

  else if (dataIn == 'S')   //или если пришла команда "S", стоим
    drive(0, 0);

  else if (dataIn == 'U')   //или если "U", зажигаем "передние фары"
  {
    uDigitalWrite(L2, HIGH);
    uDigitalWrite(L3, HIGH);
  }
  else if (dataIn == 'u')   //или если "u", гасим "передние фары"
  {
    uDigitalWrite(L2, LOW);
    uDigitalWrite(L3, LOW);
  }
  else if (dataIn == 'W')   //или если "W", зажигаем "задние фары"
  {
    uDigitalWrite(L1, HIGH);
    uDigitalWrite(L4, HIGH);
  }
  else if (dataIn == 'w')   ////или если "w", гасим "задние фары"
  {
    uDigitalWrite(L1, LOW);
    uDigitalWrite(L4, LOW);
  }

  // если к нам пришло значение от 0 до 9
  else if (((dataIn - '0') >= 0) && ((dataIn - '0') <= 9))
    velocity = (dataIn - '0') * 25; //сохраняем новое значение скорости

  else if (dataIn == 'q') //если "q" - полный газ!
    velocity = 255;

  else if (dataIn == 'Y') //Переключаемся
  { drive(0, 0);
    state = SONAR_CONTROL;
  }
}
void readSensors() {
  durationL = sonarL.ping_cm();
  durationR = sonarR.ping_cm();
  brightnL = analogRead(LINE_L);
  brightnR = analogRead(LINE_R);
}

