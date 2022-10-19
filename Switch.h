#pragma once

class _Switch {
  private:
    byte _pin;
    bool state = false;

  public:
    _Switch(byte pin, void (*func)()) {
      _pin = pin;
      pinMode(_pin, INPUT);
      attachInterrupt(digitalPinToInterrupt(_pin), func, RISING);
    };

};
