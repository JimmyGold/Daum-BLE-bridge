#include "Power2Speed.h"
// Calculates the speed from a given power. 
// It searches for the right base point and interpolates the value between that and the next base point.

uint16_t power2speed(uint16_t power){
    static uint16_t data[29][2] = { 0,0,        // array of power and coresponding speed values
                                    10,84,      // calculated from www.kreuzotter.de; speed values in 1/10 km/h
                                    20,124,     // settings: height = 1,89m
                                    30,152,     //          race bike
                                    40,173,     //          flat street
                                    50,191,     //          drops
                                    60,207,     //          Cd * A = 0,1783
                                    70,220,     //          Cr = 0,00382
                                    80,232,     //          cadence = 90 rpm
                                    90,244,
                                    100,254,
                                    110,264,
                                    120,273,
                                    130,281,
                                    140,289,
                                    150,297,
                                    200,331,
                                    250,360,
                                    300,384,
                                    350,406,
                                    400,426,
                                    450,445,
                                    500,462,
                                    550,477,
                                    600,492,
                                    650,506,
                                    700,520,
                                    750,532,
                                    800,545};

    uint8_t i;   
    float d, k, speed;     

    // limits the input power to the given range
    if (power <= 5){ // minimum power of Daum is 25W
        return 0;
    }
    else if (power >= 800){ // maximum power of Daum is 800W
        return 545;
    } 
    // search for the power range
    // i is the base point which is above of the input power
    i=0;
    while (power > data[i][0]){
        i++;
    }
/* // Debug
    Serial.print("i = ");
    Serial.print(i);
    Serial.print("\t");
    Serial.print("[");
    Serial.print(data[i][0]);
    Serial.print("][");
    Serial.print(data[i][1]);
    Serial.println("]");
*/
    // interpolate between base points 
    // speed = k*power+d
    if (i == 0){
        k = (float)(data[i+1][1]-data[i][1])/(float)(data[i+1][0]-data[i][0]);
        d = (float)data[i][1]- ((float)data[i][0]*k);
    }
    else{ // take the base point i and the next lower base point to calculate the liniar function for interpolation
        k = (float)(data[i][1]-data[i-1][1])/(float)(data[i][0]-data[i-1][0]);
        d = (float)data[i][1]- ((float)data[i][0]*k);
    }
    speed = k*(float)power + d; // calculation of speed
    return (uint16_t)speed; // speed in 1/10 km/h
}
