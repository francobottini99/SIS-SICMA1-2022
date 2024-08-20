#include "Headers/main.h"

char flash_load_buffer[FLASH_LOAD_BUFFER_SIZE];
char buffer_RF24[BUFFER_RF24_SIZE];

bool order_proceess = false;
bool send_manual_on = false;
bool send_manual_off = false;
bool send_enabled = false;
bool send_disabled = false;
bool send_gateway = false;
bool request_level = true;
bool request_info_working = true;
bool request_temp_info = true;
bool request_saldo = true;
bool request_state = true;

uint32_t time_out = 0;
uint32_t time_delay = 0;
uint32_t time_temp_info = 0;
uint32_t time_level_info = 0;
uint32_t time_working_info = 0;
uint32_t time_state_info = 0;

uint16_t saldo = 0;

char aux_saldo[10] = "...";

StatePackage state_package;
SensedPackage sensed_package;
WorkingPackage working_package;

TempPackage temp_packages[NODE_TEMP_SENSORS];
TempPackage result_package;

GatewayNotificationPackage notification_package_RF24;
GatewaySmsPackage  sms_package_RF24;

char buffer_order_phone[16];

//---------------------------------WATCH DOG---------------------------------

volatile int wd_counter = 0;
volatile int wd_max_counter = 5; // 8 * wd_max_counter segundos

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

    #ifdef CONSOLE_DEBUG
        Serial.begin(SERIAL_BAUD_RATE);
    #endif

    #ifdef CONSOLE_DEBUG
        CONSOLE_PRINT_LN(F("INIT MASTER NODE..."));
    #endif

    lcdDisplayEnqueue(lcd_text("", "INCIANDO", "NODO MAESTRO", "", 1500));

    lcd.init();
    lcd.clear();         
    lcd.backlight();

    Wire.begin();
    //SD.begin(SD_CS_PIN);

    if (!rtc.begin()) {
        lcdDisplayEnqueue(lcd_text("", "NO SE INICIALIZO", "EL RELOJ", ""));

        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("NOT BEGIN CLOCK"));
            CONSOLE_PRINT_LN("");
        #endif
    }

  	mesh.setNodeID(NODE_ID);
  	if (!mesh.begin(100, RF24_250KBPS)) {
        lcdDisplayEnqueue(lcd_text("", "NO INICIO", "LA RED MESH", ""));

        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("NOT BEGIN MESH"));
            CONSOLE_PRINT_LN("");
        #endif
    }

    delay(250);

    radio.setPALevel(RF24_PA_MAX);

    lcd.createChar(0, arrow_down);
    lcd.createChar(1, arrow_up);

    lcdDisplayEnqueue(lcd_text("", "NODO MAESTRO", "EN OPERACION", "", 1500));

    #ifdef CONSOLE_DEBUG
        CONSOLE_PRINT_LN(F("MASTER NODE INIT OK"));
        CONSOLE_PRINT_LN("");
    #endif
}

void loop() {
    watchDogEnabled();
 	recibeDataExtern();
    gateway();

    if(millis() - time_delay > 1000) {
        averageTempPackage(temp_packages, &result_package);
        
        sendDataExtern();

        refreshLevelTankInfo(sensed_package, state_package);
        refreshTempInfo(result_package);
        refreshWorkingInfo(working_package);
        refreshState(state_package);
        refreshSaldo(aux_saldo);

        lcdDisplayControl();

        time_delay = millis();
    }

    if(rtc.now().hour() == 0 && rtc.now().minute() < 1 && rtc.now().second() < 10) { 
        temp_packages[0].max_temp = -9999.0f; 
        temp_packages[0].min_temp = 9999.0f; 
        
        result_package.max_temp = -9999.0f;
        result_package.min_temp = 9999.0f;

        time_temp_info = 0;
    }

    if(rtc.now().unixtime() - time_temp_info > 300) { 
        temp_packages[0].act_temp = rtc.getTemperature();

        if(temp_packages[0].act_temp > temp_packages[0].max_temp) {temp_packages[0].max_temp = temp_packages[0].act_temp; temp_packages[0].max_time = rtc.now().unixtime();} 
        if(temp_packages[0].act_temp < temp_packages[0].min_temp) {temp_packages[0].min_temp = temp_packages[0].act_temp; temp_packages[0].min_time = rtc.now().unixtime();} 
        
        temp_packages[0].time = rtc.now().unixtime(); 
        time_temp_info = rtc.now().unixtime(); 
    }

    if(rtc.now().unixtime() - time_state_info > 300) {
        request_state = true;

        time_state_info = rtc.now().unixtime();
    }

    if(rtc.now().unixtime() - time_level_info > 600) {
        request_level = true;

        time_level_info = rtc.now().unixtime();
    }

    if(rtc.now().unixtime() - time_working_info > 600) {
        request_info_working = true;

        time_working_info = rtc.now().unixtime();
    }

    if(order_proceess) {
        if(rtc.now().unixtime() - time_out > 180) {
            if(loadStringFromFlashMemory(flash_load_buffer, G_ERR_ORDER_INDETERMINATE)) {
                sendToGateway(GatewayNotificationPackage(G_RESPONSE_REQUEST, G_SMS_PACKAGE), GatewaySmsPackage(flash_load_buffer, buffer_order_phone));
            }

            order_proceess = false;
        }
    }
}

