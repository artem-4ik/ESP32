//платформа

#define BOARD_STM

//логирование

#define LOG 1
#if LOG == 1
#define LOG_BT 1
#define LOG_USB 1
#else 
#define LOG_BT 0
#define LOG_USB 0
#endif

//глобальные настройки

#define MODE_SCANNER 0
#define MODE_TRANSMITTER 1
#define MODE_RECEIVER 2
#define TR_MODE MODE_RECEIVER

#define MODE_TRANSMIT_NUMBER 0
#define MODE_TRANSMIT_TEXT 1
#define MODE_TRANSMIT MODE_TRANSMIT_TEXT

//переферия

#define USE_GNSS 0

#define USE_NRF 1

#define BTSerial Serial2
#define USBSerial Serial
//частные настройки

#ifdef BOARD_STM
    #define RF24_PIN_CE  PB10  // Номер пина Arduino, к которому подключен вывод CE радиомодуля
    #define RF24_PIN_CSN PB11 // Номер пина Arduino, к которому подключен вывод CSN радиомодуля
#endif
