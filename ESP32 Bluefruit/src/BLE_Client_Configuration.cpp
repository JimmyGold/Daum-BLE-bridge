#include "BLE_Client_Configuration.h"

#define BLUEFRUIT_BLE_ADDRESS "DC:8E:CD:9D:4B:47"   // Address of the Bluefruit LE module - to skip it as "real" power meter

extern boolean HR_AVAILABLE;
extern boolean PWR_AVAILABLE;

//###################################################
//Device (CLIENT)
// Heart Rate
static BLEUUID service_HR_UUID(BLEUUID((uint16_t)0x180D));
static BLEUUID char_HR_UUID(BLEUUID((uint16_t)0x2A37));
// Power
static BLEUUID service_PWR_UUID(BLEUUID((uint16_t)0x1818));
// Battery
static BLEUUID service_BATT_UUID(BLEUUID((uint16_t)0x180F));
static BLEUUID char_BATT_UUID(BLEUUID((uint16_t)0x2A19));

static boolean doConnect = false;
static boolean connected;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemote_HR_Characteristic;
static BLERemoteCharacteristic* pRemote_BATT_Characteristic;
static BLEAdvertisedDevice* myDevice;
static BLEAddress* HR_Address;
String AddressHrSensor = "00:00:00:00:00:00"; 
bool notificationOn;
extern bool newBLEmessage;

extern uint8_t HR_val;

static void notifyCallback( // Client
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    if (pData[1] > 10 && pData[1] < 254) {  // HR must be between 10 and 254 bpm
      HR_val = pData[1];
    } else HR_val =0;
    newBLEmessage = true;
}

void ESPHardRestart()
{
  esp_task_wdt_init(1, true);
  esp_task_wdt_add(NULL);
  while (true)
    ;
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
    ESPHardRestart(); // Restart ESP if HR-Sensor disconnects to initiate a new connection
                      // ESP hangs in a loop - there should be a better solution
  }
};

bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemote_HR_Service = pClient->getService(service_HR_UUID);
    if (pRemote_HR_Service == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(service_HR_UUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");

    // Obtain a reference to the BATT service of the remote BLE server.
    BLERemoteService* pRemote_BATT_Service = pClient->getService(service_BATT_UUID);

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemote_HR_Characteristic = pRemote_HR_Service->getCharacteristic(char_HR_UUID);
    if (pRemote_HR_Characteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(char_HR_UUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    // Obtain a reference to the BATT characteristic in the service of the remote BLE server.
    pRemote_BATT_Characteristic = pRemote_BATT_Service->getCharacteristic(char_BATT_UUID);

    if(pRemote_HR_Characteristic->canNotify())
      // Set notification
      pRemote_HR_Characteristic->registerForNotify(notifyCallback);
      connected = true;
      return connected;
}

/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    // We have found a device, let us now see if it contains the service we are looking for.
    // Heart Rate  
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(service_HR_UUID)) { 
      Serial.print("BLE Advertised Device found: ");
      Serial.println(advertisedDevice.toString().c_str());
      
      if (advertisedDevice.getAddress().equals(*HR_Address)){ // if HR-Sensor is already avialable, then skip it (for Sensor with more than one BT-Channels)
        Serial.println("HR Sensor is already available");
        return;
      } 
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      // Save address of first HR-Sensor      
      *HR_Address = advertisedDevice.getAddress();
      
      doConnect = true;
      doScan = true;
    } 
    // Real Power Meter
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(service_PWR_UUID) && (advertisedDevice.getAddress() != BLEAddress(BLUEFRUIT_BLE_ADDRESS))){
      PWR_AVAILABLE = true;
      //Serial.println("Powermeter found");                   // Debug
      //Serial.println(advertisedDevice.toString().c_str());  // Debug  
    }
  } // onResult
}; // MyAdvertisedDeviceCallbacks

/**
 * Callback invoked when scanning has completed.
 */
static void scanRun();
static void scanCompleteCB(BLEScanResults scanResults) {
	scanResults.dump();
  scanRun();
} 

static void scanRun(){
  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 10 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(10, scanCompleteCB);
}

void BLE_Setup(void){
  BLEDevice::init("Smart Bike"); 
  HR_Address = new BLEAddress(AddressHrSensor.c_str()); // initial address or HR-Sensor 00:00:00:00:00:00
  scanRun();
}

void BLE_Loop(){
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
    scanRun();
  }

  // If we are connected to the HR BLE Server
  if (connected) {
    HR_AVAILABLE = true;
  }
  else{
    HR_AVAILABLE = false;
  }
}