void sendDataExtern() {
    mesh.update();
	mesh.DHCP();

    if(send_manual_off) {
        if(sendDataRF24('N', RF24NotificationPackage(B_SET_OFF, rtc.now().unixtime()), 1)) {
            send_manual_off = false;
        } else {
            if(rtc.now().unixtime() - time_out > 120) {
                if(loadStringFromFlashMemory(flash_load_buffer, G_ERR_ORDER_PROCESS)) {
                    sendToGateway(GatewayNotificationPackage(G_RESPONSE_REQUEST, G_SMS_PACKAGE), GatewaySmsPackage(flash_load_buffer, buffer_order_phone));
                }
                
                send_manual_off = false;
                order_proceess = false;
            }
        }        
    } else if(send_manual_on) {
        if(sendDataRF24('N', RF24NotificationPackage(B_SET_ON, rtc.now().unixtime()), 1)) {
            send_manual_on = false;
        } else {
            if(rtc.now().unixtime() - time_out > 120) {
                if(loadStringFromFlashMemory(flash_load_buffer, G_ERR_ORDER_PROCESS)) {
                    sendToGateway(GatewayNotificationPackage(G_RESPONSE_REQUEST, G_SMS_PACKAGE), GatewaySmsPackage(flash_load_buffer, buffer_order_phone));
                }

                send_manual_on = false;
                order_proceess = false;
            }
        }    
    } else if(send_enabled) {
        if(sendDataRF24('N', RF24NotificationPackage(B_SET_ENABLE, rtc.now().unixtime()), 1)) {
            send_enabled = false;
        } else {
            if(rtc.now().unixtime() - time_out > 120) {
                if(loadStringFromFlashMemory(flash_load_buffer, G_ERR_ORDER_PROCESS)) {
                    sendToGateway(GatewayNotificationPackage(G_RESPONSE_REQUEST, G_SMS_PACKAGE), GatewaySmsPackage(flash_load_buffer, buffer_order_phone));
                }

                send_enabled = false;
                order_proceess = false;
            }
        }    
    } else if(send_disabled) {
        if(sendDataRF24('N', RF24NotificationPackage(B_SET_DISABLE, rtc.now().unixtime()), 1)) {
            send_disabled = false;
        } else {
            if(rtc.now().unixtime() - time_out > 120) {
                if(loadStringFromFlashMemory(flash_load_buffer, G_ERR_ORDER_PROCESS)) {
                    sendToGateway(GatewayNotificationPackage(G_RESPONSE_REQUEST, G_SMS_PACKAGE), GatewaySmsPackage(flash_load_buffer, buffer_order_phone));
                }

                send_disabled = false;
                order_proceess = false;
            }
        }    
    } else if(request_level) {
        if(sendDataRF24('N', RF24NotificationPackage(B_GET_LEVEL, rtc.now().unixtime()), 1)) {
            request_level = false;
        } 
    } else if(request_info_working) {
        if(sendDataRF24('N', RF24NotificationPackage(B_GET_LAST_WORKING, rtc.now().unixtime()), 1)) {
            request_info_working = false;
        } 
    } else if(request_temp_info) {
        if(sendDataRF24('N', RF24NotificationPackage(B_GET_TEMP, rtc.now().unixtime()), 1)) {
            request_temp_info = false;
        }
    } else if(request_state) {
        if(sendDataRF24('N', RF24NotificationPackage(B_GET_STATE, rtc.now().unixtime()), 1)) {
            request_state = false;
        }
    }
}

