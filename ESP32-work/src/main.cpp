#include <Arduino.h>
#include "config.h"
#include "scanners/scanner_NRF.h"
#include "bluetooth/blth_print.h"
#include "motors/step_motor.h"
#include "gnss/gnss.h"
#include "Radio/NRF24L01.h"
#include "scanners/scanner_I2C.h"
#include "Servo.h"
#include "hmc5883/hmc5883.h"

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

Servo test_servo;
stepM::StepMotor motor;

void setup() {
    Serial.begin(9600);
    USBSerial.begin(115200);
    BTSerial.begin(9600);
    USBSerial.println("USB start");
    BTSerial.println("BT start");
    scanI2C();
    hmcInit();
    pinMode(LED, OUTPUT);
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
 test_servo.attach(PA8); // указываем управляющий пин
  delay(100);
  // устанавливаем начальное положение сервопривода
  test_servo.write(5);
  delay(1000); 
}

uint32_t c = 0;

void loop() {
sensors_event_t event;
mag.getEvent(&event);
float heading = atan2(event.magnetic.y, event.magnetic.x);
float declinationAngle = 0.22;
heading += declinationAngle;
if (heading < 0) {
heading += 2 * PI;
}
if (heading > 2 * PI) {
heading -= 2 * PI;
}
float headingDegrees = heading * 180 / M_PI;
Serial.print("Heading : ");
Serial.print(headingDegrees);
Serial.println(" degree");
delay(500);

#if TR_MODE == MODE_TRANSMITTER
    blth::getCommand();
    currTime = micros();
    if (++c > 10) {
        c = 0;
        digitalWrite(LED, !digitalRead(LED));
    }
   functionGNSSDelay();
   nrf24l01TestChennalSpeedTransmit();
#endif

#if TR_MODE == MODE_RECEIVER
    blth::getCommand();
    functionGNSSDelay();
    nrf24l01TestChannelSpeedReceive();
    motor.angleRotationCalculation();
    motor.setDir(smdClockWise);
    motor.makeStep();

#endif
//   test_servo.write(5); 
  // разжимаем клешню
//   test_servo.write(60);
hmcRead();
  delay(1000);
}
