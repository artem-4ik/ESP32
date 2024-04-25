#ifndef BLTH_H
#define BLTH_H

#include <Arduino.h>
#include "../config.h"
#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h> // Подключаем библиотеку для работа для работы с модулем NRF24L01

#define BLTH_PACKET_STATISTIC 112       // p
#define BLTH_SETTINGS 115               // s
#define BLTH_MENU 109                   // m
#define BLTH_SCANNER 83                 // S
#define BLTH_GNSS_STATISTIC 103          // g

#define BLTH_CRC_LENGTH_8 56            // 8
#define BLTH_CRC_LENGTH_16 54           // 6
#define BLTH_CRC_LENGTH_DISABLED 100    // d

#define BLTH_AUTO_ACK_TRUE 116          // t
#define BLTH_AUTO_ACK_FALSE 102         // f

#define BLTH_PLUS_ONE_CHANNEL 49        // 1
#define BLTH_MINUS_ONE_CHANNEL 111      // 0
#define BLTH_PLUS_TEN_CHANNELS 50       // 2
#define BLTH_MINUS_TEN_CHANNELS 110     // n

#define BLTH_DATA_RATE_250KBPS 101      // e
#define BLTH_DATA_RATE_1MBPS 119        // w
#define BLTH_DATA_RATE_2MBPS 113        // q

struct PACKET {
    uint32_t num;
    uint32_t crc;
    double latitude;
    double longitude;
};

static_assert(sizeof(PACKET) > 10, "Size is not correct");

namespace blth {
    bool settings(bool &sT, RF24 &radio);
    void printMenu();
    void getCommand();
    // Reciver
    namespace RE {
        void printPacketStatistics(uint32_t &pCT,
                                uint32_t &pPT,
                                uint32_t &sP,
                                uint32_t &mP,
                                uint32_t &mPCRC,
                                uint32_t &p,
                                uint32_t &cT);
    }
    // Transmitter
    namespace TR {
        void printPacketStatistics(uint32_t &pCT,
                                uint32_t &pPT,
                                uint32_t sP,
                                uint32_t &cT);
    }
    void printGNSSStatistics();
}
#endif
