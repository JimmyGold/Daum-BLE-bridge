#include <Arduino.h>
#include "BLEDevice.h" 


static void notifyCallback( // Client
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify);

bool connectToServer();

void BLE_Setup(void);

void BLE_Loop(void);