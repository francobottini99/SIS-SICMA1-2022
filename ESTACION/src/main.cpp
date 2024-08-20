#include "Headers/main.h"

//---------------------------------GLOBAL VARIABLES DEFINITION---------------------------------

uint16_t cycle = 1;
uint32_t system_time = 0;
uint32_t retry_send_time = 0;

LevelSensor* level_sensor;
RTC_DS3231* rtc;

StatePackage state_package;
TempPackage temp_package;
SensedPackage sendsed_package; 
ParamPackage param_package(SENSOR_NUMBER_LEVELS);

uint32_t time_send_temp_info = 0;

bool send_working_info = false;
bool send_temp_info = false;
bool send_sendsed_package = false;
bool send_state_info = false;

//---------------------------------GLOBAL VARIABLES DEFINITION---------------------------------

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
        CONSOLE_PRINT_LN(F("INIT BOMB CONTROL..."));
    #endif

    pinMode(LEVEL_SENSOR_CONTROL_BITS_PINS[0], OUTPUT);
    pinMode(LEVEL_SENSOR_CONTROL_BITS_PINS[1], OUTPUT);   
    pinMode(LEVEL_SENSOR_CONTROL_BITS_PINS[2], OUTPUT);
    pinMode(DISPLAY_CONTROL_BITS_PINS[0], OUTPUT);
    pinMode(DISPLAY_CONTROL_BITS_PINS[1], OUTPUT);   
    pinMode(DISPLAY_CONTROL_BITS_PINS[2], OUTPUT);
    pinMode(PRIMARY_RELAY_PIN, OUTPUT);
    pinMode(AUXILIAR_RELAY_PIN, OUTPUT);
    pinMode(EV_RELAY_PIN, OUTPUT);
    pinMode(DISPLAY_VALUE_PIN, OUTPUT);

    pinMode(SENSOR_CURRENT_PIN, INPUT);
    pinMode(LEVEL_SENSOR_READ_PIN, INPUT);
    pinMode(MANUAL_MODE_SENSOR_PIN, INPUT);

    digitalWrite(EV_RELAY_PIN, HIGH);
    digitalWrite(PRIMARY_RELAY_PIN, HIGH);
    digitalWrite(AUXILIAR_RELAY_PIN, HIGH);

    level_sensor = new LevelSensor(SENSOR_NUMBER_LEVELS, LEVEL_SENSOR_CONTROL_BITS_PINS, LEVEL_SENSOR_READ_PIN);
    rtc = new RTC_DS3231();

    system_time = millis();
    current_time = millis();
    retry_send_time = millis();

    Wire.begin();

    mesh.setNodeID(NODE_ID);
    mesh.begin(100, RF24_250KBPS);

    radio.setPALevel(RF24_PA_MAX);
    
    if (!rtc->begin()) {
        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("NOT BEGIN CLOCK"));
            CONSOLE_PRINT_LN("");
        #endif
    }

    param_package.id_field = 1;
    param_package.id_tank = 1;
    param_package.stop_level = 7;
    param_package.start_level = 1;
    param_package.server_reference_time = 0;
    param_package.time_to_confirm_change_level = 15000;
    param_package.security_time_start_auto = 1500;
    param_package.security_time_start_manual = 2700;

    changeState(StatePackage::DISABLE, StatePackage::ENABLE);

    #ifdef CONSOLE_DEBUG
        CONSOLE_PRINT_LN("INITI OK");
        CONSOLE_PRINT_LN("");

        CONSOLE_PRINT(F("SIZE OF PACKAGES SENSED: \""));
        CONSOLE_PRINT((int)sizeof(SensedPackage));
        CONSOLE_PRINT_LN(F("\""));
        CONSOLE_PRINT(F("SIZE OF PACKAGES PARAMS: \""));
        CONSOLE_PRINT((int)sizeof(ParamPackage));
        CONSOLE_PRINT_LN(F("\""));
        CONSOLE_PRINT(F("SIZE OF PACKAGES NOTIFICATIONS: \""));
        CONSOLE_PRINT((int)sizeof(RF24NotificationPackage));
        CONSOLE_PRINT_LN(F("\""));
        CONSOLE_PRINT(F("SIZE OF PACKAGES TEMPS: \""));
        CONSOLE_PRINT((int)sizeof(TempPackage));
        CONSOLE_PRINT_LN(F("\""));
        CONSOLE_PRINT(F("SIZE OF PACKAGES WORKINGS: \""));
        CONSOLE_PRINT((int)sizeof(WorkingPackage));
        CONSOLE_PRINT_LN(F("\""));
        CONSOLE_PRINT(F("SIZE OF PACKAGES WORKINGS: \""));
        CONSOLE_PRINT((int)sizeof(StatePackage));
        CONSOLE_PRINT_LN(F("\""));

        CONSOLE_PRINT_LN("");
    #endif

    EEPROM_readCycle(&cycle);
}

