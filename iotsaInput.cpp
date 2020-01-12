#include "iotsa.h"
#include "iotsaInput.h"
#include "iotsaConfigFile.h"

#define DEBOUNCE_DELAY 50 // 50 ms debouncing

static void dummyTouchCallback() {}
static bool anyWakeOnTouch;
static uint64_t bitmaskButtonWakeHigh;
static int buttonWakeLow;

void IotsaInputMod::setup() {
  anyWakeOnTouch = false;
  bitmaskButtonWakeHigh = 0;
  buttonWakeLow = -1;
  for(int i=0; i<nInput; i++) {
    inputs[i]->setup();
  }
  esp_err_t err;
  if (anyWakeOnTouch) {
    IFDEBUG IotsaSerial.println("IotsaInputMod: enable wake on touch");
    err = esp_sleep_enable_touchpad_wakeup();
    if (err != ESP_OK) IotsaSerial.println("Error in touchpad_wakeup");
  }
  if (bitmaskButtonWakeHigh) {
    err = esp_sleep_enable_ext1_wakeup(bitmaskButtonWakeHigh, ESP_EXT1_WAKEUP_ANY_HIGH);
    if (err != ESP_OK) IotsaSerial.println("Error in ext1_wakeup");
  }
  if (buttonWakeLow > 0) {
    err = esp_sleep_enable_ext0_wakeup((gpio_num_t)buttonWakeLow, 0);
    if (err != ESP_OK) IotsaSerial.println("Error in ext0_wakeup");
  }
}

void IotsaInputMod::serverSetup() {
}

void IotsaInputMod::loop() {

  for (int i=0; i<nInput; i++) {
    inputs[i]->loop();
  }
}


Input::Input(bool _actOnPress, bool _actOnRelease, bool _wake)
: actOnPress(_actOnPress), 
  actOnRelease(_actOnRelease), 
  wake(_wake), 
  activationCallback(NULL)
{
}

void Input::setCallback(ActivationCallbackType callback)
{
  activationCallback = callback;
}

Touchpad::Touchpad(int _pin, bool _actOnPress, bool _actOnRelease, bool _wake)
: Input(_actOnPress, _actOnRelease, _wake),
  pin(_pin)
{
}

void Touchpad::setup() {
  if (wake) {
    anyWakeOnTouch = true;
    touchAttachInterrupt(pin, dummyTouchCallback, threshold);
  }
}

void Touchpad::loop() {
  uint16_t value = touchRead(pin);
  if (value == 0) return;
  bool state = value < threshold;
  if (state != debounceState) {
    // The touchpad seems to have changed state. But we want
    // it to remain in the new state for some time (to cater for 50Hz/60Hz interference)
    debounceTime = millis();
    iotsaConfig.postponeSleep(DEBOUNCE_DELAY*2);
  }
  debounceState = state;
  if (millis() > debounceTime + DEBOUNCE_DELAY && state != buttonState) {
    // The touchpad has been in the new state for long enough for us to trust it.
    buttonState = state;
    bool doSend = (buttonState && actOnPress) || (!buttonState && actOnRelease);
    IFDEBUG IotsaSerial.printf("Touch callback for button pin %d state %d value %d\n", pin, state, value);
    if (doSend && activationCallback) {
      activationCallback();
    }
  }
  
}

Button::Button(int _pin, bool _actOnPress, bool _actOnRelease, bool _wake)
: Input(_actOnPress, _actOnRelease, _wake),
  pin(_pin)
{
}

void Button::setup() {
  pinMode(pin, INPUT_PULLUP);
  if (wake) {
    // Buttons should be wired to GND. So press gives a low level.
    if (actOnPress) {
      if (buttonWakeLow > 0) IotsaSerial.println("Multiple low wake inputs");
      buttonWakeLow = pin;
    } else {
      bitmaskButtonWakeHigh |= 1LL << pin;
    }
  }

}

void Button::loop() {
  bool state = digitalRead(pin) == LOW;
  if (state != debounceState) {
    // The touchpad seems to have changed state. But we want
    // it to remain in the new state for some time (to cater for 50Hz/60Hz interference)
    debounceTime = millis();
    iotsaConfig.postponeSleep(DEBOUNCE_DELAY*2);
  }
  debounceState = state;
  if (millis() > debounceTime + DEBOUNCE_DELAY && state != buttonState) {
    // The touchpad has been in the new state for long enough for us to trust it.
    buttonState = state;
    bool doSend = (buttonState && actOnPress) || (!buttonState && actOnRelease);
    IFDEBUG IotsaSerial.printf("Button callback for button pin %d state %d\n", pin, state);
    if (doSend && activationCallback) {
      activationCallback();
    }
  }
}
