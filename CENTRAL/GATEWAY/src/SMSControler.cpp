#include "Headers/SMSControler.h"

bool request_saldo = false;
bool wait_saldo = false;

uint8_t saldo_fails = 0;

bool send_sms_ok = false;
bool send_sms_err = false;
bool send_sms_saldo = false;
bool send_sms_saldo_num = false;
bool send_sms_saldo_msg = false;
bool send_sms_saldo_bajo = false;
bool send_sms_fail_saldo = false;
bool alredy_send_saldo_bajo = false;
bool send_sms_unknown_command = false;

bool recibeSMS(char* buffer_msg, size_t size_buffer_msg, char* buffer_phone) {
	char dateTime[32];

    if (gprs_manager.getSms(buffer_msg, size_buffer_msg, buffer_phone, dateTime)) {
        GatewayNotificationPackage notification_package(G_SMS_RECIBE, G_PHONE_NUMBER);

        if(strcmp(buffer_phone, saldo_number) != 0 || !wait_saldo) {
            sendToGateway(&notification_package, buffer_phone, 16U);
        }

        delay(1000);
        
        char* split_response = strtok(buffer_msg, "=");
        
        if (strcmp(saldo_number, buffer_phone) == 0 || strcmp("Claro", buffer_phone) == 0 || strcmp("773", buffer_phone) == 0) {
            if(wait_saldo) {
                char ammount[10];
                char prom[10];

                if (getSaldo(buffer_msg, ammount, prom)) {
                    saldo_fails = 0;
                    saldo = 0;

                    if(ammount[0] != 'L' && ammount[0] != 'N') saldo += atoi(ammount);
                    if(prom[0] != 'L' && prom[0] != 'N') saldo += atoi(prom);
                    
                    itoa(saldo, aux_saldo, 10);

                    if(saldo <= 100 && saldo >= 30 && alredy_send_saldo_bajo) {
                        notification_package = GatewayNotificationPackage(G_SALDO_BAJO);
                        sendToGateway(&notification_package);
                        send_sms_saldo_bajo = true;
                        send_disfusion = true;
                        alredy_send_saldo_bajo = false;
                    } else if(saldo > 100) {
                        alredy_send_saldo_bajo = true;
                    }

                    notification_package = GatewayNotificationPackage(G_SET_SALDO, G_SALDO_VALUE);
                    sendToGateway(&notification_package, aux_saldo, 10U);
                } else {
                    saldo_fails++;

                    if(saldo_fails > 20) {
                        notification_package = GatewayNotificationPackage(G_ERR_GET_SALDO);
                        sendToGateway(&notification_package);
                        send_sms_fail_saldo = true;
                        send_disfusion = true;
                        saldo_fails = 0;
                    }
                }
            }
        } else if (strcmp_P(split_response, GET_OK) == 0) {
            send_sms_ok = true;
            send_user = true;
        } else if (strcmp_P(split_response, GET_SALDO_NUM) == 0) {
            send_sms_saldo_num = true;
            send_user = true;
        } else if (strcmp_P(split_response, GET_SALDO_MSG) == 0) {
            send_sms_saldo_msg = true;
            send_user = true;
        } else if (strcmp_P(split_response, GET_SALDO_VALUE) == 0) {
            send_sms_saldo = true;
            send_user = true;
        } else if (strcmp_P(split_response, SET_SALDO_NUM) == 0) {
            split_response = strtok(NULL, "=");

            if(split_response != NULL) {
                saveSaldoNumber(split_response, strlen(split_response)); 
                send_sms_ok = true;
                send_user = true;
                loadParams();
            } else {
                send_sms_err = true;
                send_user = true;
            }
        } else if (strcmp_P(split_response, SET_SALDO_MSG) == 0) {
            split_response = strtok(NULL, "=");

            if(split_response != NULL) {
                saveSaldoMsg(split_response, strlen(split_response)); 
                send_sms_ok = true;
                send_user = true;
                loadParams();
            } else {
                send_sms_err = true;              
                send_user = true;
            }
        } else if (strcmp_P(split_response, GET_TEMP) == 0) {
            notification_package = GatewayNotificationPackage(G_GET_TEMP, G_PHONE_NUMBER);
            sendToGateway(&notification_package, buffer_phone, 16U);
        } else if (strcmp_P(split_response, GET_LEVEL) == 0) {
            notification_package = GatewayNotificationPackage(G_GET_LEVEL, G_PHONE_NUMBER);
            sendToGateway(&notification_package, buffer_phone, 16U);
        } else if (strcmp_P(split_response, GET_LAST_W) == 0) {
            notification_package = GatewayNotificationPackage(G_GET_LAST, G_PHONE_NUMBER);
            sendToGateway(&notification_package, buffer_phone, 16U);
        } else if (strcmp_P(split_response, SET_ON) == 0) {
            notification_package = GatewayNotificationPackage(G_SET_ON, G_PHONE_NUMBER);
            sendToGateway(&notification_package, buffer_phone, 16U);
        } else if (strcmp_P(split_response, SET_OFF) == 0) {
            notification_package = GatewayNotificationPackage(G_SET_OFF, G_PHONE_NUMBER);
            sendToGateway(&notification_package, buffer_phone, 16U);
        } else if (strcmp_P(split_response, SET_ENABLE) == 0) {
            notification_package = GatewayNotificationPackage(G_SET_ENABLE, G_PHONE_NUMBER);
            sendToGateway(&notification_package, buffer_phone, 16U);
        } else if (strcmp_P(split_response, SET_DISABLE) == 0) {
            notification_package = GatewayNotificationPackage(G_SET_DISABLE, G_PHONE_NUMBER);
            sendToGateway(&notification_package, buffer_phone, 16U);
        } else if (strcmp_P(split_response, GET_STATE) == 0) {
            notification_package = GatewayNotificationPackage(G_GET_STATE, G_PHONE_NUMBER);
            sendToGateway(&notification_package, buffer_phone, 16U);
        } else {
            send_sms_unknown_command = true;
            send_user = true;
        }

        return true;
    } else {
        return false;
    }
}