void loop() { 
    watchDogEnabled();
    bombControl();

    if ((millis() - system_time) > 1000) {     
        sensingLevel();

        system_time = millis();
    }

    if(rtc->now().hour() == 0 && rtc->now().minute() < 1 && rtc->now().second() < 10) { 
        temp_package.max_temp = -9999.0f; 
        temp_package.min_temp = 9999.0f;
        time_send_temp_info = 0; 
    }

    if(rtc->now().unixtime() - time_send_temp_info > 300) { 
        temp_package.act_temp = rtc->getTemperature();

        if(temp_package.act_temp > temp_package.max_temp) {temp_package.max_temp = temp_package.act_temp; temp_package.max_time = rtc->now().unixtime();} 
        if(temp_package.act_temp < temp_package.min_temp) {temp_package.min_temp = temp_package.act_temp; temp_package.min_time = rtc->now().unixtime();} 
        
        send_temp_info = true;
        temp_package.time = rtc->now().unixtime();
        time_send_temp_info = rtc->now().unixtime(); 
    }

    mesh.update();
    recibeData();

    if ((millis() - retry_send_time) > 1000) {        
        if(connectRF24()) {
            sendData();
        }

        retry_send_time = millis();
    }
}

void sensingLevel() {
    if (level_sensor->changeLevel(param_package.time_to_confirm_change_level)) {       
        send_sendsed_package = true;

        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT(F("LEVEL CHANGE "));
            CONSOLE_PRINT(level_sensor->getPreviusLevel());
            CONSOLE_PRINT(F(" TO "));
            CONSOLE_PRINT_LN(level_sensor->getActualLevel());
        #endif

        sendsed_package.cycle = cycle;
        sendsed_package.id_field = param_package.id_field;
        sendsed_package.id_tank = param_package.id_tank;
        sendsed_package.actual_level = level_sensor->getActualLevel();
        sendsed_package.previus_level = level_sensor->getPreviusLevel();
        sendsed_package.time = rtc->now().unixtime();

        if (level_sensor->getActualLevel() <= param_package.start_level && !bomb_is_working) {
            cycle++;
            EEPROM_saveCycle(&cycle);
            bombStart();      
            send_start = true;
        } else if (level_sensor->getActualLevel() >= param_package.stop_level && bomb_is_working) {
            bombStop();
            send_stop = true;
        } else if (level_sensor->getActualLevel() == 4 && bomb_is_working) {
            //send_medium_high = true;
        } else if (level_sensor->getActualLevel() == 3 && !bomb_is_working) {
            send_medium_low = true;
        }
        
        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN("");
        #endif
    }
}

void recibeData() {
    RF24NetworkHeader header;

    if(network.available()) {
        network.peek(header);

        if(header.type == 'N')
        {
            RF24NotificationPackage paquet;

            if(recibeDataRF24(&header, &paquet)) {
                if(paquet.code == B_GET_LEVEL) send_sendsed_package = true;
                else if(paquet.code == B_GET_LAST_WORKING) send_working_info = true;
                else if(paquet.code == B_GET_TEMP) send_temp_info = true; 
                else if(paquet.code == B_GET_STATE) send_state_info = true; 
                else if(paquet.code == B_SET_ON) {
                    if(!bomb_disable_system) {
                        if(!bomb_is_working) {
                            bombManualStart();
                            
                            if(bomb_is_working) send_manual_on_ok = true;
                            else send_manual_on_err = true;
                        } else send_bomb_alredy_on = true;
                    } else send_system_actual_disable = true;
                } else if(paquet.code == B_SET_OFF) {
                    if(bomb_is_working) {
                        bombManualStop();

                        if(!bomb_is_working) send_manual_off_ok = true;
                        else send_manual_off_err = true;
                    } else send_bomb_alredy_off = true;                
                } else if(paquet.code == B_SET_DISABLE) {
                    if(!bomb_disable_system) {
                        bombDisableSystem();

                        if(bomb_disable_system) send_system_disable_ok = true;
                        else send_system_disable_err = true;
                    } else send_system_actual_disable = true;
                } else if(paquet.code == B_SET_ENABLE) {
                    if(bomb_disable_system) {
                        bombEnableSystem();

                        if(!bomb_disable_system) send_system_enable_ok = true;
                        else send_system_enable_err = true;
                    } else send_system_actual_enabled = true;
                }
            }
        }
    }
}

