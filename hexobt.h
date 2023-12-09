#ifndef HEXOBT_H
#define HEXOBT_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

class BLEUUIDs {
  public:
  BLEUUIDs(char* tx_uuid, char* rx_uuid, char* srv_uuid);
  char* tx_uuid;
  char* rx_uuid;
  char* srv_uuid;
};

class HexoBTServerCallbacks : public BLEServerCallbacks {
  virtual void onConnect(BLEServer* pServer) = 0;
  virtual void onDisconnect(BLEServer* pServer) = 0;
};

class HexoBTCharacteristicCallbacks : public BLECharacteristicCallbacks {
  virtual void onWrite(BLECharacteristic* pTxCharacteristic) = 0;
};

class HexoBT {
  BLEServer* pServer;
  BLEService* pService;
  BLECharacteristic* pTxCharacteristic;
  BLECharacteristic* pRxCharacteristic;
  BLEUUIDs* uuids;
  public:
  bool device_connected;
  bool prev_device_connected;
  HexoBT();
  void init(std::string devname, HexoBTServerCallbacks* srvclbks, HexoBTCharacteristicCallbacks* chrclbks, BLEUUIDs* uid);
  void write(std::string data);
  bool is_device_connected();
  bool is_prev_device_connected();
  void restartAdvertising();
};

#endif
