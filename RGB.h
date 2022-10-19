#pragma once

class _RGB {
  private:
    byte _Rpin;
    byte _Gpin;
    byte _Bpin;
    String _color = "#000000";
    bool state = false;

  public:
    byte Rstate = 0;
    byte Gstate = 0;
    byte Bstate = 0;
    byte nowR = 0;
    byte nowG = 0;
    byte nowB = 0;

    _RGB(byte Rpin, byte Gpin, byte Bpin) {
      _Rpin = Rpin;
      _Gpin = Gpin;
      _Bpin = Bpin;
      pinMode(_Rpin, OUTPUT);
      pinMode(_Gpin, OUTPUT);
      pinMode(_Bpin, OUTPUT);
      digitalWrite(_Rpin, LOW);
      digitalWrite(_Gpin, LOW);
      digitalWrite(_Bpin, LOW);
    };

    bool rgb_mode = false;

    void changeColor(String color);
    void changeState(bool st);
    bool getState();
    byte getR();
    byte getG();
    byte getB();
    String getColor();
    void TurnOnOff(byte pinSwitchRGB);
};

String _RGB::getColor() {
  return _color;
}

bool _RGB::getState() {
  return state;
}

byte _RGB::getR() {
  return Rstate;
}

byte _RGB::getG() {
  return Gstate;
}

byte _RGB::getB() {
  return Bstate;
}

void _RGB::changeState(bool newState) {
  state = newState;
}

void _RGB::changeColor(String color) {
  _color = color;
  long number = (int) strtol( &color[1], NULL, 16);
  byte R = number >> 16;
  byte G = (number >> 8) & 0xFF;
  byte B = number & 0xFF;

  Rstate = R;
  Gstate = G;
  Bstate = B;

  if ((Rstate != 0) || (Gstate != 0) || (Bstate != 0)) {
    state = true;
  } else {
    state = false;
  }
}

void _RGB::TurnOnOff(byte pinSwitchRGB) {
  boolean SwitchState = false;    // проверка на то единожды ли была нажата кнопка

  delay(10);
  unsigned long funcDelay = millis();
  while (millis() - funcDelay < 500) {
    delay(10);    // здесь обязателен delay, так как wifi во время задержек выполняет обработку
  } // задержка 0.5 сек
  delay(10);

  byte R = getR();
  byte G = getG();
  byte B = getB();

  byte maxRGB = max(R, G);
  maxRGB = max(maxRGB, B);

  int Rstep = 1;
  if (R != 0) Rstep = maxRGB / R;
  int Gstep = 1;
  if (G != 0) Gstep = maxRGB / G;
  int Bstep = 1;
  if (B != 0) Bstep = maxRGB / B;

  int iR = 0, iG = 0, iB = 0;

  boolean trig = false;

  delay(10);
  while (digitalRead(pinSwitchRGB) && state) {
    SwitchState = true;
    delay(10);

    if (!rgb_mode) {
      if ((R == 255) || (G == 255) || (B == 255)) {
        trig = true;
      }

      if (!trig) {
        iR++;
        iG++;
        iB++;
        delay(10);

        if ((iR == Rstep) && (R != 0)) {
          iR = 0;
          R++;
          delay(10);
        }
        if ((iG == Gstep) && (G != 0)) {
          iG = 0;
          G++;
          delay(10);
        }
        if ((iB == Bstep) && (B != 0)) {
          iB = 0;
          B++;
          delay(10);
        }

        delay(10);
        analogWrite(_Rpin, R);
        analogWrite(_Gpin, G);
        analogWrite(_Bpin, B);

        Rstate = R;
        Gstate = G;
        Bstate = B;

        nowR = R;
        nowG = G;
        nowB = B;
      }

    } else {
      iR++;
      iG++;
      iB++;
      delay(10);

      if ((iR == Rstep) && (R > 1)) {
        iR = 0;
        R--;
        delay(10);
      }

      if ((iG == Gstep) && (G > 1)) {
        iG = 0;
        G--;
        delay(10);
      }

      if ((iB == Bstep) && (B > 1)) {
        iB = 0;
        B--;
        delay(10);
      }

      delay(10);
      analogWrite(_Rpin, R);
      analogWrite(_Gpin, G);
      analogWrite(_Bpin, B);

      Rstate = R;
      Gstate = G;
      Bstate = B;

      nowR = R;
      nowG = G;
      nowB = B;
    }

    delay(10);
    funcDelay = millis();
    while (millis() - funcDelay < 10) {
      delay(10);    // здесь обязателен delay, так как wifi во время задержек выполняет обработку
    }
    delay(10);
  }

  if (SwitchState) {
    rgb_mode = !rgb_mode;   // после регулировки яркости, сменить на противоположный режим (увеличение/уменьшение яркости)
  } else {    // вкл/выкл ленты
    delay(10);
    changeState(!getState());
  }
}