void sendData() {
    if(send_error_emergency_stop) {
        if (sendDataRF24('N', RF24NotificationPackage(B_ERR_EMERGENCY_STOP, rtc->now().unixtime()))) {
            send_error_emergency_stop = false;
        } 
    } else if(send_error_time_out) {
        if (sendDataRF24('N', RF24NotificationPackage(B_ERR_TIME_OUT, rtc->now().unixtime()))) {
            send_error_time_out = false;
        } 
    } else if(send_error_stop) {
        if (sendDataRF24('N', RF24NotificationPackage(B_ERR_STOP, rtc->now().unixtime()))) {
            send_error_stop = false;
        }  
    } else if(send_error_start) {
        if (sendDataRF24('N', RF24NotificationPackage(B_ERR_START, rtc->now().unixtime()))) {
            send_error_start = false;
        }
    } else if(send_manual_on_err) {
        if (sendDataRF24('N', RF24NotificationPackage(B_ERR_START_MANUAL, rtc->now().unixtime()))) {
            send_manual_on_err = false;
        }
    } else if(send_manual_on_ok) {
        if (sendDataRF24('N', RF24NotificationPackage(B_START_MANUAL, rtc->now().unixtime()))) {
            send_manual_on_ok = false;
        } 
    }  else if(send_manual_off_err) {
        if (sendDataRF24('N', RF24NotificationPackage(B_ERR_STOP_MANUAL, rtc->now().unixtime()))) {
            send_manual_off_err = false;
        } 
    } else if(send_manual_off_ok) {
        if (sendDataRF24('N', RF24NotificationPackage(B_STOP_MANUAL, rtc->now().unixtime()))) {
            send_manual_off_ok = false;
        } 
    } else if(send_bomb_alredy_on) {
        if (sendDataRF24('N', RF24NotificationPackage(B_ALREDY_ON, rtc->now().unixtime()))) {
            send_bomb_alredy_on = false;
        } 
    } else if(send_bomb_alredy_off) {
        if (sendDataRF24('N', RF24NotificationPackage(B_ALREDY_OFF, rtc->now().unixtime()))) {
            send_bomb_alredy_off = false;
        } 
    } else if(send_system_enable_ok) {
        if (sendDataRF24('N', RF24NotificationPackage(B_ENABLED, rtc->now().unixtime()))) {
            send_system_enable_ok = false;
        } 
    } else if(send_system_disable_ok) {
        if (sendDataRF24('N', RF24NotificationPackage(B_DISABLE, rtc->now().unixtime()))) {
            send_system_disable_ok = false;
        } 
    } else if(send_system_enable_err) {
        if (sendDataRF24('N', RF24NotificationPackage(B_ERR_ENABLED, rtc->now().unixtime()))) {
            send_system_enable_err = false;
        } 
    } else if(send_system_disable_err) {
        if (sendDataRF24('N', RF24NotificationPackage(B_ERR_DISABLE, rtc->now().unixtime()))) {
            send_system_disable_err = false;
        } 
    } else if(send_system_actual_enabled) {
        if (sendDataRF24('N', RF24NotificationPackage(B_ACTUAL_ENABLED, rtc->now().unixtime()))) {
            send_system_actual_enabled = false;
        } 
    } else if(send_system_actual_disable) {
        if (sendDataRF24('N', RF24NotificationPackage(B_ACTUAL_DISABLED, rtc->now().unixtime()))) {
            send_system_actual_disable = false;
        } 
    } else if(send_sendsed_package) {
        if(sendDataRF24('S', sendsed_package)) {
            send_sendsed_package = false;
        }
    } else if(send_start) {
        if (sendDataRF24('N', RF24NotificationPackage(B_START, sendsed_package.time))) {
            send_start = false;
        }  
    } else if(send_medium_high) {
        if (sendDataRF24('N', RF24NotificationPackage(B_MEDIUM_HIGH, sendsed_package.time))) {
            send_medium_high = false;
        }   
    } else if(send_stop) {
        if (sendDataRF24('N', RF24NotificationPackage(B_STOP, sendsed_package.time))) {
            send_stop = false;
        }
    } else if(send_medium_low) {
        if (sendDataRF24('N', RF24NotificationPackage(B_MEDIUM_LOW, sendsed_package.time))) {
            send_medium_low = false;
        }    
    } else if(send_working_info) {
        working_package.time = rtc->now().unixtime();

        if(sendDataRF24('W', working_package)) {
            send_working_info = false;
        }
    } else if(send_temp_info) {
        if(sendDataRF24('T', temp_package)) {
            send_temp_info = false;
        }
    } else if(send_state_info) {
        if(sendDataRF24('E', state_package)) {
            send_state_info = false;
        }
    }
}

bool EEPROM_readCycle(uint16_t* cycle) {
	EEPROM.get(EEPROM_DIR_CYCLE, *cycle);

	if (*cycle == 0) {
        #ifdef CONSOLE_DEBUG
		    CONSOLE_PRINT_LN(F("CYCLE N° = \"NULL\""));
		#endif

        return false;
	}
	else {
        #ifdef CONSOLE_DEBUG
		    CONSOLE_PRINT(F("CYCLE N° = \""));
		    CONSOLE_PRINT(*cycle);
		    CONSOLE_PRINT_LN(F("\""));
		#endif
        
        return true;
	}
}

void EEPROM_saveCycle(const uint16_t* cycle) {
	EEPROM.put(EEPROM_DIR_CYCLE, *cycle);
}

void EEPROM_resetCycle() {
	EEPROM.put(EEPROM_DIR_CYCLE, 1);
}