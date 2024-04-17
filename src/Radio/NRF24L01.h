#include <Arduino.h>
#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h>

#ifndef MY_NRF24L01_H
#define MY_NRF24L01_H

uint32_t prev_packetNum = 0;
uint32_t succPack = 0;
uint32_t missPack = 0;
uint32_t missPackCrc = 0;

uint32_t currTime = 0;
uint32_t prevTime = 0;
uint32_t countTime = 0;
uint32_t ping = 0;

void nrf24l01Init();
void nrf24l01StartTransmitter();
void nrf24l01StartReceiver();
void nrf24l01TestChennalSpeedTransmit();
void nrf24l01TestChennalSpeedReceive();

#endif