void recibeDataExtern() {
    RF24NetworkHeader header;

    mesh.update();
	mesh.DHCP();

    if(network.available()) {
        network.peek(header);

        if(header.type == 'N') {
            RF24NotificationPackage recibe_package;
             
            if(recibeDataRF24(&header, &recibe_package)) {
                char time[6];
                bool disfusion = true;

                if(loadStringFromFlashMemory(flash_load_buffer, recibe_package.code)) {
                    lcdDisplayEnqueue(flash_load_buffer);

                    stringTime(time, recibe_package.time, false);

                    strcat(flash_load_buffer, " (HORA: ");
                    strcat(flash_load_buffer, time);
                    strcat(flash_load_buffer, ")");

                    if(order_proceess) {
                        if(recibe_package.code == B_ERR_ENABLED || recibe_package.code == B_ENABLED || recibe_package.code == B_ACTUAL_ENABLED ||
                           recibe_package.code == B_ERR_DISABLE || recibe_package.code == B_DISABLE || recibe_package.code == B_ACTUAL_DISABLED ||
                           recibe_package.code == B_ERR_START_MANUAL || recibe_package.code == B_START_MANUAL || recibe_package.code == B_ALREDY_ON ||
                           recibe_package.code == B_ERR_STOP_MANUAL || recibe_package.code == B_STOP_MANUAL || recibe_package.code == B_ALREDY_OFF) {
                            order_proceess = false;
                            disfusion = false;
                            notification_package_RF24 = GatewayNotificationPackage(G_RESPONSE_REQUEST, G_SMS_PACKAGE);
                            sms_package_RF24 = GatewaySmsPackage(flash_load_buffer, buffer_order_phone);
                        } 
                    }
                           
                    if(disfusion) {
                        notification_package_RF24 = GatewayNotificationPackage(G_DISFUSION, G_STRING_MSG);
                        strcpy(buffer_RF24, flash_load_buffer);
                    }
                
                    send_gateway = true;
                }
            }
        } else if(header.type == 'S') {
            SensedPackage paquet;

            if(recibeDataRF24(&header, &paquet)) {
                sensed_package = paquet;
            }
        } else if(header.type == 'W') {
            WorkingPackage paquet;

            if(recibeDataRF24(&header, &paquet)) {
                working_package = paquet;
            }
        } else if(header.type == 'T') {
            TempPackage paquet;

            if(recibeDataRF24(&header, &paquet)) {
                temp_packages[1] = paquet;
            }
        } else if(header.type == 'E') { 
            StatePackage paquet;

            if(recibeDataRF24(&header, &paquet)) {
                state_package = paquet;
            }
        } 
    }
}

