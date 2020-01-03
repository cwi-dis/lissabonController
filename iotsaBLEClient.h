#ifndef _IOTSABLECLIENT_H_
#define _IOTSABLECLIENT_H_
#include "iotsa.h"
#include "iotsaApi.h"
#include <BLEDevice.h>

class IotsaBLEOtherDevice {
  friend class IotsaBLEClientMod;
public:
};

class IotsaBLEClientMod : public IotsaMod, public BLEAdvertisedDeviceCallbacks {
public:
  IotsaBLEClientMod(IotsaApplication& app) : IotsaMod(app) {}
  using IotsaMod::IotsaMod;
  void setup();
  void serverSetup();
  void loop();
  String info() { return ""; }
protected:
  void onResult(BLEAdvertisedDevice advertisedDevice);
  BLEScan *scanner = NULL;
};

#endif
