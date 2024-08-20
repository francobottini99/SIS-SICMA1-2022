#include "Headers/Packages.h"

//------------------------------------------RF24_NOTIFICATION_PACKAGE------------------------------------------

RF24NotificationPackage::RF24NotificationPackage(RF24_Codes code, uint32_t time) {
    this->code = code;
    this->time = time;
}

//------------------------------------------RF24_NOTIFICATION_PACKAGE------------------------------------------

//------------------------------------------PARAMS_PACKAGE------------------------------------------

ParamPackage::ParamPackage(uint16_t stop_level) {
    id_field = 0;
    id_tank = 0;
    cycle = 0;
    start_level = 0;
    this->stop_level = stop_level;
    time_to_confirm_change_level = 10000;
    server_reference_time = 0;
    security_time_start_auto = -1;
    security_time_start_manual = -1;
}

ParamPackage::ParamPackage() {
    id_field = 0;
    id_tank = 0;
    cycle = 0;
    start_level = 0;
    stop_level = 0;
    time_to_confirm_change_level = 10000;
    server_reference_time = 0;
    security_time_start_auto = -1;
    security_time_start_manual = -1;
}

//------------------------------------------PARAMS_PACKAGE------------------------------------------

//------------------------------------------TEMP_PACKAGE------------------------------------------

TempPackage::TempPackage() {
    min_temp = 9999.0f;
    max_temp = -9999.0f;
    act_temp = 0;
    time = 0;
    max_time = 0;
    min_time = 0;
}

//------------------------------------------TEMP_PACKAGE------------------------------------------

//------------------------------------------SENSED_PACKAGE------------------------------------------

SensedPackage::SensedPackage() {
	id_field = 0;
	id_tank = 0;
	cycle = 0;
	time = 0;
	actual_level = 0;
	previus_level = 0;
}

//------------------------------------------SENSED_PACKAGE------------------------------------------

//------------------------------------------WORKING_PACKAGE------------------------------------------

WorkingPackage::WorkingPackage() {
    max_current = -9999.0f;
    min_current = 9999.0f;
    average_current = 0;
    time_init = 0;
    time = 0;
    time_stop = 0;
    level_init = 0;
    level_stop = 0;
    type_init = WT_NULL;
    type_stop = WT_NULL;
}

//------------------------------------------WORKING_PACKAGE------------------------------------------

//------------------------------------------WORKING_PACKAGE------------------------------------------

StatePackage::StatePackage() {
    bomb_state = UNKNOWN;
    system_state = UNKNOWN;
    time = 0;
}

//------------------------------------------STATUS PACKAGE------------------------------------------
