#include "blth_print.h"
#include "../gnss/gnss.h"
#include "../radio/NRF24L01.h"

extern rf24_crclength_e preset_CRCLength;
extern uint8_t preset_AutoAck;
extern uint8_t preset_ChannelNumber;
extern rf24_datarate_e preset_DataRate;
extern uint64_t pipe;

extern PACKET pack;

bool blth::settings(bool &sT, RF24 &radio) {
    uint8_t settings; 
    if(!sT) {
        USBSerial.println("");
        USBSerial.println("| Settings |");
        
        USBSerial.print("& CRC Length ( ");
        switch (preset_CRCLength) {
            case RF24_CRC_8:
                USBSerial.println("CRC_8 ): ");
                USBSerial.println("  6  - CRC16");
                USBSerial.println("  d  - CRC_DISABLED");
                break;
            case RF24_CRC_16:
                USBSerial.println("CRC_16 ): ");
                USBSerial.println("  8  - CRC_8");
                USBSerial.println("  d  - CRC_DISABLED");
                break;
            case RF24_CRC_DISABLED:
                USBSerial.println("CRC_DISABLED ): ");
                USBSerial.println("  8  - CRC_8");
                USBSerial.println("  6  - CRC16");
                break;
        }

        USBSerial.print("& AutoAck ( ");
        if(preset_AutoAck) {
            USBSerial.println("true ): ");
            USBSerial.println("  f  - false");
        }
        else {
            USBSerial.println("false ): ");
            USBSerial.println("  t  - true");
        }

        USBSerial.print("& Chanel ( ");
        USBSerial.print(preset_ChannelNumber);
        USBSerial.println(" ) :");
        USBSerial.println("1 - +1 channel");
        USBSerial.println("o - -1 channel");
        USBSerial.println("2 - +10 channels");
        USBSerial.println("n - -10 channels");

        USBSerial.print("& Data Rate ( ");
        switch (preset_DataRate) {
            case RF24_1MBPS:
                USBSerial.println("1 MBPS ): ");
                USBSerial.println("  q  - 2 MBPS");
                USBSerial.println("  e  - 250 KBPS");
                break;
            case RF24_2MBPS:
                USBSerial.println("2 MBPS ): ");
                USBSerial.println("  w  - 1 MBPS");
                USBSerial.println("  e  - 250 KBPS");
                break;
            case RF24_250KBPS:
                USBSerial.println("250 KBPS ): ");
                USBSerial.println("  q  - 2 MBPS");
                USBSerial.println("  w  - 1 MBPS");
                break;
        }

        sT = true;
    } 

    if (USBSerial.available()) {
        settings = USBSerial.read();
        #if TR_MODE == MODE_RECEIVER
            radio.stopListening();
            radio.closeReadingPipe(pipe); // Открываем трубу ID передатчика
        #endif
        delay(100);
        switch(settings) {
            case BLTH_CRC_LENGTH_DISABLED:
                USBSerial.println("-> CRCLength: disabled"); 
                radio.setCRCLength(RF24_CRC_DISABLED);
                preset_CRCLength = RF24_CRC_DISABLED;
                break;
            case BLTH_CRC_LENGTH_8:
                USBSerial.println("-> CRCLength: 8"); 
                radio.setCRCLength(RF24_CRC_8);
                preset_CRCLength = RF24_CRC_8;
                break;
            case BLTH_CRC_LENGTH_16:
                USBSerial.println("-> CRCLength: 16"); 
                radio.setCRCLength(RF24_CRC_16);
                preset_CRCLength = RF24_CRC_16;
                break;
            case BLTH_AUTO_ACK_TRUE:
                USBSerial.println("-> AutoAck: true"); 
                radio.setAutoAck(true);
                preset_AutoAck = true;
                break;
            case BLTH_AUTO_ACK_FALSE:
                USBSerial.println("-> AutoAck: false"); 
                radio.setAutoAck(false);
                preset_AutoAck = false;
                break;
            case BLTH_PLUS_ONE_CHANNEL:
                if(preset_ChannelNumber < 126){
                    preset_ChannelNumber++;
                }
                radio.setChannel(preset_ChannelNumber);
                USBSerial.print("-> Channel Set: ");
                USBSerial.println(preset_ChannelNumber);
                break;
            case BLTH_MINUS_ONE_CHANNEL:
                if(preset_ChannelNumber >= 0){
                    preset_ChannelNumber--;
                }
                radio.setChannel(preset_ChannelNumber);
                USBSerial.print("-> Channel Set: ");
                USBSerial.println(preset_ChannelNumber);
                break;
            case BLTH_PLUS_TEN_CHANNELS:
                if(preset_ChannelNumber < 126){
                    preset_ChannelNumber += 10;
                }
                radio.setChannel(preset_ChannelNumber);
                USBSerial.print("-> Channel Set: ");
                USBSerial.println(preset_ChannelNumber);
                break;
            case BLTH_MINUS_TEN_CHANNELS:
                if(preset_ChannelNumber >= 0){
                    preset_ChannelNumber -= 10;
                }
                radio.setChannel(preset_ChannelNumber);
                USBSerial.print("-> Channel Set: ");
                USBSerial.println(preset_ChannelNumber);
                break;
            case BLTH_DATA_RATE_2MBPS:
                preset_DataRate = RF24_2MBPS;
                radio.setDataRate(preset_DataRate);
                USBSerial.println("-> Data Rate: 2 MBPS");
                break;
            case BLTH_DATA_RATE_1MBPS:
                preset_DataRate = RF24_1MBPS;
                radio.setDataRate(preset_DataRate);
                USBSerial.println("-> Data Rate: 1 MBPS");
                break;
            case BLTH_DATA_RATE_250KBPS:
                preset_DataRate = RF24_250KBPS;
                radio.setDataRate(preset_DataRate);
                USBSerial.println("-> Data Rate: 250 KBPS");
                break;
            case BLTH_MENU:
                USBSerial.println("     ***     ");
                radio.printDetails();
                USBSerial.println("     ***     ");
                return true;
        }
        delay(100);
        #if TR_MODE == MODE_TRANSMITTER
            radio.openWritingPipe(pipe); // Открываем трубу с уникальным ID
        #endif

        #if TR_MODE == MODE_RECEIVER
            radio.openReadingPipe(1, pipe); // Открываем трубу ID передатчика
            radio.startListening(); // Начинаем прослушивать открываемую трубу
        #endif
    }
    return false;
}

