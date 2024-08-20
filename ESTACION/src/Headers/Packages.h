#ifndef __PACKAGES_H__
#define __PACKAGES_H__

#include <Arduino.h>

#include "Headers/Enums.h"

struct RF24NotificationPackage {
    uint32_t time;
    RF24_Codes code;

    RF24NotificationPackage(RF24_Codes code = RF24_NOTHING, uint32_t time = 0);
};

struct ParamPackage {
    uint8_t start_level;
	uint8_t stop_level;
    uint16_t id_field;
	uint16_t id_tank;
	uint16_t cycle;
	uint16_t time_to_confirm_change_level;
	uint32_t server_reference_time;
    int32_t security_time_start_auto;
    int32_t security_time_start_manual;

    ParamPackage();
    ParamPackage(uint16_t stop_level);
};

struct SensedPackage {
    uint16_t id_field;
	uint16_t id_tank;
	uint16_t cycle;
	uint32_t time;
	uint8_t actual_level;
	uint8_t previus_level; 

    SensedPackage();
};

struct TempPackage{
    float max_temp;
	float min_temp;
	float act_temp;
    uint32_t max_time;
    uint32_t min_time;
	uint32_t time;

    TempPackage();
};

struct WorkingPackage {
    enum WorkingType {
        WT_NULL,
        WT_AUTO,
        WT_MANUAL,
        WT_EMERGENCY,
        WT_DISABLE
    };
    
    float max_current;
    float min_current;
    float average_current;

    uint32_t time_init;
    uint32_t time_stop;
	uint32_t time;

    uint8_t level_init;
    uint8_t level_stop;

    WorkingType type_init;
    WorkingType type_stop;

    WorkingPackage();
};

struct StatePackage {
    enum State {
        UNKNOWN,
        ENABLE,
        DISABLE
    };

    uint32_t time;
    State bomb_state;
    State system_state;

    StatePackage();
};

#endif