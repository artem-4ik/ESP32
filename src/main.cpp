#include <Arduino.h>
#include "config.h"
#include "scanners/scanner_NRF.h"
#include "bluetooth/blth_print.h"
#include "motors/step_motor.h"
#include "gnss/gnss.h"
#include "Radio/NRF24L01.h"

int angle = 0;
int prev_angle = 0;

stepM::StepMotor motor;

void setup() {
    USBSerial.begin(115200);
    BTSerial.begin(9600);
    USBSerial.println("USB start");
    BTSerial.println("BT start");

    motor.start();

    pinMode(PC13, OUTPUT);

#if USE_NRF == 1
#if TR_MODE == MODE_SCANNER
    scannerStart();
    while (1) {
        scannerScan();
    }
#endif

    nrf24l01Init();
#if TR_MODE == MODE_TRANSMITTER
    nrf24l01StartTransmitter();
#elif TR_MODE == MODE_RECEIVER
    nrf24l01StartReceiver();
#endif  
#endif
}

uint8_t data = 12;
uint32_t c = 0;
int8_t i = 0;

bool settingsTabl = false;
bool scanner = false;

void loop() {
#if TR_MODE == MODE_TRANSMITTER

    blth::getCommand();

    currTime = micros();

    if (++c > 10) {
        c = 0;
        digitalWrite(PC13, !digitalRead(PC13));
    }

   functionGNSSDelay();

   nrf24l01TestChennalSpeedTransmit();

#endif

#if TR_MODE == MODE_RECEIVER
    angelRotationCalculation();

    blth::getCommand();

    functionGNSSDelay();

   nrf24l01TestChennalSpeedReceive();

	angle = stepM::calculateAzimuth(pack.latitude, pack.longitude, packTonR.latitude, packTonR.longitude);
    motor.AngleToStep(angle);
    
    motor.controlMS1(0);
    motor.controlMS2(0);
    motor.controlMS3(0);
    
    motor.controlDIR(D_RIGHT);

#endif
}
 