#include "iotsa.h"
#include "iotsaBLEClient.h"
#include "iotsaConfigFile.h"


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

void IotsaBLEClientMod::setDeviceFoundCallback(BleDeviceFoundCallback _callback) {
  callback = _callback;
}

void IotsaBLEClientMod::setServiceFilter(const BLEUUID& serviceUUID) {
  if (serviceFilter) delete serviceFilter;
  serviceFilter = new BLEUUID(serviceUUID);
}

void IotsaBLEClientMod::setManufacturerFilter(uint16_t manufacturerID) {
  manufacturerFilter = manufacturerID;
  hasManufacturerFilter = true;
}

void IotsaBLEClientMod::loop() {

}

void IotsaBLEClientMod::onResult(BLEAdvertisedDevice advertisedDevice) {
  IFDEBUG IotsaSerial.printf("BLEClientMod::onResult(%s)\n", advertisedDevice.toString().c_str());
  // Do we want callbacks?
  if (callback == NULL) return;
  // Do we filter on services?
  if (serviceFilter != NULL) {
    if (!advertisedDevice.isAdvertisingService(*serviceFilter)) return;
  }
  // Do we filter on manufacturer data?
  if (hasManufacturerFilter) {
    std::string mfgData(advertisedDevice.getManufacturerData());
    if (mfgData.length() < 2) return;
    const uint16_t *mfg = (const uint16_t *)mfgData.c_str();
    if (*mfg != manufacturerFilter) return;
  }
  callback(advertisedDevice);
}