bool getSaldo(const char* msg, char* amount, char* prom) {
    uint8_t count = 0;

    bool not_saldo = true;
    bool get_saldo = false;
    bool get_prom = false;
    bool out_range_saldo = false;
    bool out_range_prom = false;

    for (unsigned int i = 0; i < strlen(msg); i++) {
        if(msg[i] == '$') {
            not_saldo = false;

            for (unsigned int j = i + 1; j < strlen(msg); j++) {
                if(msg[j] == ',' || msg[j] == ' ') {
                    if(!get_saldo && count > 0) {
                        amount[count++] = '\0';
                        get_saldo = true;
                    }
                    else if(!get_prom && count > 0) {
                        prom[count++] = '\0';
                        get_prom = true;
                    }
                    
                    count = 0;
                    break;
                } else {
                    if(count < 10) {
                        if(!get_saldo) amount[count++] = msg[j];
                        else if(!get_prom) prom[count++] = msg[j];
                    } else {
                        if(!get_saldo) out_range_saldo = true;
                        else if(!get_prom) out_range_prom = true;
                        
                        break;
                    }
                }

                if(get_saldo && get_prom) break;
            }
        }
    }

    if(not_saldo) {
        amount[0] = '\0';
        prom[0] = '\0';

        return false;
    }  else if(out_range_saldo) {
        amount[0] = 'L';

        if(out_range_prom) {
            prom[0] = 'L';

            return false;
        } else {
            return true;
        }
    } else if(out_range_prom) {
        prom[0] = 'L';

        if(out_range_saldo) {
            amount[0] = 'L';

            return false;
        } else {
            return true;
        }        
    } else if(!get_saldo) {
        amount[0] = 'N';

        if(!get_prom) {
            prom[0] = 'N';

            return false;
        } else {
            return true;
        }  
    } else if(!get_prom) {
        prom[0] = 'N';

        if(!get_saldo) {
            amount[0] = 'N';

            return false;
        } else {
            return true;
        }  
    } else {
        return true;
    }
}