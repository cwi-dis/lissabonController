#ifndef _IOTSAINPUT_H_
#define _IOTSAINPUT_H_
#include "iotsa.h"
#include "iotsaApi.h"
#include "iotsaRequest.h"

typedef std::function<bool()> ActivationCallbackType;

class Input : public IotsaRequestContainer {
public:
  Input(bool _actOnPress, bool _actOnRelease, bool _wake=false);
  void setCallback(ActivationCallbackType callback);
  virtual void setup() = 0;
  virtual void loop() = 0;
protected:
  bool actOnPress;
  bool actOnRelease;
  bool wake;
  ActivationCallbackType activationCallback;
};

class Touchpad : public Input {
public:
  Touchpad(int _pin, bool _actOnPress, bool _actOnRelease, bool _wake=false);
  void setup();
  void loop();
  bool pressed;
  uint32_t duration;
protected:
  int pin;
  bool debounceState;
  unsigned int debounceTime;
  uint16_t threshold;
  uint32_t lastChangeMillis;
};

class Button : public Input {
public:
  Button(int _pin, bool _actOnPress, bool _actOnRelease, bool _wake=false);
  void setup();
  void loop();
  bool pressed;
  uint32_t duration;
protected:
  int pin;
  bool debounceState;
  unsigned int debounceTime;
  uint32_t lastChangeMillis;
};

class RotaryEncoder : public Input {
public:
  RotaryEncoder(int _pinA, int _pinB);
  void setup();
  void loop();
  int value;
  uint32_t duration;
protected:
  int pinA;
  int pinB;
  bool pinAstate;
  uint32_t lastChangeMillis;
};

class IotsaInputMod : public IotsaMod {
public:
  IotsaInputMod(IotsaApplication& app, Input **_inputs, int _nInput) : IotsaMod(app), inputs(_inputs), nInput(_nInput) {}
  using IotsaMod::IotsaMod;
  void setup();
  void serverSetup();
  void loop();
  String info() { return ""; }
protected:
  Input **inputs;
  int nInput;

};

#endif
