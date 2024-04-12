#include <Arduino.h>

#include "scanners/scanner_NRF.h"
#include "bluetooth/blth_print.h"
#include "motors/step_motor.h"
#include "gnss/gnss.h"
// #include <Wire.h>
// #include <LiquidCrystal_I2C.h>
// LiquidCrystal_I2C lcd(0x27, 16, 2);

#define MODE_TRANSMIT_NUMBER 0
#define MODE_TRANSMIT_TEXT 1
#define MODE_TRANSMIT MODE_TRANSMIT_TEXT

#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h> // Подключаем библиотеку для работа для работы с модулем NRF24L01
#define PIN_CE  PB10  // Номер пина Arduino, к которому подключен вывод CE радиомодуля
#define PIN_CSN PB11 // Номер пина Arduino, к которому подключен вывод CSN радиомодуля
RF24 radio(PIN_CE, PIN_CSN); // Создаём объект radio с указанием выводов CE и CSN

TinyGPSPlus gnss;

rf24_crclength_e preset_CRCLength;
uint8_t preset_AutoAck;
uint8_t preset_ChannelNumber;
rf24_datarate_e preset_DataRate;
uint64_t pipe;

PACKET pack;
PACKET packTonR = {0, 100, 0.0, 0.0};

int angle;
int prev_angle;

stepM::StepMotor motor;

void setup() {
    Serial.begin(115200);
    Serial.println("serial start");
    Serial2.begin(9600);    
    
    pipe = 0x7878787878LL;
    preset_CRCLength = RF24_CRC_DISABLED;
    preset_AutoAck = false;
    preset_ChannelNumber = 110;
    preset_DataRate = RF24_2MBPS;
    pack = {0, 100, 0.0, 0.0};

    angle = 0;
    prev_angle = 0;

    motor.start();

    pinMode(PC13, OUTPUT);

#if TR_MODE == MODE_SCANNER
    scannerStart();
    while (1) {
        scannerScan();
    }
#endif
    radio.begin();  // Инициализация модуля NRF24L01
    delay(3000);
    radio.setCRCLength(preset_CRCLength);
    radio.setAutoAck(preset_AutoAck);
    radio.setChannel(preset_ChannelNumber); // Обмен данными будет вестись на пятом канале (2,405 ГГц)
    radio.setDataRate(preset_DataRate); // Скорость обмена данными 1 Мбит/сек
    radio.setPALevel(RF24_PA_MAX); // Выбираем высокую мощность передатчика
    radio.powerUp();                  	// Включение или пониженное потребление powerDown - powerUp
    radio.stopListening();            	// Остановить прослушивание радиоэфира
    radio.printDetails();

#if TR_MODE == MODE_TRANSMITTER
    radio.openWritingPipe(pipe); // Открываем трубу с уникальным ID
    pack.num = 1;
    pack.crc = pack.num + 4;
#endif

#if TR_MODE == MODE_RECEIVER
    radio.openReadingPipe(1, pipe); // Открываем трубу ID передатчика
    radio.startListening(); // Начинаем прослушивать открываемую трубу
#endif  
}

uint8_t data = 12;
uint32_t c = 0;

uint32_t prev_packetNum = 0;

uint32_t currTime = 0;
uint32_t prevTime = 0;
uint32_t countTime = 0;
uint32_t ping = 0;

uint32_t printPrevTime = 0;
uint32_t printCurTime = 0;

uint32_t succPack = 0;
uint32_t missPack = 0;
uint32_t missPackCrc = 0;

uint16_t settings;
uint16_t buff;
int8_t i = 0;

bool menu = false;
bool settingsTabl = false;
bool scanner = false;

