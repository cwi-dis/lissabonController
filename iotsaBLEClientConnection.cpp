#include "iotsaBLEClientConnection.h"

bool IotsaBLEClientConnection::available() {
  return device != NULL;
}

bool IotsaBLEClientConnection::connect() {
  if (device == NULL) return false;
  if (client != NULL) return true;
  client = BLEDevice::createClient();
  bool ok = client->connect(device);
  if (!ok) delete client;
  return ok;
}

void IotsaBLEClientConnection::disconnect() {
  if (client != NULL) {
    client->disconnect();
    delete client;
  }
}

bool IotsaBLEClientConnection::set(BLEUUID& serviceUUID, BLEUUID& charUUID, const uint8_t *data, size_t size) {
  if (client == NULL) return false;
  BLERemoteService *service = client->getService(serviceUUID);
  if (service == NULL) return false;
  BLERemoteCharacteristic *characteristic = service->getCharacteristic(charUUID);
  if (characteristic == NULL) return false;
  if (!characteristic->canWrite()) return false;
  characteristic->writeValue((uint8_t *)data, size);
  return true;
}

bool IotsaBLEClientConnection::set(BLEUUID& serviceUUID, BLEUUID& charUUID, uint8_t value) {
  return set(serviceUUID, charUUID, (const uint8_t *)&value, 1);
}

bool IotsaBLEClientConnection::set(BLEUUID& serviceUUID, BLEUUID& charUUID, uint16_t value) {
  return set(serviceUUID, charUUID, (const uint8_t *)&value, 2);
}

bool IotsaBLEClientConnection::set(BLEUUID& serviceUUID, BLEUUID& charUUID, uint32_t value) {
  return set(serviceUUID, charUUID, (const uint8_t *)&value, 4);
}

bool IotsaBLEClientConnection::set(BLEUUID& serviceUUID, BLEUUID& charUUID, const std::string& value) {
  return set(serviceUUID, charUUID, (const uint8_t *)value.c_str(), value.length());
}

bool IotsaBLEClientConnection::set(BLEUUID& serviceUUID, BLEUUID& charUUID, const String& value) {
  return set(serviceUUID, charUUID, (const uint8_t *)value.c_str(), value.length());
}

bool IotsaBLEClientConnection::getAsBuffer(BLEUUID& serviceUUID, BLEUUID& charUUID, uint8_t **datap, size_t *sizep) {
  if (client == NULL) return false;
  BLERemoteService *service = client->getService(serviceUUID);
  if (service == NULL) return false;
  BLERemoteCharacteristic *characteristic = service->getCharacteristic(charUUID);
  if (characteristic == NULL) return false;
  if (!characteristic->canRead()) return false;
  return false;
}

int IotsaBLEClientConnection::getAsInt(BLEUUID& serviceUUID, BLEUUID& charUUID) {
  size_t size;
  uint8_t *ptr;
  int val = 0;
  int shift = 0;
  if (!getAsBuffer(serviceUUID, charUUID, &ptr, &size)) return 0;
  while (size--) {
    val = val | (*ptr++ << shift);
    shift += 8;
  }
  return val;
}

std::string IotsaBLEClientConnection:: getAsString(BLEUUID& serviceUUID, BLEUUID& charUUID) {
  size_t size;
  uint8_t *ptr;
  if (!getAsBuffer(serviceUUID, charUUID, &ptr, &size)) return "";
  return std::string((const char *)ptr, size);
}

bool IotsaBLEClientConnection::getAsNotification(BLEUUID& serviceUUID, BLEUUID& charUUID, BleNotificationCallback callback) {
  assert(0);
  return false;
}

