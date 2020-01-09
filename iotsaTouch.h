#ifndef _IOTSATOUCH_H_
#define _IOTSATOUCH_H_
#include "iotsa.h"
#include "iotsaApi.h"
#include "iotsaRequest.h"

typedef std::function<bool()> ActivationCallbackType;

class Input : public IotsaRequestContainer {
  friend class IotsaTouchMod;
public:
  Input(int _pin, bool _sendOnPress, bool _sendOnRelease, bool _wake) : pin(_pin), sendOnPress(_sendOnPress), sendOnRelease(_sendOnRelease), wakeOnPress(_wake), activationCallback(NULL) {}
  void setCallback(ActivationCallbackType callback) { activationCallback = callback; }
protected:
  int pin;
  uint16_t threshold = 20;
  bool sendOnPress;
  bool sendOnRelease;
  bool wakeOnPress;
  bool debounceState;
  unsigned int debounceTime;
  ActivationCallbackType activationCallback;
  bool buttonState;
  virtual void setupHandler() = 0;
  virtual void loopHandler() = 0;
};

class Touchpad : public Input {
public:
  using Input::Input;
protected:
  void setupHandler();
  void loopHandler();
};

class Button : public Input {
public:
  using Input::Input;
protected:
  void setupHandler();
  void loopHandler();
};

class IotsaTouchMod : public IotsaMod {
public:
  IotsaTouchMod(IotsaApplication& app, Input **_inputs, int _nInput) : IotsaMod(app), inputs(_inputs), nInput(_nInput) {}
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
