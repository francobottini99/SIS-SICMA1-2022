#include "Headers/main.h"

GPRS_Manager gprs_manager(GPRS_TX_PIN, GPRS_RX_PIN, GPRS_POWER_PIN, GPRS_BAUD_RATE);

char buffer_data[200];
char bufffer_phone[16];

char apn[45] = {"NULL"};
char server_ip[16] = {"NULL"};
char saldo_msg[30] = {"NULL"};
char saldo_number[15] = {"NULL"};

char aux_saldo[10] = "...";
uint16_t saldo = 0;

uint16_t server_port = 0;

uint32_t request_saldo_time_out = 0;
uint32_t time_delay = 0;

uint8_t counts_to_reset_gprs = 0;

bool send_disfusion = true;
bool send_user = false;
bool send_start = true;

GatewayNotificationPackage notification_recibe_package;

//---------------------------------WATCH DOG---------------------------------

volatile int wd_counter = 0;
volatile int wd_max_counter = 16; // 8 * wd_max_counter segundos

ISR(WDT_vect) {
    wd_counter++;

    if(wd_counter < wd_max_counter - 1) wdt_reset();
    else {
        MCUSR = 0;
        WDTCSR |= 0b00011000;
        WDTCSR = 0b00001000 | 0b000000;        
    }
}

void watchDogEnabled() {
    wd_counter = 0;
    cli();

    MCUSR = 0;
    WDTCSR |= 0b00011000;
    WDTCSR = 0b01000000 | 0b100001;

    sei();
}

//---------------------------------WATCH DOG---------------------------------

void setup() {
    wdt_disable();

    GatewayNotificationPackage notification_package;
    
    #ifdef CONSOLE_DEBUG
        Serial.begin(SERIAL_BAUD_RATE);
    #endif

    Wire.begin(GATEWAY_ADRRESS);

    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);
    
    notification_package = GatewayNotificationPackage(G_INIT_GPRS);
    sendToGateway(&notification_package);

    while(!gprs_manager.initGPRS()) { 
        notification_package = GatewayNotificationPackage(G_ERR_INIT);
        sendToGateway(&notification_package);
        
        delay(3000); 
        
        notification_package = GatewayNotificationPackage(G_INIT_GPRS);
        sendToGateway(&notification_package);
    }
    
    #ifdef CONSOLE_DEBUG
        CONSOLE_PRINT_LN("");
    #endif

    loadParams();

    gprs_manager.deleteAllSms();

    notification_package = GatewayNotificationPackage(G_INIT_OK);
    sendToGateway(&notification_package);

    request_saldo = true;
}

void loop() {
    watchDogEnabled();

    if(millis() - time_delay > 1000) {
        if(send_user || send_disfusion) {
            smsManager(buffer_data, sizeof(buffer_data), bufffer_phone);
        } else {
            if(!recibeData()) {
                if(!recibeSMS(buffer_data, sizeof(buffer_data), bufffer_phone)) {
                    if(request_saldo) {
                        GatewaySmsPackage sms_to_send(saldo_msg, saldo_number);

                        sendSms(&sms_to_send);

                        request_saldo = false;
                        wait_saldo = true; 
                    }
                }
            }
        }

        time_delay = millis();
    }

    if(wait_saldo && request_saldo_time_out == 0) { 
        request_saldo_time_out = millis(); 
    } else if(wait_saldo && millis() - request_saldo_time_out > 600000) { 
        wait_saldo = false;
        request_saldo_time_out = 0;
    }
} 

void sendSms(GatewaySmsPackage* sms_package) {                     
    bool complete = true;
    GatewayNotificationPackage notification_package;

    while(!gprs_manager.sendSms(sms_package->msg, sms_package->phone)) {
        if(failControl()) { 
            complete = false; 
            break; 
        }
    }

    if(complete) {
        if(strcmp(saldo_msg, sms_package->msg) != 0) { 
            notification_package = GatewayNotificationPackage(G_SMS_SEND, G_PHONE_NUMBER); 
            sendToGateway(&notification_package, sms_package->phone, 16U); 
        } else { 
            notification_package = GatewayNotificationPackage(G_SALDO_CHECK); 
            sendToGateway(&notification_package); 
        }
    }
}

