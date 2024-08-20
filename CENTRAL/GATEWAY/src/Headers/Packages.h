#ifndef __PACKAGES_H__
#define __PACKAGES_H__

#include <Arduino.h>

#include "Headers/Enums.h"

struct GatewaySmsPackage {
    char* msg;
    char phone[16];

    GatewaySmsPackage(char* msg);
    GatewaySmsPackage(char* msg, char* phone);
    GatewaySmsPackage(char* buffer, const char* delimiter);
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

#endif