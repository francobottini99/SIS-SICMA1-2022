#ifndef __BOMB_H__
#define __BOMB_H__

#include "Headers/main.h"

#include "Headers/Packages.h"

struct WorkingPackage;

//---------------------------------PIN DEFINITION---------------------------------

uint8_t const SENSOR_CURRENT_PIN PROGMEM = A0;
uint8_t const PRIMARY_RELAY_PIN PROGMEM = A1;
uint8_t const EV_RELAY_PIN PROGMEM = A2;
uint8_t const AUXILIAR_RELAY_PIN PROGMEM = A3;

//---------------------------------PIN DEFINITION---------------------------------

//---------------------------------GLOBAL VARIABLES DECLARATION---------------------------------

extern bool bomb_is_working;
extern bool bomb_disable_system;

extern uint8_t fail_start_count;
extern uint8_t fail_stop_count;
extern uint8_t fail_emergency_stop_count;
extern uint32_t current_time;

extern float aux_current;
extern float aux_current_max;
extern float aux_current_min;
extern float efective_current;

extern WorkingPackage working_package;

extern bool send_error_start;
extern bool send_error_stop;
extern bool send_stop;
extern bool send_start;
extern bool send_error_emergency_stop;
extern bool send_error_time_out;
extern bool send_manual_on_ok;
extern bool send_manual_off_ok;
extern bool send_manual_on_err;
extern bool send_manual_off_err;
extern bool send_bomb_alredy_on;
extern bool send_bomb_alredy_off;
extern bool send_system_disable_ok;
extern bool send_system_enable_ok;
extern bool send_system_disable_err;
extern bool send_system_enable_err;
extern bool send_system_actual_disable;
extern bool send_system_actual_enabled;
extern bool send_medium_high;
extern bool send_medium_low;
extern bool send_notification;

//---------------------------------GLOBAL VARIABLES DECLARATION---------------------------------

//---------------------------------CONSTANTS DEFINITION---------------------------------

uint8_t const FAIL_MAX_COUNT PROGMEM = 90;
float const SENSOR_CURRENT_SENSIBILITY PROGMEM = 0.185;
float const SENSOR_CURRENT_OFFSET PROGMEM = 0.15F;
float const CURRENT_LIMIT PROGMEM = 0.7F;

//---------------------------------CONSTANTS DEFINITION---------------------------------

//---------------------------------FUNCTIONS DECLARATION---------------------------------

void bombStart(void);
void bombStop(void);
void bombEmergencyStop(void);
void bombDisableSystem(void);
void bombEnableSystem(void);
void bombControl(void);
void bombManualStart(void);
void bombManualStop(void);
void workingInit(WorkingPackage::WorkingType type);
void workingStop(WorkingPackage::WorkingType type);
void changeState(StatePackage::State bomb_state, StatePackage::State system_state);

//---------------------------------FUNCTIONS DECLARATION---------------------------------

#endif