void smsManager(char* buffer, size_t buffer_size, char* phone) {
    GatewaySmsPackage sms_to_send(buffer, phone);

    if(send_disfusion) {
        if(send_sms_saldo_bajo) {
            send_sms_saldo_bajo = false;

            strcpy_P(buffer, SALDO_BAJO);
            strcat(buffer, aux_saldo);
        } else if(send_sms_fail_saldo) {
            send_sms_fail_saldo = false;

            strcpy_P(buffer, ERR_GET_SALDO);
        } else if(send_start) {
            send_start = false;

            strcpy(buffer, "ESTACION DE CONTROL INICIADA");
        }
        
        strcpy_P(sms_to_send.phone, NUM_C);
        sendSms(&sms_to_send);

        //strcpy_P(sms_to_send.phone, NUM_J);
        //sendSms(&sms_to_send);

        if(!wait_saldo) request_saldo = true;

        send_disfusion = false;
    } else if(send_user) {
        if(send_sms_ok) {
            send_sms_ok = false;
            
            strcpy_P(buffer, MSG_OK);
        } else if(send_sms_err) {
            send_sms_err = false;

            strcpy_P(buffer, MSG_ERR);
        } else if(send_sms_saldo_msg) {
            send_sms_saldo_msg = false;

            strcpy(buffer, "SALDO MSG: ");
            strcat(buffer, saldo_msg);
        } else if(send_sms_saldo_num) {
            send_sms_saldo_num = false;

            strcpy(buffer, "SALDO NUMERO: ");
            strcat(buffer, saldo_number);
        } else if(send_sms_saldo) {
            send_sms_saldo = false;

            strcpy(buffer, "SALDO: $ ");
            strcat(buffer, aux_saldo);
        } else if(send_sms_unknown_command) {
            send_sms_unknown_command = false;

            strcpy_P(buffer, UNKNOWN_COMMAND);
        }
        
        sendSms(&sms_to_send);

        if(!wait_saldo) request_saldo = true;

        send_user = false;
    } 
}

bool recibeData() {
    recibePackageFromGateway(&notification_recibe_package);
        
    if(flag) {        
        if(notification_recibe_package.associated_data != G_NOTHING_DATA) {           
            if(notification_recibe_package.associated_data == G_SMS_PACKAGE) {
                recibeBufferFromGateway(buffer_data, sizeof(buffer_data));

                GatewaySmsPackage sms_to_send = GatewaySmsPackage(buffer_data, ";");

                strcpy(buffer_data, sms_to_send.msg);
                strcpy(bufffer_phone, sms_to_send.phone);

                send_user = true;
            } else if(notification_recibe_package.associated_data == G_STRING_MSG) {
                recibeBufferFromGateway(buffer_data, sizeof(buffer_data));

                send_disfusion = true;
            }
        } else {
            if (notification_recibe_package.code == G_GET_SALDO) {
                notification_recibe_package = GatewayNotificationPackage(G_SET_SALDO, G_SALDO_VALUE);
                sendToGateway(&notification_recibe_package, aux_saldo, 10U);
            }
        }

        flag = false;

        return true;
    } else {
        return false;
    }
}

void loadParams() {
    GatewayNotificationPackage notification_package;

    if (!readSaldoNumber(saldo_number)) {
        notification_package = GatewayNotificationPackage(G_ERR_SALDO_NUM);
        sendToGateway(&notification_package);
    }

    if (!readSaldoMsg(saldo_msg)) {
        notification_package = GatewayNotificationPackage(G_ERR_SALDO_MSG);
        sendToGateway(&notification_package);
    }

    #ifdef CONSOLE_DEBUG
        CONSOLE_PRINT_LN("");
    #endif
}

bool failControl() {
    counts_to_reset_gprs++;

    if (counts_to_reset_gprs > LIMIT_CONN_INTENTS) {
        wdt_disable();

        GatewayNotificationPackage notification_package;

        notification_package = GatewayNotificationPackage(G_INIT_GPRS);
        sendToGateway(&notification_package);

        if(!gprs_manager.restartGPRS()) {
            while(!gprs_manager.initGPRS()) { 
                notification_package = GatewayNotificationPackage(G_ERR_INIT);
                sendToGateway(&notification_package);
        
                delay(3000); 
        
                notification_package = GatewayNotificationPackage(G_INIT_GPRS);
                sendToGateway(&notification_package);
            }
        }

        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN("");
        #endif

        notification_package = GatewayNotificationPackage(G_INIT_OK);
        sendToGateway(&notification_package);

        send_disfusion = false;
        send_user = false;

        strcpy(buffer_data, "");
        strcpy(bufffer_phone, "");

        counts_to_reset_gprs = 0;

        return true;
    } else {
        return false;
    }
}