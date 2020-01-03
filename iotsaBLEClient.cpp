#include "iotsa.h"
#include "iotsaBLEClient.h"
#include "iotsaConfigFile.h"

void IotsaBLEClientMod::setup() {
  IFDEBUG IotsaSerial.println("BLEClientmod::setup()");
  BLEDevice::init(iotsaConfig.hostName.c_str());
  scanner = BLEDevice::getScan();
  scanner->setAdvertisedDeviceCallbacks(this);
  scanner->setActiveScan(true);
  scanner->start(20);
}

void IotsaBLEClientMod::serverSetup() {
}

void IotsaBLEClientMod::loop() {

}

void IotsaBLEClientMod::onResult(BLEAdvertisedDevice advertisedDevice) {
  IFDEBUG IotsaSerial.printf("BLEClientMod::onResult(%s)\n", advertisedDevice.toString().c_str());
}