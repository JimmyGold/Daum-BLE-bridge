#include <Arduino.h>

void DaumRequestData(void);

void DaumReceiveData(bool *connected, uint8_t *program, uint8_t *person, bool *pedaling, uint16_t *powerPreset, 
                        uint16_t *speed, uint8_t *cadence, float *distance, uint16_t *time, float *kJoule, uint8_t *pulse, uint8_t *pulseStatus,
                        uint8_t *gear,  uint16_t *power);
