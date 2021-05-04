#include "Daum_RS232.h"

// ############# REQUEST SERIAL DATA (RUN) ################
void DaumRequestData(void){  
    Serial2.write(0x40); // get Data
    Serial2.write(0x00); // Address
    Serial2.write(0x0A); // LF
    Serial.println("Send Data");
}

// ############# RECEIVE SERIAL DATA (RUN) ################
void DaumReceiveData(bool *connected, uint8_t *program, uint8_t *person, bool *pedaling, uint16_t *powerPreset, 
                        uint8_t *speed, uint8_t *cadence, float *distance, uint16_t *time, float *kJoule, uint8_t *pulse, uint8_t *pulseStatus,
                        uint8_t *gear,  uint16_t *power){
    uint8_t Data[20];
    uint8_t i;
    i=0;
    while (Serial2.available()) { // cache serial data to array
      Data[i] = Serial2.read();
      i++;
    //  Serial.println("Data");
    }
    if (Data[0] == 0x40 && Data[1]==0x00) { // Daum answers with the request data and address at the beginning
      *connected = true;
      *program = Data[2]; // number of choosen program
      *person = Data[3];  // number of choosen person
      if (Data[4] == 0x01) {
        *pedaling = true; // crank is pedaling
      }
      else{
        *pedaling = false;
      }
      *powerPreset = Data[5]*5; // power preset data in steps of 5W
      *cadence = Data[6]; // cadence data in rpm
      *speed = Data[7];   // speed data in km/h in steps of 1km/h
      *distance = ((float)word(Data[9],Data[8]))/10.0; // distance data in steps of 100m
      *time = word(Data[11],Data[10]);  // time in s
      *kJoule = ((float)word(Data[13],Data[12]))/10.0; // energy in steps of 100 Joules
      *pulse = Data[14];  // pulse in steps of 1 per minute
      *pulseStatus = Data[15]; // status of pulse (over, under, in range)
      *gear = Data[16]; // selected gear (only in special programs)
      if ((Data[4] == 0x01) && *cadence > 45) {
        power = powerPreset;  // output power (if pedaling && cadence is over 45 rpm)
      }
      else {
        power = 0;
      }
      Serial.println("Connected");
    }
    else{ // if not connected or no run data
      *connected = false;
      *program = 0;
      *person = 0;
      *pedaling = false;
      *powerPreset = 0;
      *cadence = 0;
      *speed = 0;
      *distance = 0;
      *time = 0;
      *kJoule = 0;
      *pulse = 0;
      *pulseStatus = 0;
      *gear = 0;
    } 
  }  
