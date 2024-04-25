#ifndef HMC5883_H
#define HMC5883_H

#include <Arduino.h>
#include <Wire.h>

#define HMC_ADRESS 0x1E

#define HMC_REGISTER_A 0x00
#define HMC_REGISTER_B 0x01
#define HMC_REGISTER_MODE 0x02
#define HMC_REGISTER_DATA 0x03

#define HMC_DO_0_75 0
#define HMC_DO_1_5 1
#define HMC_DO_3 2
#define HMC_DO_7_5 3
#define HMC_DO_15 4
#define HMC_DO_30 5
#define HMC_DO_75 6

#define HMC_MS 0

#define HMC_MD 0 /* Continuous-Conversion Mode. */

#define HMC_GN 3 /* ± 2.5Ga */

void hmcInit();
void hmcRead();


#endif
