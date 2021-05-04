#include "Bluefruit_CSC.h"

// Create the bluefruit object, either software serial...uncomment these lines
/*
SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);

Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);
*/

/* ...or hardware serial, which does not need the RTS/CTS pins. Uncomment this line */
// Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);



/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

const float WHEEL_CICUMFERENCE = 2.096; // standard setting from Garmin Edge in meter

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}


// ############# Setup Bluefruit LE modul as CyclingSpeedAndCadence-Service
boolean success;
void bluefruitSetupCSC(int32_t *cscServiceID, int32_t *cscMeasureCharID, int32_t *cscLocationCharID,
                    int32_t *cscFeatureCharID, int32_t *cscControlPointCharID){
  Serial.print(F("Initialising the Bluefruit LE module: "));
  if (!ble.begin(VERBOSE_MODE))
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println(F("OK!"));

  /* Perform a factory reset to make sure everything is in a known state */
  Serial.println(F("Performing a factory reset: "));
  if (!ble.factoryReset())
  {
    error(F("Couldn't factory reset"));
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  // this line is particularly required for Flora, but is a good idea
  // anyways for the super long lines ahead!
  // ble.setInterCharWriteDelay(5); // 5 ms

  /* Change the device name to make it easier to find */
  Serial.println(F("Setting device name to 'Daum 8008 TRS3 CSC': "));

  if (!ble.sendCommandCheckOK(F("AT+GAPDEVNAME=Daum 8008 TRS3 CSC")))
  {
    error(F("Could not set device name?"));
  }

  // Speed & Cadence
  Serial.println(F("Adding the Speed and Cadence Service definition (UUID = 0x1816): ")); // Add Service 0x1816 (Speed & Cadence)
  success = ble.sendCommandWithIntReply(F("AT+GATTADDSERVICE=UUID=0x1816"), cscServiceID);
  if (!success)
  {
    error(F("Could not add CSC service"));
  }

  Serial.println(F("Adding the CSC measurement characteristic (UUID = 0x2A5B): ")); // Add characteristic 0x2A5B
  success = ble.sendCommandWithIntReply(F("AT+GATTADDCHAR=UUID=0x2A5B, PROPERTIES=0x10, MIN_LEN=11, MAX_LEN=11, VALUE=0"), cscMeasureCharID);
  if (!success)
  {
    error(F("Could not add measurement characteristic"));
  }

  Serial.println(F("Adding the Sensor Location characteristic (UUID = 0x2A5D): "));                                               // Add characteristic 0x2A5D
  success = ble.sendCommandWithIntReply(F("AT+GATTADDCHAR=UUID=0x2A5D, PROPERTIES=0x02, MIN_LEN=1, VALUE=6"), cscLocationCharID); // OK
  if (!success)
  {
    error(F("Could not add Sensor Location characteristic"));
  }

  Serial.println(F("Adding the Feature characteristic (UUID = 0x2A5C): "));                                                         // Add characteristic 0x2A5C
  success = ble.sendCommandWithIntReply(F("AT+GATTADDCHAR=UUID=0x2A5C, PROPERTIES=0x02, MIN_LEN=2, VALUE=0x07"), cscFeatureCharID); // OK
  if (!success)
  {
    error(F("Could not add Feature characteristic"));
  }

  Serial.println(F("Adding the Control Point characteristic (UUID = 0x2A55): ")); // Add characteristic 0x2A5C
  success = ble.sendCommandWithIntReply(F("AT+GATTADDCHAR=UUID=0x2A55, PROPERTIES=0x28, MIN_LEN=1, VALUE=0"), cscControlPointCharID);
  if (!success)
  {
    error(F("Could not add Control Point characteristic"));
  }
  Serial.print(F("Adding CSC Service UUID to the advertising payload: "));
  ble.sendCommandCheckOK(F("AT+GAPSETADVDATA=02-01-06-03-02-16-18")); // Advertising 0x1816
  //------------------------------

  /* Reset the device for the new service setting changes to take effect */
  Serial.print(F("Performing a SW reset (service changes require a reset): "));
  ble.reset();
  Serial.println();
}

// ############# Setup as Power, Speed and Cadence Sensor
void bluefruitSetupPWR(int32_t *pwrServiceID, int32_t *pwrMeasureCharID, int32_t *pwrLocationCharID, int32_t *pwrFeatureCharID,
                    int32_t *pwrControlPointCharID){
  Serial.print(F("Initialising the Bluefruit LE module: "));
  if (!ble.begin(VERBOSE_MODE))
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println(F("OK!"));

  /* Perform a factory reset to make sure everything is in a known state */
  Serial.println(F("Performing a factory reset: "));
  if (!ble.factoryReset())
  {
    error(F("Couldn't factory reset"));
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  // this line is particularly required for Flora, but is a good idea
  // anyways for the super long lines ahead!
  // ble.setInterCharWriteDelay(5); // 5 ms

  /* Change the device name to make it easier to find */
  Serial.println(F("Setting device name to 'Daum 8008 TRS3 Power': "));

  if (!ble.sendCommandCheckOK(F("AT+GAPDEVNAME=Daum 8008 TRS3 Power")))
  {
    error(F("Could not set device name?"));
  }
  
  Serial.println(F("Adding the Power Service definition (UUID = 0x1818): ")); 
  success = ble.sendCommandWithIntReply( F("AT+GATTADDSERVICE=UUID=0x1818"), pwrServiceID);
  if (! success) {
    error(F("Could not add Power service"));
  }

  Serial.println(F("Adding the Power Measurement characteristic (UUID = 0x2A63): ")); 
  success = ble.sendCommandWithIntReply( F("AT+GATTADDCHAR=UUID=0x2A63, PROPERTIES=0x10, MIN_LEN=14, MAX_LEN=14, VALUE=0"), pwrMeasureCharID);
    if (! success) {
    error(F("Could not add Power Measurement characteristic"));
  }

  Serial.println(F("Adding the Sensor Location characteristic (UUID = 0x2A5D): ")); 
  success = ble.sendCommandWithIntReply( F("AT+GATTADDCHAR=UUID=0x2A5D, PROPERTIES=0x02, MIN_LEN=1, VALUE=6"), pwrLocationCharID);
    if (! success) {
    error(F("Could not add Sensor Location characteristic"));
  }

  Serial.println(F("Adding the Feature characteristic (UUID = 0x2A65): "));  
  success = ble.sendCommandWithIntReply( F("AT+GATTADDCHAR=UUID=0x2A65, PROPERTIES=0x02, MIN_LEN=4, VALUE=0x0000000C"), pwrFeatureCharID);
    if (! success) {
    error(F("Could not add Feature characteristic"));
  }

  Serial.println(F("Adding the Control Point characteristic (UUID = 0x2A66): ")); 
  success = ble.sendCommandWithIntReply( F("AT+GATTADDCHAR=UUID=0x2A66, PROPERTIES=0x28, MIN_LEN=1, VALUE=0"), pwrControlPointCharID);
    if (! success) {
    error(F("Could not add Control Point characteristic"));
  }
  Serial.print(F("Adding CP Service UUID to the advertising payload: "));
  ble.sendCommandCheckOK( F("AT+GAPSETADVDATA=02-01-06-03-02-18-18")); // Advertising 0x180D, 0x1818

  /* Reset the device for the new service setting changes to take effect */
  Serial.print(F("Performing a SW reset (service changes require a reset): "));
  ble.reset();
  Serial.println();
}



// ############# Calculation of Speed and Cadence Characteristic and output it over BLE
extern unsigned long msLastCall;
void sendSpeedAndCadence(uint16_t speed, uint16_t cadence, uint32_t cscMeasureCharID){
  static uint16_t crankRev = 0;
  static uint16_t crankRevOld = 0;
  static uint16_t crankTS = 0; 
  static uint16_t crankTSold;
  static uint32_t wheelRev =0;
  static uint32_t wheelRevOld = 0;
  static double wheelRevDouble;
  static uint16_t wheelTS = 0;
  static uint16_t wheelTSold;
  static uint8_t cscData[11];
  static double crankRevDouble=0;
  unsigned long TSdifference;

  TSdifference = millis()- msLastCall; // time difference between last call
  msLastCall = millis();

  // Speed
  wheelRevDouble = wheelRevDouble + ((speed*TSdifference)/(WHEEL_CICUMFERENCE*3.6))/1000; // exact Wheel Revolution in this period
  wheelRev = wheelRevDouble; // int value of Wheel Revolution
  if (wheelRev != wheelRevOld){
    wheelTS = wheelTSold + ((wheelRev - wheelRevOld)*WHEEL_CICUMFERENCE*1024*3.6/(speed)); // calculation of timestamp for the int value
    wheelRevOld = wheelRev;
    wheelTSold = wheelTS;
  }
  // Cadence
  crankRevDouble = crankRevDouble + (cadence * TSdifference)/(60*1024);
  crankRev = crankRevDouble;
  if (crankRev != crankRevOld){
    crankTS = crankTSold + ((crankRev - crankRevOld)*60*1024/cadence);
    crankRevOld = crankRev;
    crankTSold = crankTS;
  }
  
  // fill data to the array
  cscData[0]= 0x03; // 0x01 = wheel only, 0x02 = crank only, 0x03 wheel & crank
  cscData[1]= wheelRev & 0xFF;
  cscData[2]= wheelRev >> 8 & 0xFF;
  cscData[3]= wheelRev >> 16 & 0xFF;
  cscData[4]= wheelRev >> 24 & 0xFF;
  cscData[5]= wheelTS & 0xFF;
  cscData[6]= wheelTS >> 8 & 0xFF;
  cscData[7]= crankRev & 0xFF;
  cscData[8]= crankRev >> 8 & 0xFF;
  cscData[9]= crankTS & 0xFF;
  cscData[10]= crankTS >> 8 & 0xFF;

  char sBuffer[47];
  char sTemp[20];

  /* Command is sent when \n (\r) or println is called */
  /* AT+GATTCHAR=CharacteristicID,value */
  strcpy(sBuffer,"AT+GATTCHAR=");
  sprintf(sTemp,"%d",(int)cscMeasureCharID);  
  strcat(sBuffer,sTemp);
  strcat(sBuffer,",");
  sprintf(sTemp,"%02X-",cscData[0]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X-",cscData[1]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X-",cscData[2]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X-",cscData[3]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X-",cscData[4]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X-",cscData[5]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X-",cscData[6]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X-",cscData[7]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X-",cscData[8]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X-",cscData[9]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X",cscData[10]);  
  strcat(sBuffer,sTemp);
  ble.println(sBuffer);

  /* Check if command executed OK */
  if ( !ble.waitForOK() )
  {
    Serial.println(F("Failed to get response!"));
  }
}

// ############# Calculation of Power, Speed and Cadence Characteristic and output it over BLE
void sendPwr(uint16_t power, uint16_t speed, uint16_t cadence, uint32_t pwrMeasureCharID){
  static uint16_t crankRev = 0;
  static uint16_t crankRevOld = 0;
  static uint16_t crankTS = 0; 
  static uint16_t crankTSold;
  static uint32_t wheelRev =0;
  static uint32_t wheelRevOld = 0;
  static double wheelRevDouble;
  static uint16_t wheelTS = 0;
  static uint16_t wheelTSold;
  static double crankRevDouble=0;
  unsigned long TSdifference;
  static uint8_t pwrData[34];

  TSdifference = millis()- msLastCall;
  msLastCall = millis();
 
  // Speed
  wheelRevDouble = wheelRevDouble + ((speed*TSdifference)/(WHEEL_CICUMFERENCE*3.6))/1000; // exact Wheel Revolution in this period
  wheelRev = wheelRevDouble; // int value of Wheel Revolution
  if (wheelRev != wheelRevOld){
    wheelTS = wheelTSold + ((wheelRev - wheelRevOld)*WHEEL_CICUMFERENCE*2048*3.6/(speed)); // calculation of timestamp for the int value
    wheelRevOld = wheelRev;
    wheelTSold = wheelTS;
  }
  // Cadence
  crankRevDouble = crankRevDouble + (cadence * TSdifference)/(60*1024);
  crankRev = crankRevDouble;
  if (crankRev != crankRevOld){
    crankTS = crankTSold + ((crankRev - crankRevOld)*60*1024/cadence);
    crankRevOld = crankRev;
    crankTSold = crankTS;
  }

  char sBuffer[60];
  char sTemp[20];

  // Power
  pwrData[0]= 0x30; 
  pwrData[1]= 0x00;
  pwrData[2]= power & 0xFF;
  pwrData[3]= power >> 8 & 0xFF;
  pwrData[4]= wheelRev & 0xFF;
  pwrData[5]= wheelRev >> 8 & 0xFF;
  pwrData[6]= wheelRev >> 16 & 0xFF;
  pwrData[7]= wheelRev >> 24 & 0xFF;
  pwrData[8]= wheelTS & 0xFF;
  pwrData[9]= wheelTS >> 8 & 0xFF;
  pwrData[10]= crankRev & 0xFF;
  pwrData[11]= crankRev >> 8 & 0xFF;
  pwrData[12]= crankTS & 0xFF;
  pwrData[13]= crankTS >> 8 & 0xFF;


  strcpy(sBuffer,"AT+GATTCHAR=");
  sprintf(sTemp,"%d",(int)pwrMeasureCharID);  
  strcat(sBuffer,sTemp);
  strcat(sBuffer,",");
  sprintf(sTemp,"%02X-",pwrData[0]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X-",pwrData[1]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X-",pwrData[2]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X-",pwrData[3]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X-",pwrData[4]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X-",pwrData[5]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X-",pwrData[6]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X-",pwrData[7]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X-",pwrData[8]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X-",pwrData[9]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X-",pwrData[10]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X-",pwrData[11]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X-",pwrData[12]);  
  strcat(sBuffer,sTemp);
  sprintf(sTemp,"%02X",pwrData[13]);  
  strcat(sBuffer,sTemp);

  ble.println(sBuffer);

  /* Check if command executed OK */
  if ( !ble.waitForOK() )
  {
    Serial.println(F("Failed to get response!"));
  }
}