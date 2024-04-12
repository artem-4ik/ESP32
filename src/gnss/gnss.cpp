#include "gnss.h"

extern TinyGPSPlus gnss;
extern SoftwareSerial ss;

void GNSS::GNSSDelay(uint32_t ms) {
    uint32_t start = millis();
    do {
        while (Serial2.available()) {
            gnss.encode(Serial2.read());
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