void gateway() {
    char gateway_recibe = askRecibe();
    char gateway_send = askSend();

    if(gateway_recibe == C_NOTIFICATION) {
        if(send_gateway && !order_proceess) {
            if(notification_package_RF24.associated_data == G_NOTHING_DATA) {
                sendToGateway(notification_package_RF24);
            } else if(notification_package_RF24.associated_data == G_STRING_MSG) { 
                sendToGateway(notification_package_RF24, buffer_RF24);
            } else if(notification_package_RF24.associated_data == G_SMS_PACKAGE) { 
                sendToGateway(notification_package_RF24, sms_package_RF24);
            }

            send_gateway = false; 
        } else if(request_saldo && !order_proceess) {
            sendToGateway(GatewayNotificationPackage(G_GET_SALDO));
            
            request_saldo = false;
        }
    } else if(gateway_recibe == C_BUFFER) {
        sendGatewayBuffer();
    } else if(gateway_send == C_NOTIFICATION) {
        GatewayNotificationPackage recibe_package;

        if(recibePackageFromGateway(&recibe_package, askForLength())) {
            if(recibe_package.nodeId == 0) {
                if(recibe_package.associated_data == G_NOTHING_DATA) {
                    if(loadStringFromFlashMemory(flash_load_buffer, recibe_package.code))
                        lcdDisplayEnqueue(flash_load_buffer, 1500);
                } else if(recibe_package.associated_data == G_PHONE_NUMBER) {
                    char phone_number[16];

                    recibeBufferFromGateway(phone_number, askForLength());
                
                    if(recibe_package.code == G_SET_ON) {
                        if(!order_proceess){
                            strcpy(buffer_order_phone, phone_number);
                            send_manual_on = true;                               
                            order_proceess = true;
                            time_out = rtc.now().unixtime();
                        } else {
                            if(loadStringFromFlashMemory(flash_load_buffer, G_ERR_ORDER_PROCESS))
                                sendToGateway(GatewayNotificationPackage(G_RESPONSE_REQUEST, G_SMS_PACKAGE), GatewaySmsPackage(flash_load_buffer, phone_number));
                            }
                    } else if(recibe_package.code == G_SET_OFF) { 
                        if(!order_proceess){
                            strcpy(buffer_order_phone, phone_number);
                            send_manual_off = true;
                            order_proceess = true;
                            time_out = rtc.now().unixtime();
                        } else {
                            if(loadStringFromFlashMemory(flash_load_buffer, G_ERR_ORDER_PROCESS))
                                sendToGateway(GatewayNotificationPackage(G_RESPONSE_REQUEST, G_SMS_PACKAGE), GatewaySmsPackage(flash_load_buffer, phone_number));
                        }
                    } else if(recibe_package.code == G_SET_ENABLE) { 
                        if(!order_proceess){
                            strcpy(buffer_order_phone, phone_number);
                            send_enabled = true;
                            order_proceess = true;
                            time_out = rtc.now().unixtime();
                        } else {
                            if(loadStringFromFlashMemory(flash_load_buffer, G_ERR_ORDER_PROCESS))
                                sendToGateway(GatewayNotificationPackage(G_RESPONSE_REQUEST, G_SMS_PACKAGE), GatewaySmsPackage(flash_load_buffer, phone_number));
                        }
                    } else if(recibe_package.code == G_SET_DISABLE) { 
                        if(!order_proceess){
                            strcpy(buffer_order_phone, phone_number);
                            send_disabled = true;
                            order_proceess = true;
                            time_out = rtc.now().unixtime();
                        } else {
                            if(loadStringFromFlashMemory(flash_load_buffer, G_ERR_ORDER_PROCESS))
                                sendToGateway(GatewayNotificationPackage(G_RESPONSE_REQUEST, G_SMS_PACKAGE), GatewaySmsPackage(flash_load_buffer, phone_number));
                        }
                    } else if(recibe_package.code == G_GET_TEMP) {
                        sendToGateway(GatewayNotificationPackage(G_RESPONSE_REQUEST, G_SMS_PACKAGE), result_package.toSmsFormat(phone_number));
                    } else if(recibe_package.code == G_GET_LEVEL) {
                        sendToGateway(GatewayNotificationPackage(G_RESPONSE_REQUEST, G_SMS_PACKAGE), sensed_package.toSmsFormat(phone_number, state_package)); 
                    } else if(recibe_package.code == G_GET_LAST) {
                        sendToGateway(GatewayNotificationPackage(G_RESPONSE_REQUEST, G_SMS_PACKAGE), working_package.toSmsFormat(phone_number)); 
                    } else if(recibe_package.code == G_GET_STATE) {
                        sendToGateway(GatewayNotificationPackage(G_RESPONSE_REQUEST, G_SMS_PACKAGE), state_package.toSmsFormat(phone_number)); 
                    } else if(recibe_package.code == G_SMS_SEND || recibe_package.code == G_SMS_RECIBE) { 
                        if(loadStringFromFlashMemory(flash_load_buffer, recibe_package.code)) {
                            strcat(flash_load_buffer, phone_number);
                            lcdDisplayEnqueue(flash_load_buffer, 1500);
                        }
                    }
                } else if(recibe_package.associated_data == G_SALDO_VALUE) {
                    if(recibe_package.code == G_SET_SALDO) {
                        recibeBufferFromGateway(aux_saldo, askForLength());
                        saldo = atoi(aux_saldo);
                        refreshSaldo(aux_saldo);
                    }
                }
            }
        }             
    }        
}

