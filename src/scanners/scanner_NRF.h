
#include <Arduino.h>
#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h> // Подключаем библиотеку для работа для работы с модулем NRF24L01

extern RF24 radio;

void scannerStart();
void scannerScan();
