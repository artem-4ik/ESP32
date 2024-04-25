#include "NRF24L01.h"
#include "../config.h"
#include "../bluetooth/blth_print.h"
RF24 radio(RF24_PIN_CE, RF24_PIN_CSN);
rf24_crclength_e preset_CRCLength;
uint8_t preset_AutoAck;
uint8_t preset_ChannelNumber;
rf24_datarate_e preset_DataRate;
uint64_t pipe;

uint8_t data = 12;
uint32_t prev_packetNum = 0;
uint32_t succPack = 0;
uint32_t missPack = 0;
uint32_t missPackCrc = 0;
uint32_t currTime = 0;
uint32_t prevTime = 0;
uint32_t countTime = 0;
uint32_t ping = 0;

PACKET pack;
PACKET packTonR = {0, 100, 0.0, 0.0};

void nrf24l01Init(){
    pipe = 0x7878787878LL;
    preset_CRCLength = RF24_CRC_DISABLED;
    preset_AutoAck = false;
    preset_ChannelNumber = 110;
    preset_DataRate = RF24_2MBPS;
    pack = {0, 100, 0.0, 0.0};
    radio.begin();  // Инициализация модуля NRF24L01
    delay(3000); // FIXME 
    radio.setCRCLength(preset_CRCLength);
    radio.setAutoAck(preset_AutoAck);
    radio.setChannel(preset_ChannelNumber); // Обмен данными будет вестись на пятом канале (2,405 ГГц)
    radio.setDataRate(preset_DataRate); // Скорость обмена данными 1 Мбит/сек
    radio.setPALevel(RF24_PA_MAX); // Выбираем высокую мощность передатчика
    radio.powerUp();                  	// Включение или пониженное потребление powerDown - powerUp
    radio.stopListening();            	// Остановить прослушивание радиоэфира
    radio.printDetails();
}

void nrf24l01StartTransmitter() {
    radio.openWritingPipe(pipe); // Открываем трубу с уникальным ID
    pack.num = 1;
    pack.crc = pack.num + 4;
}

void nrf24l01StartReceiver() {
    radio.openReadingPipe(1, pipe); // Открываем трубу ID передатчика
    radio.startListening(); // Начинаем прослушивать открываемую трубу
}

void nrf24l01TestChennalSpeedTransmit(){
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
}

void nrf24l01TestChannelSpeedReceive() {
#if USE_NRF == 1
    if (radio.available()) { // Если в буфер приёмника поступили данные;
        currTime = micros();
        #if MODE_TRANSMIT == MODE_TRANSMIT_NUMBER
            radio.read(&data, sizeof(data)); // Читаем показания потенциометра
            if (data == 12) {
                USBSerial.print("+");
            }
        #endif
        
        #if MODE_TRANSMIT == MODE_TRANSMIT_TEXT
            radio.read(&packTonR, sizeof(packTonR)); // Читаем показания
            if (packTonR.crc != (packTonR.num) + 100) {
                ++missPack;
                ++missPackCrc;
            } else {
                if(packTonR.num - prev_packetNum > 1){
                    missPack += packTonR.num - prev_packetNum - 1; 
                } else {
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
#endif
}
