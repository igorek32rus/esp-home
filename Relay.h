#pragma once

class _Relay {
  private:
    byte _pin;
    bool state = false;
    void acceptState();

  public:
    _Relay(byte pin) {
      _pin = pin;
      pinMode(_pin, OUTPUT);
    };

    void changeState();
    void changeOn();
    void changeOff();
    bool getState();
};

void _Relay::changeState() {
  state = !state;
  acceptState();
}

void _Relay::changeOn() {
  state = true;
  acceptState();
}

void _Relay::changeOff() {
  state = false;
  acceptState();
}

bool _Relay::getState() {
  return state;
}

void _Relay::acceptState() {
  if (state) {
    digitalWrite(_pin, HIGH);
  } else {
    digitalWrite(_pin, LOW);
  }
}
