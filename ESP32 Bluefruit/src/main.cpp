
/*********************************************************************
 This is an example for our nRF51822 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

/*
    Please note the long strings of data sent mean the *RTS* pin is
    required with UART to slow down data sent to the Bluefruit LE!
*/

#include <Arduino.h>
#include <SPI.h>

#include "BluefruitConfig.h"
#include "Bluefruit_CSC.h"

#include "BLEDevice.h" 
#include <rom/rtc.h>
#include <esp_int_wdt.h>
#include <esp_task_wdt.h>

#include "BLE_Client_Configuration.h"
#include "Daum_RS232.h"

#define RXD2 26   // Serial connection to Daum
#define TXD2 25   // Serial connection to Daum
#define pulseOutput 27  // to Pulse input (Ear Clip)
#define DFUpin 19


// ###### global variables ######
boolean HR_AVAILABLE = false; // HR-Sensor over BLE is available
boolean PWR_AVAILABLE = false; // Real Power - Sensor over BLE is available
static boolean pwrAvailableOld = false;
static boolean pwrDaumOn = false;
static boolean startup = true;
uint8_t i, rpm, program, person, pulse, pulseStatus, gear, speed, cadence;
uint16_t powerPreset, power = 0, timeSecound, pwrPower;
bool pedaling;
float distance, kJoule;
unsigned long lastDataRequest;
unsigned long lastPulse;
unsigned long msPulse;
unsigned long msLastCall;
unsigned long msNotify;
unsigned long msPwrAvailable;
bool DaumConnected = false;
bool newData;
bool stopReading;

boolean connected = false;
boolean newBLEmessage = false;
uint8_t HR_val = 0;

/* The service information */
// Speed and Cadence
int32_t cscServiceID;
int32_t cscMeasureCharID;
int32_t cscLocationCharID;
int32_t cscFeatureCharID;
int32_t cscControlPointCharID;
// Power
int32_t pwrServiceId;
int32_t pwrMeasureCharId;
int32_t pwrLocationCharId;
int32_t pwrFeatureCharId;
int32_t pwrControlPointCharId;


// ######## SETUP ###########
void setup(void){
  pinMode(DFUpin,OUTPUT);
  digitalWrite(DFUpin, HIGH);
  pinMode(pulseOutput, OUTPUT_OPEN_DRAIN);  // Simulation of ear-clip sensor - set as open drain

  while (!Serial); // required for Flora & Micro
  delay(500);
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N2, RXD2, TXD2); // SERIAL_8N1 (8 Bits, No Paraty, 1 Stop Bit) SERIAL_8N2 (8 Bits, No Paraty, 2 Stop Bit)
 
  /* Initialise Bluefruit LE module */
  BLE_Setup(); // ESP32 internal BLE device (Client - Heart Rate)
}

// ######## LOOP ###########
void loop(void){
  BLE_Loop(); // ESP32 internal BLE Client (Heart Rate & PM search)
  
  // ############# REQUEST SERIAL DATA ################
  if (millis() - lastDataRequest > 1000){ // every 1000ms
    lastDataRequest = millis();
    DaumRequestData();
    newData = false;
    stopReading = false;
    if (HR_AVAILABLE && newBLEmessage) {
      Serial.println(HR_val); // print HR form BLE Sensor to terminal
      newBLEmessage = false;
    }
  }

  // ############# READ SERIAL DATA & WRITE BLE DATA ################
  if (PWR_AVAILABLE){
    msPwrAvailable = millis();
  }
  if (millis() > msPwrAvailable + 30000){ // if Power Meter is longer than 30s unavailable then switch to Daum Power output
    PWR_AVAILABLE = false;
  }
  if (PWR_AVAILABLE && !pwrAvailableOld && !startup){
    pwrAvailableOld = true;
    Serial.println("Setup Bluefruit as Speed & Cadence Sensor");
    bluefruitSetupCSC(&cscServiceID, &cscMeasureCharID, &cscLocationCharID, &cscFeatureCharID, &cscControlPointCharID);
    pwrDaumOn = false;
  }
  if (!PWR_AVAILABLE && (pwrAvailableOld || startup)){
    pwrAvailableOld = false;
    startup = false;
    Serial.println("Setup Bluefruit as Power Sensor");
    bluefruitSetupPWR(&pwrServiceId, &pwrMeasureCharId, &pwrLocationCharId, &pwrFeatureCharId, &pwrControlPointCharId);
    pwrDaumOn = true;
  }
  // Read serial data after request
  if ((millis() - (lastDataRequest) > 850) &! stopReading){ // read data 950ms after data request
    stopReading = true;
    DaumReceiveData( (boolean *)&DaumConnected, &program, &person, &pedaling, &powerPreset, &speed, 
              &cadence, &distance, &timeSecound, &kJoule, &pulse, &pulseStatus, &gear, &power);
    if (DaumConnected){
      newData = true;
    }          
    if (pwrDaumOn){
      pwrPower = power;
      sendPwr(pwrPower, speed, cadence, pwrMeasureCharId); 
    //  Serial.println("Power Mode");
    }
    else{
      sendSpeedAndCadence(speed, cadence, cscMeasureCharID); 
    //  Serial.println("CSC Mode");
    }
  }

  // ############# WRITE DATA FROM DAUM TO TERMINAL ################
  if (newData){
    newData = false;
    Serial.print("Leistung: \t");
    Serial.println(powerPreset);
    Serial.print("Kadenz: \t");
    Serial.println(cadence);
    Serial.print("Strecke: \t");
    Serial.println(distance);
    Serial.print("Geschwindigkeit: \t");
    Serial.println(speed);
    Serial.print("Puls: \t");
    Serial.println(pulse);
    Serial.print("Zeit: \t");
    Serial.println(timeSecound);
    Serial.print("Joule: \t");
    Serial.println(kJoule);
  }

    // ############# PULSE SIGNAL ################
  if (HR_AVAILABLE){
    msPulse = ulong((60*1000.0)/float(HR_val));
    if (millis() - lastPulse >= msPulse){  // Turn on pulse signal (inverted because of open drain)
      lastPulse = millis();
      digitalWrite(pulseOutput, LOW);
    }
    if (millis() - lastPulse > 100 ){ // Turn off pulse signal (inverted because of open drain)
      digitalWrite(pulseOutput, HIGH);
    }
  } 
}