void loop() {
#if TR_MODE == MODE_TRANSMITTER
    if(!menu) {
        Serial.println("| Menu |");
        Serial.println("p - Packet statistic");
        Serial.println("s - NRF settings");
        Serial.println("m - Main Menu");
        Serial.println("g - GNSS");
        // Serial.println("S - scanner");
        menu = true;
        settings = 0;
    }

    if(Serial.available()) {
        buff = Serial.read();
        if(!(buff == 13 || buff == 10)) {
            settings = buff;
        }
    }

    currTime = micros();

    if (++c > 10) {
        c = 0;
        digitalWrite(PC13, !digitalRead(PC13));
    }
    GNSS::GNSSDelay(100);
    if(GNSS::LocIsValide()) {
        pack.latitude = GNSS::GetLatitude();
        pack.longitude = GNSS::GetLongitude();
    }

    #if MODE_TRANSMIT == MODE_TRANSMIT_NUMBER
        radio.write(&data, sizeof(data));
    #endif
    #if MODE_TRANSMIT == MODE_TRANSMIT_TEXT
        ++pack.num;
        pack.crc = (pack.num) + 100;
        radio.write(&pack, sizeof(pack));
    #endif

    countTime += currTime - prevTime;
    prevTime = currTime;

    switch(settings) {
        case BLTH_PACKET_STATISTIC:
            blth::TR::printPacketStatistics(printCurTime, printPrevTime, pack.num, countTime);
            break;
        case BLTH_SETTINGS:
            if(blth::settings(settingsTabl, radio)) {
                settings = BLTH_MENU;
            }
            break;
        case BLTH_MENU:
            menu = false;
            break;
        case BLTH_GNSS_STATISTIC:
            blth::printGNSSStatistics();
            break;
    }

    if(settings != BLTH_SETTINGS) {
        settingsTabl = false;
    }
#endif

#if TR_MODE == MODE_RECEIVER
    if(abs(angle - prev_angle) > DEGREE_DEVIATION) {
        motor.steps();
        prev_angle = angle; 
    }     

    if(!menu) {
        Serial.println("| Menu |");
        Serial.println("p - Packet statistic");
        Serial.println("s - NRF settings");
        Serial.println("m - Main Menu");
        Serial.println("g - GNSS");
        //Serial.println("S - scanner");
        menu = true;
        settings = 0;
    }

    if(Serial.available()) {
        buff = Serial.read();
        if(!(buff == 13 || buff == 10)) {
            settings = buff;
            Serial.println(settings);
        }
    }

    GNSS::GNSSDelay(100);
    if(GNSS::LocIsValide()) {
        pack.latitude = GNSS::GetLatitude();
        pack.longitude = GNSS::GetLongitude();
    }

    if (radio.available()) { // Если в буфер приёмника поступили данные;
        currTime = micros();
        #if MODE_TRANSMIT == MODE_TRANSMIT_NUMBER
            radio.read(&data, sizeof(data)); // Читаем показания потенциометра
            if (data == 12) {
                Serial.print("+");
            }

        #endif
        
        #if MODE_TRANSMIT == MODE_TRANSMIT_TEXT
            radio.read(&packTonR, sizeof(packTonR)); // Читаем показания

            if (packTonR.crc != (packTonR.num) + 100) {
                ++missPack;
                ++missPackCrc;
            }
            else {
                if(packTonR.num - prev_packetNum > 1){
                    missPack += packTonR.num - prev_packetNum - 1; 
                }
                else {
                    succPack++;
                }

                prev_packetNum = packTonR.num;
            }
        #endif
        
        countTime += currTime - prevTime;
        ping += currTime - prevTime;
        ping /= 2;
        prevTime = currTime;
    }

	angle = stepM::calculateAzimuth(pack.latitude, pack.longitude, packTonR.latitude, packTonR.longitude);
    motor.AngleToStep(angle);
    
    motor.controlMS1(0);
    motor.controlMS2(0);
    motor.controlMS3(0);

    motor.controlDIR(D_RIGHT);

    switch(settings) {
        case BLTH_PACKET_STATISTIC:
            blth::RE::printPacketStatistics(printCurTime, printPrevTime, succPack, missPack, missPackCrc, ping, countTime);
            break;
        case BLTH_SETTINGS:
            if(blth::settings(settingsTabl, radio)) {
                settings = BLTH_MENU;
            }
            break;
        case BLTH_MENU:
            menu = false;
            break;
        case BLTH_GNSS_STATISTIC:
            blth::printGNSSStatistics();
            Serial.println("Long and Lat TRANSMITTER:");
            Serial.println("LatT:");
            Serial.println(packTonR.latitude, 6);
            Serial.println("LngT:");
            Serial.println(packTonR.longitude, 6);
            Serial.println();
            break;

    }

    if(settings != BLTH_SETTINGS) {
        settingsTabl = false;
    }
#endif
}
 