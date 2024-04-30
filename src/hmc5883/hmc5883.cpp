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

void hmcRead() {
    
    Wire.beginTransmission(HMC_ADRESS);
   
        Wire.write(HMC_REGISTER_DATA);
 
    Wire.endTransmission();

    Wire.beginTransmission(HMC_ADRESS);
        Wire.requestFrom(HMC_ADRESS, 6);
    
    while(!Wire.available()) {};
    uint8_t data[6];
    for (size_t i = 0; i < 6; i++)
    {
        /* code */
        data[i] = Wire.read();
        BTSerial.print(data[i]);
        BTSerial.print(" ");
    }
    hmcX = data[0] << 8 | data[1];
    hmcY = data[2] << 8 | data[3];
    hmcZ = data[4] << 8 | data[5];
    BTSerial.print("\n");
        BTSerial.print(hmcX);
        BTSerial.print(" ");
        BTSerial.print(hmcY);
        BTSerial.print(" ");
        BTSerial.print(hmcZ);
    BTSerial.print("\n");
    
    Wire.endTransmission();

}