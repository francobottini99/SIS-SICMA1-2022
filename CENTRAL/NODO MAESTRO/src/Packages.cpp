#include "Headers/Packages.h"

//------------------------------------------GATEWAY_SMS_PACKAGE------------------------------------------

GatewaySmsPackage::GatewaySmsPackage() {
    strcpy(this->msg, "-");
    strcpy(phone, "-");
}

GatewaySmsPackage::GatewaySmsPackage(const char* msg, const char* phone) {
    strcpy(this->msg, msg);
    strcpy(this->phone, phone); 
}

void GatewaySmsPackage::toString(char* buffer, const char* delimiter) {
    strcpy(buffer, msg);
    strcat(buffer, delimiter);
        
    strcat(buffer, phone);
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
    itoa((int)associated_data, aux_associated_data, 10);

    strcpy(buffer, aux_nodeId);
    strcat(buffer, delimiter);
        
    strcat(buffer, aux_code);
    strcat(buffer, delimiter);
    
    strcat(buffer, aux_associated_data);
}

//------------------------------------------GATEWAY_NOTIFICATION_PACKAGE------------------------------------------

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

GatewaySmsPackage TempPackage::toSmsFormat(const char* phone) {
    GatewaySmsPackage sms_package;
    
    char aux_maxTemp[4] = "";
    char aux_minTemp[4] = "";
    char aux_actTemp[4] = "";
    char aux_time[6] = "";
    char aux_maxTime[6] = "";
    char aux_minTime[6] = "";

    strcpy(sms_package.phone, phone);
    
    if(time != 0) {
        itoa(max_temp, aux_maxTemp, 10);
        itoa(min_temp, aux_minTemp, 10);
        itoa(act_temp, aux_actTemp, 10);
        stringTime(aux_time, time, false);
        stringTime(aux_maxTime, max_time, false);
        stringTime(aux_minTime, min_time, false);
    } else {
        strcpy(aux_maxTemp, "...");
        strcpy(aux_minTemp, "...");
        strcpy(aux_actTemp, "...");
        strcpy(aux_maxTime, "...");
        strcpy(aux_minTime, "...");
        strcpy(aux_time, "...");
    }

    strcpy(sms_package.msg, "---TEMPERATURAS HOY---\r\n");
    strcat(sms_package.msg, "MAXIMA: ");
    if(max_temp > 0 && max_temp < 10) strcat(sms_package.msg, " ");
    strcat(sms_package.msg, aux_maxTemp);
    strcat(sms_package.msg, " [C] - HORA: ");
    strcat(sms_package.msg, aux_maxTime);
    strcat(sms_package.msg, "\r\n");
    strcat(sms_package.msg, "MINIMA:  "); 
    if(min_temp > 0 && min_temp < 10) strcat(sms_package.msg, " ");
    strcat(sms_package.msg, aux_minTemp);
    strcat(sms_package.msg, " [C] - HORA: ");
    strcat(sms_package.msg, aux_minTime);
    strcat(sms_package.msg, "\r\n");
    strcat(sms_package.msg, "ACTUAL:  "); 
    if(act_temp > 0 && act_temp < 10) strcat(sms_package.msg, " ");
    strcat(sms_package.msg, aux_actTemp);
    strcat(sms_package.msg, " [C] - HORA: ");
    strcat(sms_package.msg, aux_time);
    strcat(sms_package.msg, " ");

    return sms_package;
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

GatewaySmsPackage SensedPackage::toSmsFormat(const char* phone, StatePackage state_package) {
    GatewaySmsPackage sms_package;

    char aux_actual_level[4] = "";
    char aux_time[6] = "";
    char aux_time_diff[16] = "";
    char aux_vol_max[5] = "";
    char aux_vol_min[5] = "";

    if(time != 0) {
        itoa(actual_level, aux_actual_level, 10);
        stringTime(aux_time, time, false);
        stringTimeDiff(aux_time_diff, time, rtc.now().unixtime());
        itoa((sensed_package.actual_level + 1) * 286, aux_vol_max, 10);
        itoa(sensed_package.actual_level * 286, aux_vol_min, 10); 
    } else {
        strcpy(aux_actual_level, "...");
        strcpy(aux_time, "...");
        strcpy(aux_time_diff, "...");
        strcpy(aux_vol_max, "...");
        strcpy(aux_vol_min, "...");
    }

    strcpy(sms_package.phone, phone);

    strcpy(sms_package.msg, "---ESTADO DEL TANQUE---\r\n");
    strcat(sms_package.msg, "ACTUAL: ");
    strcat(sms_package.msg, aux_actual_level);
    if(state_package.bomb_state == StatePackage::ENABLE) strcat(sms_package.msg, " - [LLENANDO]");
    else if(state_package.bomb_state == StatePackage::DISABLE) strcat(sms_package.msg, " - [VACIANDO]");
    strcat(sms_package.msg, "\r\n");
    strcat(sms_package.msg, "VOLUMEN:  ");
    strcat(sms_package.msg, aux_vol_min);
    if(actual_level < 7) {strcat(sms_package.msg, " - "); strcat(sms_package.msg, aux_vol_max);}
    strcat(sms_package.msg, " [L]");
    strcat(sms_package.msg, "\r\n");
    strcat(sms_package.msg, "HORA:     ");
    strcat(sms_package.msg, aux_time);
    strcat(sms_package.msg, "\r\n");
    strcat(sms_package.msg, "TIEMPO: ");
    strcat(sms_package.msg, aux_time_diff);
    strcat(sms_package.msg, " ");

    return sms_package;
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

GatewaySmsPackage WorkingPackage::toSmsFormat(const char* phone) {   
    GatewaySmsPackage sms_package;

    char aux_time_init[6] = "";
    char aux_time_stop[6] = "";
    char aux_time_diff[16] = "";
    char aux_max_current[8] = "";
    char aux_min_current[8] = "";
    char aux_average_current[8] = "";
    char aux_vol_max[5] = "";
    char aux_vol_min[5] = "";

    strcpy(sms_package.phone, phone);
    
    if(level_init != 0 && level_stop != 0) {
        stringTime(aux_time_init, time_init, false);
        stringTime(aux_time_stop, time_stop, false);
        stringTimeDiff(aux_time_diff, time_init, time_stop);
        toStringFloat(max_current, aux_max_current);
        toStringFloat(min_current, aux_min_current);
        toStringFloat(average_current, aux_average_current);
        
        if(level_stop - level_init > 0) {
            itoa((level_stop - level_init) * 286, aux_vol_max, 10);
            itoa((level_stop - level_init - 1) * 286, aux_vol_min, 10); 
        } else {
            itoa(286, aux_vol_max, 10);
            itoa(0, aux_vol_min, 10); 
        }
    } else {
        strcpy(aux_time_init, "...");
        strcpy(aux_time_stop, "...");
        strcpy(aux_time_diff, "...");
        strcpy(aux_max_current, "...");
        strcpy(aux_min_current, "...");
        strcpy(aux_average_current, "...");
        strcpy(aux_vol_max, "...");
        strcpy(aux_vol_min, "...");
    }

    strcpy(sms_package.msg, "----ULT. ARR.----\r\n");
    strcat(sms_package.msg, "IN: ");
    strcat(sms_package.msg, aux_time_init);
    strcat(sms_package.msg, " - ");
    if(type_init == WT_AUTO) strcat(sms_package.msg, "AUT");
    else if(type_init == WT_MANUAL) strcat(sms_package.msg, "MAN");
    else strcat(sms_package.msg, "...");
    strcat(sms_package.msg, "\r\n");
    strcat(sms_package.msg, "FI: ");
    strcat(sms_package.msg, aux_time_stop);
    strcat(sms_package.msg, " - ");
    if(type_stop == WT_AUTO) strcat(sms_package.msg, "AUT");
    else if(type_stop == WT_MANUAL) strcat(sms_package.msg, "MAN");
    else if(type_stop == WT_EMERGENCY) strcat(sms_package.msg, "EME");
    else if(type_stop == WT_DISABLE) strcat(sms_package.msg, "DES");
    else strcat(sms_package.msg, "...");
    strcat(sms_package.msg, "\r\n");
    strcat(sms_package.msg, "TO: ");
    strcat(sms_package.msg, aux_time_diff);
    strcat(sms_package.msg, "\r\n\r\n");
    strcat(sms_package.msg, "AB: ");
    strcat(sms_package.msg, aux_vol_min);
    if(working_package.level_stop - working_package.level_init < 7) {strcat(sms_package.msg, " - "); strcat(sms_package.msg, aux_vol_max);}
    strcat(sms_package.msg, " [L]");
    strcat(sms_package.msg, "\r\n\r\n");
    strcat(sms_package.msg, "MA: ");
    strcat(sms_package.msg, aux_max_current);
    strcat(sms_package.msg, " [A]\r\n");
    strcat(sms_package.msg, "MI: ");
    strcat(sms_package.msg, aux_min_current);
    strcat(sms_package.msg, " [A]\r\n");
    strcat(sms_package.msg, "PR: ");
    strcat(sms_package.msg, aux_average_current);
    strcat(sms_package.msg, "  [A]");
    strcat(sms_package.msg, " ");

    return sms_package;
}

//------------------------------------------WORKING_PACKAGE------------------------------------------

StatePackage::StatePackage() {
    bomb_state = UNKNOWN;
    system_state = UNKNOWN;
    time = 0;
}

GatewaySmsPackage StatePackage::toSmsFormat(const char* phone) {
    GatewaySmsPackage sms_package;
    
    char timeDiff[15];

    strcpy(sms_package.phone, phone);

    stringTimeDiff(timeDiff, time_last_package, rtc.now().unixtime());

    strcpy(sms_package.msg, "---ESTADO SICMA#001---\r\n");
    strcat(sms_package.msg, "BOMBA: ");
    if(bomb_state == ENABLE) strcat(sms_package.msg, "ENCENDIDA");
    else if(bomb_state == DISABLE) strcat(sms_package.msg, "APAGADA");
    else strcat(sms_package.msg, "...");
    strcat(sms_package.msg, "\r\n");
    strcat(sms_package.msg, "SISTEMA: ");
    if(system_state == ENABLE) strcat(sms_package.msg, "HABILITADO");
    else if(system_state == DISABLE) strcat(sms_package.msg, "DESHABILITADO");
    else strcat(sms_package.msg, "...");
    strcat(sms_package.msg, "\r\n");
    if(rtc.now().unixtime() - time_last_package < 600) strcat(sms_package.msg, "CONECTADO");
    else strcat(sms_package.msg, "DESCONECTADO");
    if(rtc.now().unixtime() - time_last_package < 345600) { strcat(sms_package.msg, " - "); strcat(sms_package.msg, timeDiff); }
    else strcat(sms_package.msg, " - INDEFINIDO");
    strcat(sms_package.msg, " ");

    return sms_package;
}

//------------------------------------------STATUS PACKAGE------------------------------------------

//------------------------------------------STATUS PACKAGE------------------------------------------

//------------------------------------------EXTRA_FUNCTIONS------------------------------------------

void toStringFloat(float number, char* buffer) {
    double intPart;
    double decPart = modf(number, &intPart) * 100;

    char aux_dec[4] = "";
    char aux_int[4] = "";

    itoa(intPart, aux_int, 10);
    itoa(decPart, aux_dec, 10);

    strcat(buffer, aux_int);
    strcat(buffer, ",");
    strcat(buffer, aux_dec);
}

/*TempPackage averageTempPackage(TempPackage* array_packages, uint8_t size_array) {
    TempPackage result_package;
    uint8_t conn = 0;

    result_package.max_temp = 0;
    result_package.min_temp = 0;

    for (unsigned int i = 0; i < size_array; i++) {
        if(array_packages[i].time != 0) {
            result_package.max_temp += array_packages[i].max_temp;
            result_package.min_temp += array_packages[i].min_temp;
            result_package.act_temp += array_packages[i].act_temp;
            result_package.max_time += array_packages[i].max_time;
            result_package.min_time += array_packages[i].min_time;
            result_package.time += array_packages[i].time;
        } else {
            conn++;
        }
    }

    if(size_array - conn > 0) { 
        result_package.max_temp /= (size_array - conn);
        result_package.min_temp /= (size_array - conn);
        result_package.act_temp /= (size_array - conn);
        result_package.max_time /= (size_array - conn);
        result_package.min_time /= (size_array - conn);
        result_package.time /= (size_array - conn);
    } else {
        result_package.max_temp = -9999.0f;
        result_package.min_temp = 9999.0f;
        result_package.act_temp = 0;
        result_package.max_time = 0;
        result_package.min_time = 0;
        result_package.time = 0;
    }

    return result_package;
}*/

void averageTempPackage(struct TempPackage* array_packages, struct TempPackage* result_package) {   
    if(array_packages[1].time == 0) return;

    double x = (double)rtc.now().hour() / 10.0 + (double)rtc.now().minute() / 600.0;
    double inside_weight = 0.4 * exp(-(pow(x - 1.55, 2.0) / 0.07)) + 0.7 * exp(-(pow(x - 1.1, 2.0) / 0.07));  
    double outside_weight = 1 - inside_weight; 

    result_package->act_temp = inside_weight * ((double)array_packages[0].act_temp) - 3.0 + outside_weight * ((double)array_packages[1].act_temp - 5.0);
    result_package->time = inside_weight * (double)array_packages[0].time + outside_weight * (double)array_packages[1].time;

    if(result_package->max_temp < result_package->act_temp) {
        result_package->max_temp = result_package->act_temp;
        result_package->max_time = result_package->time;
    }
    
    if(result_package->min_temp > result_package->act_temp) {
        result_package->min_temp = result_package->act_temp;
        result_package->min_time = result_package->time;
    }
}

//------------------------------------------EXTRA_FUNCTIONS------------------------------------------