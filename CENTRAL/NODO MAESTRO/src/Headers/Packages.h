#ifndef __PACKAGES_H__
#define __PACKAGES_H__

#include <Arduino.h>

#include "Headers/RTCControler.h"
#include "Headers/Enums.h"

//struct TempPackage averageTempPackage(TempPackage* array_packages, uint8_t size_array);
void averageTempPackage(struct TempPackage* array_packages, struct TempPackage* result_package);

void toStringFloat(float number, char* buffer);

struct GatewaySmsPackage {
    char msg[160];
    char phone[16];

    GatewaySmsPackage();
    GatewaySmsPackage(const char* msg, const char* phone);

    void toString(char* buffer, const char* delimiter);
};

struct GatewayNotificationPackage {
    uint8_t nodeId;
    GatewayCodes code;
    GatewayAssociatedData associated_data;

    GatewayNotificationPackage(GatewayCodes code = G_NOTHING_CODE, GatewayAssociatedData associated_data = G_NOTHING_DATA, uint8_t nodeId = 0);
    GatewayNotificationPackage(char* buffer, const char* delimiter);

    void copy(GatewayNotificationPackage gateway_notification_package);
    void toString(char* buffer, const char* delimiter);
};

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

    GatewaySmsPackage toSmsFormat(const char* phone);
};

struct SensedPackage {
    uint16_t id_field;
	uint16_t id_tank;
	uint16_t cycle;
	uint32_t time;
	uint8_t actual_level;
	uint8_t previus_level; 

    SensedPackage();

    GatewaySmsPackage toSmsFormat(const char* phone, StatePackage state_package);
};

struct TempPackage{
    float max_temp;
	float min_temp;
	float act_temp;
    uint32_t max_time;
    uint32_t min_time;
	uint32_t time;

    TempPackage();

    GatewaySmsPackage toSmsFormat(const char* phone);
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

    GatewaySmsPackage toSmsFormat(const char* phone);
};

#endif