#ifndef MY_NRF24L01_H
#define MY_NRF24L01_H

#include <Arduino.h>
#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h>

extern uint32_t succPack;
extern uint32_t missPack;
extern uint32_t missPackCrc;
extern uint32_t currTime;
extern uint32_t countTime;
extern uint32_t ping;

void nrf24l01Init();
void nrf24l01StartTransmitter();
void nrf24l01StartReceiver();
void nrf24l01TestChennalSpeedTransmit();
void nrf24l01TestChannelSpeedReceive();

#endif
