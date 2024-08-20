#ifndef __MEMORY_CONTROLER_H__
#define __MEMORY_CONTROLER_H__

#include "Headers/main.h"

#include "EEPROM.h"

//---------------------------------PIN DECLARATION---------------------------------

//---------------------------------PIN DECLARATION---------------------------------

//---------------------------------CONSTANTS DECLARATION---------------------------------

const int16_t DIR_SALDO_NUMBER PROGMEM = sizeof(int16_t) + sizeof(char) * 16  + sizeof(char) * 50;
const int16_t DIR_SALDO_MSG PROGMEM = DIR_SALDO_NUMBER + sizeof(char) * 15;
//const int16_t - PROGMEM = DIR_SALDO_MSG + sizeof(char) * 30;

//---------------------------------CONSTANTS DECLARATION---------------------------------

//---------------------------------GLOBAL VARIABLES DECLARATION---------------------------------

//---------------------------------GLOBAL VARIABLES DECLARATION---------------------------------

//---------------------------------FUNCTIONS DECLARATION---------------------------------

void saveSaldoNumber(const char* saldoNumber, uint8_t size);
void saveSaldoMsg(const char* saldoMsg, uint8_t size);
bool readSaldoNumber(char* saldoNumber);
bool readSaldoMsg(char* saldoMsg);

//---------------------------------FUNCTIONS DECLARATION---------------------------------

#endif