bool loadStringFromFlashMemory(char* buffer, GatewayCodes code) {
    if(code == G_INIT_GPRS) { strcpy_P(buffer, GATEWAY_INIT_GPRS); return true; }
    else if(code == G_INIT_OK) { strcpy_P(buffer, GATEWAY_OK_INIT); return true; }
    else if(code == G_ERR_INIT) { strcpy_P(buffer, GATEWAY_ERR_INIT); return true; }
    else if(code == G_ERR_SALDO_NUM) { strcpy_P(buffer, GATEWAY_ERR_SALDO_NUM); return true; }
    else if(code == G_ERR_SALDO_MSG) { strcpy_P(buffer, GATEWAY_ERR_SALDO_MSG); return true; }
    else if(code == G_SALDO_CHECK) { strcpy_P(buffer, GATEWAY_GET_SALDO); return true; }
    else if(code == G_SALDO_BAJO) { strcpy_P(buffer, GATEWAY_SALDO_BAJO); return true; }
    else if(code == G_ERR_GET_SALDO) { strcpy_P(buffer, GATEWAY_ERR_GET_SALDO); return true; }
    else if(code == G_SMS_SEND) { strcpy_P(buffer, GATEWAY_SMS_SEND); return true; }
    else if(code == G_SMS_RECIBE) { strcpy_P(buffer, GATEWAY_SMS_RECIBE); return true; }
    else if(code == G_ERR_ORDER_PROCESS) { strcpy_P(buffer, GATEWAY_ERR_ORDER_PROCESS); return true; }
    else if(code == G_ERR_ORDER_INDETERMINATE) { strcpy_P(buffer, GATEWAY_ERR_ORDER_INDETERMINATE); return true; }
    else return false;
}

bool loadStringFromFlashMemory(char* buffer, RF24_Codes code) {
    if(code == B_ERR_EMERGENCY_STOP) { strcpy_P(buffer, BOMB_ERR_EMERGENCY_STOP); return true; } 
    else if(code == B_ERR_TIME_OUT) { strcpy_P(buffer, BOMB_ERR_TIME_OUT); return true; }
    else if(code == B_ERR_STOP) { strcpy_P(buffer, BOMB_ERR_STOP); return true; }
    else if(code == B_ERR_START) { strcpy_P(buffer, BOMB_ERR_START); return true; }
    else if(code == B_ERR_START_MANUAL) { strcpy_P(buffer, BOMB_ERR_START_MANUAL); return true; }
    else if(code == B_ERR_STOP_MANUAL) { strcpy_P(buffer, BOMB_ERR_STOP_MANUAL); return true; }
    else if(code == B_ALREDY_ON) { strcpy_P(buffer, BOMB_ALREDY_ON); return true; }
    else if(code == B_ALREDY_OFF) { strcpy_P(buffer, BOMB_ALREDY_OFF); return true; }
    else if(code == B_START) { strcpy_P(buffer, BOMB_START); return true; }
    else if(code == B_STOP) { strcpy_P(buffer, BOMB_STOP); return true; }
    else if(code == B_MEDIUM_LOW) { strcpy_P(buffer, BOMB_MEDIUM_LOW); return true; }
    else if(code == B_MEDIUM_HIGH) { strcpy_P(buffer, BOMB_MEDIUM_HIGH); return true; }
    else if(code == B_START_MANUAL) { strcpy_P(buffer, BOMB_START_MANUAL); return true; }
    else if(code == B_STOP_MANUAL) { strcpy_P(buffer, BOMB_STOP_MANUAL); return true; }
    else if(code == B_ACTUAL_DISABLED) { strcpy_P(buffer, BOMB_ACTUAL_DISABLE); return true; }
    else if(code == B_ACTUAL_ENABLED) { strcpy_P(buffer, BOMB_ACTUAL_ENABLED); return true; }
    else if(code == B_ERR_DISABLE) { strcpy_P(buffer, BOMB_ERR_DISABLED); return true; }
    else if(code == B_ERR_ENABLED) { strcpy_P(buffer, BOMB_ERR_ENABLED); return true; }
    else if(code == B_ENABLED) { strcpy_P(buffer, BOMB_ENABLED); return true; }
    else if(code == B_DISABLE) { strcpy_P(buffer, BOMB_DISABLE); return true; }
    else return false;
}