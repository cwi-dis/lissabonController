#ifndef _IOTSABLECLIENT_H_
#define _IOTSABLECLIENT_H_
#include "iotsa.h"
#include "iotsaApi.h"
#include <BLEDevice.h>

typedef std::function<void(BLEAdvertisedDevice&)> BleDeviceFoundCallback;

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
  void setDeviceFoundCallback(BleDeviceFoundCallback _callback);
  void setServiceFilter(const BLEUUID& serviceUUID);
  void setManufacturerFilter(uint16_t manufacturerID);
protected:
  void onResult(BLEAdvertisedDevice advertisedDevice);
  void startScanning();
  void stopScanning();
  static void scanComplete(BLEScanResults results);
  BLEScan *scanner = NULL;
  BleDeviceFoundCallback callback = NULL;
  BLEUUID* serviceFilter = NULL;
  uint16_t manufacturerFilter;
  bool hasManufacturerFilter = false;
};

#endif
