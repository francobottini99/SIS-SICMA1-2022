#ifndef __GATEWAY_CONTROLER_H__
#define __GATEWAY_CONTROLER_H__

#include "Headers/main.h"
#include "Headers/Packages.h"

#include <Wire.h>

struct GatewayNotificationPackage;
struct GatewaySmsPackage;

//---------------------------------CONSTANTS DEFINITION---------------------------------

uint8_t const GATEWAY_ADRRESS PROGMEM = 0x20;
uint16_t const SIZE_GATEWAY_BUFFER PROGMEM = 256;

char const C_ASK_FOR_DATA = '\5';
char const C_END_TRASMISSION = '\3';
char const C_START_TRASMISSION = '\2';
char const C_ASK_FOR_LENGTH = '\6';
char const C_FAIL_TRASMISSION = '\23';
char const C_RECIBE = '\24';

char const C_ASK_RECIBE = '\1';
char const C_ASK_SEND = '\7';

char const C_NOTIFICATION = '\21';
char const C_BUFFER = '\22';
char const C_NULL = '\0';

extern char gateway_buffer[];

extern uint16_t size_data;

//---------------------------------CONSTANTS DEFINITION---------------------------------

//---------------------------------TYPE DECLARATION---------------------------------

//---------------------------------TYPE DECLARATION---------------------------------

//---------------------------------GLOBAL VARIABLES DEFINITION---------------------------------

//---------------------------------GLOBAL VARIABLES DEFINITION---------------------------------

//---------------------------------FUNCTIONS DEFINITION---------------------------------

bool recibePackageFromGateway(GatewayNotificationPackage* gateway_notification_package, size_t responseLenght);
bool recibeBufferFromGateway(char* buffer, size_t responseLenght);
bool sendToGateway(GatewayNotificationPackage gateway_notification_package);
bool sendToGateway(GatewayNotificationPackage gateway_notification_package, const char* data);
bool sendToGateway(GatewayNotificationPackage gateway_notification_package, GatewaySmsPackage data);
bool sendGatewayBuffer(void);
char askRecibe(void);
char askSend(void);
size_t askForLength(void);

//---------------------------------FUNCTIONS DEFINITION---------------------------------


#endif