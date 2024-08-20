#ifndef __MAIN_H__
#define __MAIN_H__

/*#ifndef CONSOLE_DEBUG
    #define CONSOLE_DEBUG
#endif*/

#ifdef CONSOLE_DEBUG
    #ifndef CONSOLE_PRINT
        #define CONSOLE_PRINT(x) Serial.print(x);
    #endif
#endif

#ifdef CONSOLE_DEBUG
    #ifndef CONSOLE_PRINT_LN
        #define CONSOLE_PRINT_LN(x) Serial.println(x);
    #endif
#endif

#include "LevelSensor.h"

#include "RTClib.h"

#include "Headers/NodeControler.h"
#include "Headers/ContolSystems.h"
#include "Headers/Packages.h"
#include "Headers/Enums.h"

#include <avr/wdt.h>
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>

struct ParamPackage;
struct SensedPackage;
struct TempPackage;
struct RF24NotificationPackage;
struct StatePackage;

//---------------------------------PIN DEFINITION---------------------------------

uint8_t const DISPLAY_VALUE_PIN PROGMEM = A5;
uint8_t const LEVEL_SENSOR_READ_PIN PROGMEM = 7;
uint8_t const MANUAL_MODE_SENSOR_PIN PROGMEM = 10;
uint8_t const DISPLAY_CONTROL_BITS_PINS[] = { 6, 5, A4 };
uint8_t const LEVEL_SENSOR_CONTROL_BITS_PINS[] = { 2, 3, 4 };

//---------------------------------PIN DEFINITION---------------------------------

//---------------------------------CONSTANTS DEFINITION---------------------------------

uint8_t const SENSOR_NUMBER_LEVELS PROGMEM = 7;
uint8_t const NUMBER_PACKAGE_IN_RAM PROGMEM = 14;
uint16_t const EEPROM_DIR_CYCLE PROGMEM = 0;

#ifdef CONSOLE_DEBUG
    uint32_t const SERIAL_BAUD_RATE PROGMEM = 9600;
#endif

//---------------------------------CONSTANTS DEFINITION---------------------------------

//---------------------------------GLOBAL VARIABLES DECLARATION---------------------------------

extern uint16_t cycle;
extern uint32_t system_time;
extern uint32_t time_synchronization;
extern uint32_t retry_send_time;

extern LevelSensor* level_sensor;
extern RTC_DS3231* rtc;

extern ParamPackage param_package;
extern SensedPackage sendsed_package;
extern TempPackage temp_package;
extern StatePackage state_package;

extern bool send_package;
extern bool send_working_info;
extern bool send_sendsed_package;
extern bool send_temp_info;
extern bool send_state_info;

//---------------------------------GLOBAL VARIABLES DECLARATION---------------------------------

//---------------------------------FUNCTIONS DECLARATION---------------------------------

void sendData(void);
void recibeData(void);
void sensingLevel(void);
void setup(void);
void loop(void);
void EEPROM_saveCycle(const uint16_t* cycle);
bool EEPROM_readCycle(uint16_t* cycle);
void EEPROM_resetCycle(void);

//---------------------------------FUNCTIONS DECLARATION---------------------------------
#endif