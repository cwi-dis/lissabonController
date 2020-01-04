#include "iotsa.h"
#include "iotsaBLEClient.h"
#include "iotsaConfigFile.h"

typedef const char * UUIDstring;
BLEUUID wantedServiceUUID("153C0001-D28E-40B8-84EB-7F64B56D4E2E");

void IotsaBLEClientMod::setup() {
  IFDEBUG IotsaSerial.println("BLEClientmod::setup()");
  BLEDevice::init(iotsaConfig.hostName.c_str());
  scanner = BLEDevice::getScan();
  scanner->setAdvertisedDeviceCallbacks(this);
  scanner->setActiveScan(true);
  scanner->setInterval(155);
  scanner->setWindow(151);
  startScanning();
}

void IotsaBLEClientMod::startScanning() {
  scanner->start(20, &IotsaBLEClientMod::scanComplete, false);
  iotsaConfig.pauseSleep();
}

void IotsaBLEClientMod::stopScanning() {
  scanner->stop();
  iotsaConfig.resumeSleep();
}

void IotsaBLEClientMod::scanComplete(BLEScanResults results) {
    IFDEBUG IotsaSerial.println("BLE scan complete");
    iotsaConfig.resumeSleep();
}

void IotsaBLEClientMod::serverSetup() {
}

void IotsaBLEClientMod::loop() {

}

void IotsaBLEClientMod::onResult(BLEAdvertisedDevice advertisedDevice) {
  IFDEBUG IotsaSerial.printf("BLEClientMod::onResult(%s)\n", advertisedDevice.toString().c_str());
  if (advertisedDevice.isAdvertisingService(wantedServiceUUID)) {
    IFDEBUG IotsaSerial.println("This is the one we want");
  }
}