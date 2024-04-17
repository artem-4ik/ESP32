#ifndef GNSS_H
#define GNSS_H

#include <TinyGPS++.h>
#include <algorithm>
#include <SoftwareSerial.h>

#define GNSS_RX PA10
#define GNSS_TX PA9

void functionGNSSDelay();

namespace GNSS {
    void GNSSDelay(uint32_t ms);
    double GetLatitude();
    double GetLongitude();
    bool LocIsValide();
}
 
#endif
