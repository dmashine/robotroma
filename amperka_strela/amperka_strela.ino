//Arduino Bluetooth RC Car in Strela
//Original App:
//https://play.google.com/store/apps/details?id=braulio.calle.bluetoothRCcontroller
#include <NewPing.h>
#include <Wire.h> // Библиотека для работы с I2C
#include <Strela.h> // Библиотека для работы со Стрелой
#include <PID_v1.h> //Библиотека пид-регулятора

#define LINE_L A4
#define LINE_R A5
#define BLACK_B 300

enum State {
  RC_CONTROL,
  SONAR_CONTROL,
  LINE_FOLLOW
};

typedef struct {
  double q; //process noise covariance
  double r; //measurement noise covariance
  double x; //value
  double p; //estimation error covariance
  double k; //kalman gain
} kalman_state;

void kalman_update(kalman_state* state, double measurement);
kalman_state kalman_init(double q, double r, double p, double intial_value);

int velocity = 0;  //Здесь будет храниться значение скорости

int defaultSpeed = 50; // это число мы будем использовать в логике поворотов

NewPing sonarL(A3, A2); // Левый подключен к P5 и P6
NewPing sonarR(A1, A0); // Правый подключен к P7 и P8
int durationL;
int durationR;
kalman_state k_sR;
kalman_state k_sL;

int brightnL;
int brightnR;

double Setpoint, Input, Output;

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

  //setup kalman
  // kalman_init(double q, double r, double p, double intial_value)
  k_sR = kalman_init(1.0, 2.0, 1.0, 40);
  k_sL = kalman_init(1.0, 2.0, 1.0, 40);

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
  int d = abs(durationL - durationR);
  int l = abs(brightnL - brightnR);
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
        drive(0, 0); // стоп
        readSensors(); //осмотрелись
        if (durationL < durationR)
          drive(230, -230); //развернулись
        else
          drive(230, -230); //развернулись
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
    case LINE_FOLLOW:
      //Kv = velocity;
      //int Input = (float)(brightnL - brightnR); //duration типа int
      Kv = map(l, 0, 1000, 230, 255);
      if (Serial1.available() > 0)
      {
        drive(0, 0);
        state = RC_CONTROL;
      }
      if ((brightnL < BLACK_B) || (brightnR < BLACK_B)) { // Алярм! Выехали с линии
        if (brightnL - brightnR < -100)
        { drive(-Kv, Kv);
          delay(50);
        }
        else if (brightnL - brightnR > 100){
          drive(Kv, -Kv);
          delay(50);
        }
      }
      else {
        drive(230, 23); //выровняли, едем вперед.
      }
      break;
    case RC_CONTROL:
      if (Serial1.available() > 0)    //Если появились новые команды
      {
        control();                    //вызываем функцию управления
      }
      break;
  }
  if (millis() % 100 == 0) { // Не перегружаем Serial()
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
    Serial.print("\tbL-");
    Serial.print(brightnL);
    Serial.print("\tbR-");
    Serial.print(brightnR);
    Serial.print("\td-");
    Serial.print(d);
    Serial.print("\tl-");
    Serial.print(l);
    Serial.println();
  }
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
  else if (dataIn == 'Z') //Переключаемся
  { drive(0, 0);
    state = LINE_FOLLOW;
  }
}
void readSensors() {
  int dL = sonarL.ping_cm();
  int dR = sonarR.ping_cm();
  kalman_update(&k_sL, dL);
  kalman_update(&k_sR, dR);
  durationL = (int)k_sL.x;
  durationR = (int)k_sR.x;
  brightnL = analogRead(LINE_L);
  brightnR = analogRead(LINE_R);
}

// ****
// Калман
// ****

kalman_state kalman_init(double q, double r, double p, double intial_value)
{
  kalman_state result;
  result.q = q;
  result.r = r;
  result.p = p;
  result.x = intial_value;
  return result;
}
void kalman_update(kalman_state* state, double measurement)
{
  //prediction update
  //omit x = x
  state->p = state->p + state->q;
  //measurement update
  state->k = state->p / (state->p + state->r);
  state->x = state->x + state->k * (measurement - state->x);
  state->p = (1 - state->k) * state->p;
}
