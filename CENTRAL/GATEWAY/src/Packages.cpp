#include "Headers/Packages.h"

//------------------------------------------GATEWAY_SMS_PACKAGE------------------------------------------

GatewaySmsPackage::GatewaySmsPackage(char* msg) {
    this->msg = msg;
    strcpy(phone, "-");
}

GatewaySmsPackage::GatewaySmsPackage(char* msg, char* phone) {
    this->msg = msg;
    strcpy(this->phone, phone);
}

GatewaySmsPackage::GatewaySmsPackage(char* buffer, const char* delimiter) {
    char* token = strtok(buffer, delimiter);
    msg = token;
        
    token = strtok(NULL, delimiter);
    strcpy(phone, token);
}

//------------------------------------------GATEWAY_SMS_PACKAGE------------------------------------------

//------------------------------------------GATEWAY_NOTIFICATION_PACKAGE------------------------------------------

GatewayNotificationPackage::GatewayNotificationPackage(GatewayCodes code, GatewayAssociatedData associated_data, uint8_t nodeId) {
    this->nodeId = nodeId;
    this->code = code;
    this->associated_data = associated_data;      
}

GatewayNotificationPackage::GatewayNotificationPackage(char* buffer, const char* delimiter) {
    char* token = strtok(buffer, delimiter);
    nodeId = atoi(token);

    token = strtok(NULL, delimiter);
    code = (GatewayCodes)atoi(token);
        
    token = strtok(NULL, delimiter);
    associated_data = (GatewayAssociatedData)atoi(token);
}

void GatewayNotificationPackage::copy(GatewayNotificationPackage gateway_notification_package) {
    nodeId = gateway_notification_package.nodeId;
    code = gateway_notification_package.code;
    associated_data = gateway_notification_package.associated_data;
}

void GatewayNotificationPackage::toString(char* buffer, const char* delimiter) {
    char aux_nodeId[4] = "";
    char aux_code[4] = "";
    char aux_associated_data[4] = "";

    itoa(nodeId, aux_nodeId, 10);
    itoa((int)code, aux_code, 10);
    itoa(associated_data, aux_associated_data, 10);

    strcpy(buffer, aux_nodeId);
    strcat(buffer, delimiter);
        
    strcat(buffer, aux_code);
    strcat(buffer, delimiter);
    
    strcat(buffer, aux_associated_data);
}

//------------------------------------------GATEWAY_NOTIFICATION_PACKAGE------------------------------------------
