#ifndef __MAIN_H__
#define __MAIN_H__

/*#ifndef CONSOLE_DEBUG
    #define CONSOLE_DEBUG
#endif*/

#ifdef CONSOLE_DEBUG
    #ifndef CONSOLE_PRINT
        #define CONSOLE_PRINT(x) Serial.print(x);
        //#define CONSOLE_PRINT(x)
    #endif
#endif

#ifdef CONSOLE_DEBUG
    #ifndef CONSOLE_PRINT_LN
        #define CONSOLE_PRINT_LN(x) Serial.println(x);
        //#define CONSOLE_PRINT_LN(x)
    #endif
#endif

#include <Arduino.h>
#include <avr/wdt.h>

#include "Headers/GatewayContol.h"
#include "Headers/MemoryControler.h"
#include "Headers/SMSControler.h"
#include "Headers/Packages.h"
#include "Headers/Enums.h"
#include "GPRS_Manager.h"

struct GatewayNotificationPackage;
struct GatewaySmsPackage;

//---------------------------------PIN DECLARATION---------------------------------

uint8_t const GPRS_TX_PIN PROGMEM = 7;
uint8_t const GPRS_RX_PIN PROGMEM = 8;
uint8_t const GPRS_POWER_PIN PROGMEM = 9;

//---------------------------------PIN DECLARATION---------------------------------

//---------------------------------CONSTANTS DECLARATION---------------------------------

char const NUM_J[] PROGMEM = { "+5493546501708" };
char const NUM_C[] PROGMEM = { "+5493585611808" };

char const SALDO_BAJO[] PROGMEM = { "ATENCION: SALDO MENOR A $ " };
char const ERR_GET_SALDO[] PROGMEM = { "NO SE PUDO OBTENER EL SALDO EN REITERADAS OCASIONES" };

#ifdef CONSOLE_DEBUG
    uint32_t const SERIAL_BAUD_RATE PROGMEM = 9600;
#endif

uint32_t const GPRS_BAUD_RATE PROGMEM = 9600;
uint8_t const LIMIT_CONN_INTENTS PROGMEM = 5;

//---------------------------------CONSTANTS DECLARATION---------------------------------

//---------------------------------GLOBAL VARIABLES DECLARATION---------------------------------

extern GPRS_Manager gprs_manager;

extern char saldo_msg[30];
extern char saldo_number[15];

extern uint8_t counts_to_reset_gprs;

extern uint16_t saldo;
extern char aux_saldo[10];

extern bool send_disfusion;
extern bool send_user;

//---------------------------------GLOBAL VARIABLES DECLARATION---------------------------------

//---------------------------------FUNCTIONS DECLARATION---------------------------------

void loop(void);
void setup(void);
void loadParams(void);
bool failControl(void);
bool recibeData(void);
void sendSms(GatewaySmsPackage* sms_package);
void smsManager(char* buffer, size_t buffer_size, char* phone = nullptr);

//---------------------------------FUNCTIONS DECLARATION---------------------------------

#endif