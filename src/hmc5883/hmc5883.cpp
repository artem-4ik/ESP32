#include "hmc5883.h"
#include "../bluetooth/blth_print.h"

int16_t hmcX, hmcY, hmcZ;


void hmcInit() {

    Wire.begin();

    Wire.beginTransmission(HMC_ADRESS);  

    Wire.write(HMC_REGISTER_A); 
    Wire.write(HMC_DO_15 << 2 | HMC_MS ); 
    Wire.endTransmission();
    

    Wire.beginTransmission(HMC_ADRESS);  

    Wire.write(HMC_REGISTER_B); 
    Wire.write(HMC_GN << 5 ); 
    Wire.endTransmission();

    Wire.beginTransmission(HMC_ADRESS);  

    Wire.write(HMC_REGISTER_MODE); 
    Wire.write(HMC_MD); 
    Wire.endTransmission();
}

int16_t hmcRead() {
    static int16_t oldA = 0;

    Wire.beginTransmission(HMC_ADRESS);
   
        Wire.write(HMC_REGISTER_DATA);
 
    Wire.endTransmission();

    Wire.beginTransmission(HMC_ADRESS);
        Wire.requestFrom(HMC_ADRESS, 6);
    
    while(!Wire.available()) {    };
    uint8_t data[6];
    for (size_t i = 0; i < 6; i++)
    {
        /* code */
        data[i] = Wire.read();
        // BTSerial.print(data[i]);
        // BTSerial.print(" ");
    }
    hmcX = data[0] << 8 | data[1];
    hmcY = data[4] << 8 | data[5];
    hmcZ = data[2] << 8 | data[3];

    hmcX -= 50;
    hmcY += 280;
 
    static int32_t prev_time = 0;
    static int32_t new_time;

    new_time = millis();
    if (new_time - prev_time < 300){
        return oldA;
    }
    prev_time = new_time;
    
    float a = atan2(hmcY, hmcX);
    BTSerial.print(a * 180 / 3.14);
    BTSerial.print("\n");  

oldA = (int16_t) (a * 180 / 3.14);
    
    // BTSerial.print("\n");
    // BTSerial.print("X = ");
    // BTSerial.print(hmcX);
    // BTSerial.print(" ");
    // BTSerial.print("Y = ");
    // BTSerial.print(hmcY);
    // BTSerial.print(" ");
    // BTSerial.print("Z = ");
    // BTSerial.print(hmcZ);
    // BTSerial.print("\n");
    
    Wire.endTransmission();
    return oldA;
}