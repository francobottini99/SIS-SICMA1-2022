#ifndef __GATEWAY_CONTROLER_H__
#define __GATEWAY_CONTROLER_H__

#include "Headers/main.h"
#include "Headers/Packages.h"

#include <Wire.h>

//---------------------------------CONSTANTS DEFINITION---------------------------------

uint8_t const GATEWAY_ADRRESS PROGMEM = 0x20;

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

//---------------------------------CONSTANTS DEFINITION---------------------------------

//---------------------------------TYPE DECLARATION---------------------------------

//---------------------------------TYPE DECLARATION---------------------------------

//---------------------------------GLOBAL VARIABLES DEFINITION---------------------------------

/*
   status = 'S' --> listo para recibir informacion
   status = 'W' --> procesando informacion recibida
   status = 'R' --> informacion disponible para lectura
*/
extern bool flag;

extern bool recibe_buffer;
extern bool recibe_notification;

extern bool send_buffer;
extern bool send_notification;

extern char gateway_read_notification_buffer[];
extern char gateway_write_notification_buffer[];

extern GatewayNotificationPackage* gateway_read_notification_package;
extern GatewayNotificationPackage* gateway_write_notification_package;

extern char* gateway_read_buffer;
extern char* gateway_write_buffer;

extern uint16_t size_read_buffer;
extern uint16_t len_write_buffer;
extern uint16_t len_data;

extern uint32_t nC;

extern bool inTransmission;

extern char tag;
extern char request;

//---------------------------------GLOBAL VARIABLES DEFINITION---------------------------------

//---------------------------------FUNCTIONS DEFINITION---------------------------------

void receiveEvent(int bytes);
void requestEvent(void);
void sendToGateway(char* buffer, size_t len_buffer);
void sendToGateway(GatewayNotificationPackage* notification_package, char* buffer, size_t size_buffer);
void sendToGateway(GatewayNotificationPackage* notification_package);
void recibePackageFromGateway(GatewayNotificationPackage* notification_package);
void recibeBufferFromGateway(char* buffer, size_t size_buffer);
void cleanBuffer(char* buffer, uint16_t size_buffer);

#ifdef CONSOLE_DEBUG
   void consolePrint_Recibe(GatewayNotificationPackage* notification_package);
   void consolePrint_Send(GatewayNotificationPackage* notification_package);
   void consolePrint_Recibe(const char* data);
   void consolePrint_Send(const char* data);
#endif

//---------------------------------FUNCTIONS DEFINITION---------------------------------


#endif