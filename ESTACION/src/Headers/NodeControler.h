#ifndef __RF24_CONTROLER_H__
#define __RF24_CONTROLER_H__

#include "Headers/main.h"
#include "Headers/Packages.h"

#include "RF24Network.h"
#include "RF24.h"
#include "RF24Mesh.h"

//---------------------------------PIN DEFINITION---------------------------------

uint8_t const NRF24L01_CSN_PIN PROGMEM = 8;
uint8_t const NRF24L01_CE_PIN PROGMEM = 9;

//---------------------------------PIN DEFINITION---------------------------------

//---------------------------------TYPE DECLARATION---------------------------------

//---------------------------------TYPE DECLARATION---------------------------------

//---------------------------------TYPE DECLARATION---------------------------------


uint8_t const NODE_ID PROGMEM = 1;

//---------------------------------CONSTANTS DEFINITION---------------------------------

//---------------------------------GLOBAL VARIABLES DECLARATION---------------------------------

extern RF24 radio;
extern RF24Network network;
extern RF24Mesh mesh;

extern uint32_t time_out_reset;

//---------------------------------GLOBAL VARIABLES DECLARATION---------------------------------

//---------------------------------FUNCTIONS DECLARATION---------------------------------

//bool sendDataRF24(const unsigned char header, const RF24Paquet* request, const uint8_t node = 0);
bool recibeDataRF24(RF24NetworkHeader* header, RF24NotificationPackage* paquet);

bool sendDataRF24(const unsigned char header, SensedPackage paquet, const uint8_t node = 0);
bool sendDataRF24(const unsigned char header, RF24NotificationPackage paquet, const uint8_t node = 0);
bool sendDataRF24(const unsigned char header, TempPackage paquet, const uint8_t node = 0);
bool sendDataRF24(const unsigned char header, WorkingPackage paquet, const uint8_t node = 0);
bool sendDataRF24(const unsigned char header, StatePackage paquet, const uint8_t node = 0);

bool connectRF24(void);
bool renewAdressRF24(void);

//---------------------------------FUNCTIONS DECLARATION---------------------------------

#endif