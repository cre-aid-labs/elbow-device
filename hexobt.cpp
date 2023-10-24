
#include "hexobt.h"
#include <arduino.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


HexoBT::HexoBT() {
  pServer = NULL;
  pService = NULL;
  pTxCharacteristic = NULL;
  pRxCharacteristic = NULL;
  uuids = NULL;
  device_connected = false;
  prev_device_connected = false;
}

BLEUUIDs::BLEUUIDs(char* tx_uuid, char* rx_uuid, char* srv_uuid) {
  this -> tx_uuid = tx_uuid;
  this -> rx_uuid = rx_uuid;
  this -> srv_uuid = srv_uuid;
}


void HexoBT::init(std::string devname, HexoBTServerCallbacks* srvclbks, HexoBTCharacteristicCallbacks* chrclbks, BLEUUIDs* uid) {
  BLEDevice::init(devname);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(srvclbks);
  this->uuids = uid;
  pService = pServer -> createService(uuids->srv_uuid);
  pTxCharacteristic = pService -> createCharacteristic(uuids->tx_uuid, BLECharacteristic::PROPERTY_NOTIFY);
  pTxCharacteristic->addDescriptor(new BLE2902());
  pRxCharacteristic = pService->createCharacteristic(uuids->rx_uuid, BLECharacteristic::PROPERTY_WRITE);
  pRxCharacteristic->setCallbacks(chrclbks);

  pService->start();

  pServer -> getAdvertising() -> start();
}

void HexoBT::restartAdvertising() {
  vTaskDelay(500/portTICK_PERIOD_MS);
  pServer -> startAdvertising();
}
