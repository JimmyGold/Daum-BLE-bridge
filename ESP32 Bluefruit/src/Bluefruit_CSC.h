#include "Adafruit_BluefruitLE_SPI.h"
#include "BluefruitConfig.h"


void error(const __FlashStringHelper*err);

void bluefruitSetupCSC(int32_t *cscServiceID, int32_t *cscMeasureCharID, int32_t *cscLocationCharID,
                    int32_t *cscFeatureCharID, int32_t *cscControlPointCharID);

void bluefruitSetupPWR(int32_t *pwrServiceID, int32_t *pwrMeasureCharID, int32_t *pwrLocationCharID, int32_t *pwrFeatureCharID,
                    int32_t *pwrControlPointCharID);                   

extern unsigned long msLastCall;
void sendSpeedAndCadence(uint16_t speed, uint16_t cadence, uint32_t cscMeasureCharID);
void sendPwr(uint16_t power, uint16_t speed, uint16_t cadence, uint32_t pwrMeasureCharID);