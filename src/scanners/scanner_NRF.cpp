#include "scanner_NRF.h"

const uint8_t num_channels = 128;
uint8_t values[num_channels];

const int num_reps = 100;

void scannerStart() {
    radio.begin();
    
    radio.setAutoAck(false);
    radio.startListening();
    radio.printDetails();
    delay(5000);

    int i = 0;
    while (i < num_channels) {
        printf("%x", i >> 4);
        ++i;
    }
    printf("\r\n");
    
    i = 0;
    while (i < num_channels) {
        printf("%x", i & 0xf);
        ++i;
    }
    printf("\r\n");
}

void scannerScan() {
    // Clear measurement values
    memset(values, 0, sizeof(values));

    // Scan all channels num_reps times
    int rep_counter = num_reps;
    while (rep_counter--) {
        int i = num_channels;
        while (i--) {
            // Select this channel
            radio.setChannel(i);

            // Listen for a little
            radio.startListening();
            delayMicroseconds(512);
            radio.stopListening();

            // Did we get a carrier?
            if (radio.testCarrier())
            ++values[i];
        }
    }

    // Print out channel measurements, clamped to a single hex digit
    int i = 0;
    while (i < num_channels) {
        printf("%x", min(0xf, values[i] & 0xf));
        ++i;
    }
    printf("\r\n");
}
