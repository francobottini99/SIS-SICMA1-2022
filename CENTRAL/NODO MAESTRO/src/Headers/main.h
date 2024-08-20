#ifndef __MAIN_H__
#define __MAIN_H__

/*#ifndef CONSOLE_DEBUG
    #define CONSOLE_DEBUG
#endif*/

#ifdef CONSOLE_DEBUG
    #ifndef CONSOLE_PRINT
        #define CONSOLE_PRINT(x) Serial.print(x);
    #endif

    #ifndef CONSOLE_PRINT_LN
        #define CONSOLE_PRINT_LN(x) Serial.println(x);
    #endif
#endif

#include "RTClib.h"

#include "Headers/NodeControler.h"
#include "Headers/LCDControler.h"
#include "Headers/GatewayControl.h"
#include "Headers/RTCControler.h"
#include "Headers/Packages.h"
#include "Headers/FlashString.h"
#include "Headers/Enums.h"
#include <avr/wdt.h>

#include <Arduino.h>

struct SensedPackage;
struct TempPackage;
struct WorkingPackage;
struct RF24NotificationPackage;
struct GatewayNotificationPackage;
struct GatewaySmsPackage;
struct StatePackage;

//---------------------------------CONSTANTS DECLARATION---------------------------------

#ifdef CONSOLE_DEBUG
    uint32_t const SERIAL_BAUD_RATE PROGMEM = 9600;
#endif

uint16_t const BUFFER_RF24_SIZE PROGMEM = 256;
uint16_t const FLASH_LOAD_BUFFER_SIZE PROGMEM = 128;

uint16_t const NODE_TEMP_SENSORS PROGMEM = 2;

//---------------------------------CONSTANTS DECLARATION---------------------------------

//---------------------------------GLOBAL VARIABLES DECLARATION---------------------------------

extern uint32_t time_out;
extern uint32_t time_delay;
extern uint32_t time_temp_info;
extern uint32_t time_level_info;
extern uint32_t time_working_info;
extern uint32_t time_state_info;

extern char buffer_RF24[];
extern char flash_load_buffer[];

extern char buffer_order_phone[16];

extern bool send_manual_on;
extern bool send_manual_off;
extern bool send_gateway;
extern bool request_level;
extern bool request_info_working;
extern bool request_temp_info;
extern bool request_saldo;
extern bool request_state;

extern SensedPackage sensed_package;
extern TempPackage temp_packages[];
extern WorkingPackage working_package;
extern StatePackage state_package;

//---------------------------------GLOBAL VARIABLES DECLARATION---------------------------------

//---------------------------------FUNCTIONS DECLARATION---------------------------------

void loop(void);
void setup(void);
void sendDataExtern(void);
void recibeDataExtern(void);
void gateway(void);
bool loadStringFromFlashMemory(char* buffer, GatewayCodes code);
bool loadStringFromFlashMemory(char* buffer, RF24_Codes code);

//---------------------------------FUNCTIONS DECLARATION---------------------------------

#endif