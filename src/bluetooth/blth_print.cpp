#include "blth_print.h"
#include "../gnss/gnss.h"

extern rf24_crclength_e preset_CRCLength;
extern uint8_t preset_AutoAck;
extern uint8_t preset_ChannelNumber;
extern rf24_datarate_e preset_DataRate;
extern uint64_t pipe;

extern PACKET pack;

bool blth::settings(bool &sT, RF24 &radio) {
    uint8_t settings;
    if(!sT) {
        Serial.println("");
        Serial.println("| Settings |");
        
        Serial.print("& CRC Length ( ");
        switch (preset_CRCLength) {
            case RF24_CRC_8:
                Serial.println("CRC_8 ): ");
                Serial.println("  6  - CRC16");
                Serial.println("  d  - CRC_DISABLED");
                break;
            case RF24_CRC_16:
                Serial.println("CRC_16 ): ");
                Serial.println("  8  - CRC_8");
                Serial.println("  d  - CRC_DISABLED");
                break;
            case RF24_CRC_DISABLED:
                Serial.println("CRC_DISABLED ): ");
                Serial.println("  8  - CRC_8");
                Serial.println("  6  - CRC16");
                break;
        }

        Serial.print("& AutoAck ( ");
        if(preset_AutoAck) {
            Serial.println("true ): ");
            Serial.println("  f  - false");
        }
        else {
            Serial.println("false ): ");
            Serial.println("  t  - true");
        }

        Serial.print("& Chanel ( ");
        Serial.print(preset_ChannelNumber);
        Serial.println(" ) :");
        Serial.println("1 - +1 channel");
        Serial.println("o - -1 channel");
        Serial.println("2 - +10 channels");
        Serial.println("n - -10 channels");

        Serial.print("& Data Rate ( ");
        switch (preset_DataRate) {
            case RF24_1MBPS:
                Serial.println("1 MBPS ): ");
                Serial.println("  q  - 2 MBPS");
                Serial.println("  e  - 250 KBPS");
                break;
            case RF24_2MBPS:
                Serial.println("2 MBPS ): ");
                Serial.println("  w  - 1 MBPS");
                Serial.println("  e  - 250 KBPS");
                break;
            case RF24_250KBPS:
                Serial.println("250 KBPS ): ");
                Serial.println("  q  - 2 MBPS");
                Serial.println("  w  - 1 MBPS");
                break;
        }

        sT = true;
    } 

    if (Serial.available()) {
        settings = Serial.read();
        #if TR_MODE == MODE_RECEIVER
            radio.stopListening();
            radio.closeReadingPipe(pipe); // Открываем трубу ID передатчика
        #endif
        delay(100);
        switch(settings) {
            case BLTH_CRC_LENGTH_DISABLED:
                Serial.println("-> CRCLength: disabled"); 
                radio.setCRCLength(RF24_CRC_DISABLED);
                preset_CRCLength = RF24_CRC_DISABLED;
                break;
            case BLTH_CRC_LENGTH_8:
                Serial.println("-> CRCLength: 8"); 
                radio.setCRCLength(RF24_CRC_8);
                preset_CRCLength = RF24_CRC_8;
                break;
            case BLTH_CRC_LENGTH_16:
                Serial.println("-> CRCLength: 16"); 
                radio.setCRCLength(RF24_CRC_16);
                preset_CRCLength = RF24_CRC_16;
                break;
            case BLTH_AUTO_ACK_TRUE:
                Serial.println("-> AutoAck: true"); 
                radio.setAutoAck(true);
                preset_AutoAck = true;
                break;
            case BLTH_AUTO_ACK_FALSE:
                Serial.println("-> AutoAck: false"); 
                radio.setAutoAck(false);
                preset_AutoAck = false;
                break;
            case BLTH_PLUS_ONE_CHANNEL:
                if(preset_ChannelNumber < 126){
                    preset_ChannelNumber++;
                }
                radio.setChannel(preset_ChannelNumber);
                Serial.print("-> Channel Set: ");
                Serial.println(preset_ChannelNumber);
                break;
            case BLTH_MINUS_ONE_CHANNEL:
                if(preset_ChannelNumber >= 0){
                    preset_ChannelNumber--;
                }
                radio.setChannel(preset_ChannelNumber);
                Serial.print("-> Channel Set: ");
                Serial.println(preset_ChannelNumber);
                break;
            case BLTH_PLUS_TEN_CHANNELS:
                if(preset_ChannelNumber < 126){
                    preset_ChannelNumber += 10;
                }
                radio.setChannel(preset_ChannelNumber);
                Serial.print("-> Channel Set: ");
                Serial.println(preset_ChannelNumber);
                break;
            case BLTH_MINUS_TEN_CHANNELS:
                if(preset_ChannelNumber >= 0){
                    preset_ChannelNumber -= 10;
                }
                radio.setChannel(preset_ChannelNumber);
                Serial.print("-> Channel Set: ");
                Serial.println(preset_ChannelNumber);
                break;
            case BLTH_DATA_RATE_2MBPS:
                preset_DataRate = RF24_2MBPS;
                radio.setDataRate(preset_DataRate);
                Serial.println("-> Data Rate: 2 MBPS");
                break;
            case BLTH_DATA_RATE_1MBPS:
                preset_DataRate = RF24_1MBPS;
                radio.setDataRate(preset_DataRate);
                Serial.println("-> Data Rate: 1 MBPS");
                break;
            case BLTH_DATA_RATE_250KBPS:
                preset_DataRate = RF24_250KBPS;
                radio.setDataRate(preset_DataRate);
                Serial.println("-> Data Rate: 250 KBPS");
                break;
            case BLTH_MENU:
                Serial.println("     ***     ");
                radio.printDetails();
                Serial.println("     ***     ");
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
        Serial.print("S => ");
        Serial.print(sP);
        Serial.print("    |    ");
        Serial.print("M => ");
        Serial.print(mP);
        Serial.print("    |    ");
        Serial.print("MCRC => ");
        Serial.print(mPCRC);
        Serial.print("    |    ");
        Serial.print("Ping => ");
        Serial.print((float)p / 1000, 3);
        Serial.println(" ms");
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
        Serial.print("Sent => ");
        Serial.println(sP - pSP);
        cT = 0;
        // pSP - prevSentPack
        pSP = sP;
        pPT = pCT;
    }
}

void blth::printGNSSStatistics() {
    
    if (!GNSS::LocIsValide()) {
        Serial.print("Latitude : ");
        Serial.println("*****");
        Serial.print("Longitude : ");
        Serial.println("*****");
        delay(4000);
    }
    else {
        Serial.println("GNSS READING: ");
    
        Serial.print("Latitude : ");
        Serial.println(pack.latitude, 6);
    
        Serial.print("Longitude : ");
        Serial.println(pack.longitude, 6);
    
        delay(4000);
    }
}