void blth::printMenu() {
    USBSerial.println("| Menu |");
    USBSerial.println("p - Packet statistic");
    USBSerial.println("s - NRF settings");
    USBSerial.println("m - Main Menu");
    USBSerial.println("g - GNSS");
    // USBSerial.println("S - scanner");
    settings = 0; //TODO 
}


uint32_t printPrevTime = 0;
uint32_t printCurTime = 0;
uint16_t buff;

void blth::getCommand() {
    static uint16_t settings;
    if(!USBSerial.available()) {
        return;
    }

    buff = USBSerial.read();
    if(buff == 13 || buff == 10) {
        return;
    }
    settings = buff;
    USBSerial.println(settings);

switch(settings) {
        case BLTH_PACKET_STATISTIC:
        #if TR_MODE == MODE_TRANSMITTER
            blth::TR::printPacketStatistics(printCurTime, printPrevTime, pack.num, countTime);
        #elif TR_MODE == MODE_RECEIVER
            blth::RE::printPacketStatistics(printCurTime, printPrevTime, succPack, missPack, missPackCrc, ping, countTime);
        #endif
            break;
        case BLTH_SETTINGS:
            if(blth::settings(settingsTabl, radio)) {
                settings = BLTH_MENU; // TODO возможно это  blth::printMenu();
            }
            break;
        case BLTH_MENU:
            blth::printMenu();
            break;
        case BLTH_GNSS_STATISTIC:
            blth::printGNSSStatistics();
            USBSerial.println("Long and Lat TRANSMITTER:");
            USBSerial.println("LatT:");
            USBSerial.println(packTonR.latitude, 6);
            USBSerial.println("LngT:");
            USBSerial.println(packTonR.longitude, 6);
            USBSerial.println();
            break;

    }
    if(settings != BLTH_SETTINGS) {
        settingsTabl = false;
    }
}

// pCT - printCurTime
// pPT - printPrevTime
// sP - suckPack
// mP - missPack
// mCRC - missPackCRC
// p - ping
// cT - countTime
void blth::RE::printPacketStatistics(uint32_t &pCT,
                                    uint32_t &pPT,
                                    uint32_t &sP,
                                    uint32_t &mP,
                                    uint32_t &mPCRC,
                                    uint32_t &p,
                                    uint32_t &cT) 
{
    pCT = millis();

    if (pCT - pPT > 1000) {
        USBSerial.print("S => ");
        USBSerial.print(sP);
        USBSerial.print("    |    ");
        USBSerial.print("M => ");
        USBSerial.print(mP);
        USBSerial.print("    |    ");
        USBSerial.print("MCRC => ");
        USBSerial.print(mPCRC);
        USBSerial.print("    |    ");
        USBSerial.print("Ping => ");
        USBSerial.print((float)p / 1000, 3);
        USBSerial.println(" ms");
        cT = 0;
        sP = 0;
        mP = 0;
        mPCRC = 0;
        pPT = pCT;
    }
}

// pCT - printCurTime
// pPT - printPrevTime
// sP - sentPuck
// cT - countTime
void blth::TR::printPacketStatistics(uint32_t &pCT,
                                    uint32_t &pPT,
                                    uint32_t sP,
                                    uint32_t &cT) 
{
    static uint32_t pSP; 
    pCT = millis();

    if (pCT - pPT > 1000) {
        USBSerial.print("Sent => ");
        USBSerial.println(sP - pSP);
        cT = 0;
        // pSP - prevSentPack
        pSP = sP;
        pPT = pCT;
    }
}

void blth::printGNSSStatistics() {
    
    if (!GNSS::LocIsValide()) {
        USBSerial.print("Latitude : ");
        USBSerial.println("*****");
        USBSerial.print("Longitude : ");
        USBSerial.println("*****");
        delay(4000);
    }
    else {
        USBSerial.println("GNSS READING: ");
    
        USBSerial.print("Latitude : ");
        USBSerial.println(pack.latitude, 6);
    
        USBSerial.print("Longitude : ");
        USBSerial.println(pack.longitude, 6);
    
        delay(4000);
    }
}
