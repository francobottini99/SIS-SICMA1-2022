#ifndef __SMS_CONTROLER_H__
#define __SMS_CONTROLER_H__

#include "Headers/main.h"

//---------------------------------PIN DECLARATION---------------------------------

//---------------------------------PIN DECLARATION---------------------------------

//---------------------------------CONSTANTS DECLARATION---------------------------------

char const MSG_OK[] PROGMEM = { "OK" };
char const MSG_ERR[] PROGMEM = { "ERROR" };
char const UNKNOWN_COMMAND[] PROGMEM = { "COMANDO DESCONOCIDO" };

char const GET_OK[] PROGMEM = { "OK?" };
char const GET_SALDO_NUM[] PROGMEM = { "SALDO_NUM?" };
char const GET_SALDO_MSG[] PROGMEM = { "SALDO_MSG?" };
char const GET_SALDO_VALUE[] PROGMEM = { "SALDO?" };
char const SET_SALDO_NUM[] PROGMEM = { "SALDO_NUM" };
char const SET_SALDO_MSG[] PROGMEM = { "SALDO_MSG" };
char const GET_TEMP[] PROGMEM = { "TEMP?" };
char const GET_LEVEL[] PROGMEM = { "NIV?" };
char const GET_LAST_W[] PROGMEM = { "ULT?" };
char const GET_STATE[] PROGMEM = { "EST?" };

char const SET_ON[] PROGMEM = { "BOM_ON!" };
char const SET_OFF[] PROGMEM = { "BOM_OFF!" };
char const SET_ENABLE[] PROGMEM = {"SIS_ON!"};
char const SET_DISABLE[] PROGMEM = {"SIS_OFF!"};

//---------------------------------CONSTANTS DECLARATION---------------------------------

//---------------------------------GLOBAL VARIABLES DECLARATION---------------------------------

extern bool request_saldo;
extern bool wait_saldo;

extern uint8_t saldo_fails; 

extern bool send_sms_ok;
extern bool send_sms_err;
extern bool send_sms_saldo;
extern bool send_sms_saldo_num;
extern bool send_sms_saldo_msg;
extern bool send_sms_saldo_bajo;
extern bool send_sms_fail_saldo;
extern bool alredy_send_saldo_bajo;
extern bool send_sms_unknown_command;

//---------------------------------GLOBAL VARIABLES DECLARATION---------------------------------

//---------------------------------FUNCTIONS DECLARATION---------------------------------

bool recibeSMS(char* buffer_msg, size_t size_buffer_msg, char* buffer_phone);
bool getSaldo(const char* msg, char* amount, char* prom);

//---------------------------------FUNCTIONS DECLARATION---------------------------------

#endif