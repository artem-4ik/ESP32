#include "gnss.h"
#include "../config.h"
TinyGPSPlus gnss;
extern SoftwareSerial ss;

void GNSS::GNSSDelay(uint32_t ms) {
    uint32_t start = millis();
    do {
        while (BTSerial.available()) {
            gnss.encode(BTSerial.read());
        }
    } while (millis() - start < ms);
}

double GNSS::GetLatitude() {
    return gnss.location.lat();
}

double GNSS::GetLongitude() {
    return gnss.location.lng();
}

bool GNSS::LocIsValide() {
    return gnss.location.isValid(); 
}

void functionGNSSDelay() { 
#if USE_GNSS == 1
    GNSS::GNSSDelay(100);
    if(GNSS::LocIsValide()) {
        pack.latitude = GNSS::GetLatitude();
        pack.longitude = GNSS::GetLongitude();
    }
#endif
}
