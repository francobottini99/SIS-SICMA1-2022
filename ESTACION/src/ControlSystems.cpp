#include "Headers/ContolSystems.h"

//---------------------------------GLOBAL VARIABLES DEFINITION---------------------------------

uint8_t fail_start_count = 0;
uint8_t fail_stop_count = 0;
uint8_t fail_emergency_stop_count = 0;
uint32_t current_time = 0;

float aux_current = 0.0f;
float aux_current_max = 0.0f;
float aux_current_min = 0.0f;
float efective_current = 0.0f;

WorkingPackage working_package;

bool bomb_is_working = false;
bool bomb_disable_system = false;
bool bomb_start_manual = false;

bool send_error_stop = false;
bool send_error_start = false;
bool send_error_emergency_stop = false;
bool send_stop = false;
bool send_start = false;
bool send_error_time_out = false;
bool send_manual_on_ok = false;
bool send_manual_off_ok = false;
bool send_manual_on_err = false;
bool send_manual_off_err = false;
bool send_bomb_alredy_on = false;
bool send_bomb_alredy_off = false;
bool send_system_disable_ok = false;
bool send_system_enable_ok = false;
bool send_system_disable_err = false;
bool send_system_enable_err = false; 
bool send_system_actual_disable = false;
bool send_system_actual_enabled = false;
bool send_medium_high = false;
bool send_medium_low = false;
bool send_notification = false;

uint16_t count = 0;

//---------------------------------GLOBAL VARIABLES DEFINITION---------------------------------

void bombManualStart() {
    if (!bomb_disable_system && !bomb_is_working) {    
        digitalWrite(PRIMARY_RELAY_PIN, LOW);
        
        bomb_is_working = true;
        bomb_start_manual = true;

        changeState(StatePackage::ENABLE, StatePackage::ENABLE);
        workingInit(WorkingPackage::WT_MANUAL);
        
        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("BOMB MANUAL START"));
            CONSOLE_PRINT_LN("");
        #endif
    }
}

void bombManualStop() {
    if (!bomb_disable_system && bomb_is_working) {
        digitalWrite(PRIMARY_RELAY_PIN, HIGH);

        bomb_is_working = false;

        changeState(StatePackage::DISABLE, StatePackage::ENABLE);
        workingStop(WorkingPackage::WT_MANUAL);

        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("BOMB MANUAL STOP"));  
            CONSOLE_PRINT_LN("");
        #endif
    }
}

void bombStart() {
    if (!bomb_disable_system && !bomb_is_working) {
        digitalWrite(PRIMARY_RELAY_PIN, LOW);
        
        bomb_is_working = true;
        bomb_start_manual = false;

        changeState(StatePackage::ENABLE, StatePackage::ENABLE);
        workingInit(WorkingPackage::WT_AUTO);

        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("BOMB START"));
            CONSOLE_PRINT_LN("");
        #endif
    }
}

void bombStop() {
    if (!bomb_disable_system && bomb_is_working) {
        digitalWrite(PRIMARY_RELAY_PIN, HIGH);

        bomb_is_working = false;

        changeState(StatePackage::DISABLE, StatePackage::ENABLE);
        workingStop(WorkingPackage::WT_AUTO);

        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("BOMB STOP"));  
            CONSOLE_PRINT_LN("");
        #endif  
    }
}

void bombEmergencyStop() {
    if (!bomb_disable_system && bomb_is_working) {
        digitalWrite(AUXILIAR_RELAY_PIN, LOW);

        bombDisableSystem();

        workingStop(WorkingPackage::WT_EMERGENCY);

        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("EMERGENCY BOMB STOP"));
            CONSOLE_PRINT_LN(""); 
        #endif
    }
}

void bombDisableSystem() {
    digitalWrite(AUXILIAR_RELAY_PIN, LOW);
    digitalWrite(PRIMARY_RELAY_PIN, HIGH);

    bomb_disable_system = true;

    changeState(StatePackage::DISABLE, StatePackage::DISABLE);

    if(bomb_is_working) {
        bomb_is_working = false;
        workingStop(WorkingPackage::WT_DISABLE);
    }

    #ifdef CONSOLE_DEBUG
        CONSOLE_PRINT_LN(F("CONTROL BOMB DISABLE")); 
        CONSOLE_PRINT_LN("");
    #endif 
}

void bombEnableSystem() {
    digitalWrite(AUXILIAR_RELAY_PIN, HIGH);
    
    bomb_disable_system = false;

    changeState(StatePackage::DISABLE, StatePackage::ENABLE);

    #ifdef CONSOLE_DEBUG
        CONSOLE_PRINT_LN(F("CONTROL BOMB ENABLE")); 
        CONSOLE_PRINT_LN("");
    #endif 
}

void workingInit(WorkingPackage::WorkingType type) {
    working_package.time_init = rtc->now().unixtime();
    working_package.min_current = 9999.0f;
    working_package.max_current = -9999.0f;
    working_package.average_current = 0;
    working_package.time_stop = 0;
    working_package.level_init = level_sensor->getActualLevel();
    working_package.level_stop = 0;
    working_package.type_init = type;
    working_package.type_stop = WorkingPackage::WT_NULL;
    count = 0;
    send_working_info = true;
}

void workingStop(WorkingPackage::WorkingType type) {
    working_package.time_stop = rtc->now().unixtime();
    working_package.average_current /= count; 
    working_package.type_stop = type;
    working_package.level_stop = level_sensor->getActualLevel();
    send_working_info = true;
}

void changeState(StatePackage::State bomb_state, StatePackage::State system_state) {
    state_package.bomb_state = bomb_state;
    state_package.system_state = system_state;
    state_package.time = rtc->now().unixtime();
    send_state_info = true;
}

void bombControl() {  
    float voltage = analogRead(SENSOR_CURRENT_PIN) * (5.0 / 1023.0);

    //aux_current = 0.9 * aux_current + 0.1 * ((voltage - 2.5) / SENSOR_CURRENT_SENSIBILITY); 
    aux_current = (voltage - 2.5) / SENSOR_CURRENT_SENSIBILITY; 

    if(aux_current > aux_current_max) aux_current_max = aux_current;
    if(aux_current < aux_current_min) aux_current_min = aux_current;
    
    if(!bomb_disable_system) {
        if(bomb_is_working) {
           int32_t security_time;

            if(bomb_start_manual) security_time = param_package.security_time_start_manual;
            else security_time = param_package.security_time_start_auto;

            if(security_time > -1) {
                if(rtc->now().unixtime() - working_package.time_init > (uint32_t)security_time) {
                    bombEmergencyStop(); 
                    send_error_time_out = true;

                    #ifdef CONSOLE_DEBUG
                        CONSOLE_PRINT_LN(F("TIME OUT")); 
                        CONSOLE_PRINT_LN(""); 
                    #endif
                }
            }  
        }      
    }

    if(millis() - current_time > 1000) {
        efective_current = ((aux_current_max - aux_current_min) / 2) * 0.707f - SENSOR_CURRENT_OFFSET;

        /*#ifdef CONSOLE_DEBUG 
            CONSOLE_PRINT(F("STATUS SYSTEM: "));
            if(bomb_disable_system) { CONSOLE_PRINT_LN(F("DISABLE")); }
            else { CONSOLE_PRINT_LN(F("ENABLE")); }
            CONSOLE_PRINT(F("STATUS BOMB: "));
            if(!bomb_is_working) { CONSOLE_PRINT_LN(F("DISABLE")); }
            else { CONSOLE_PRINT_LN(F("ENABLE")); }
            CONSOLE_PRINT(F("FAIL EMERGENCY COUNT: "));
            CONSOLE_PRINT_LN(fail_emergency_stop_count);
            CONSOLE_PRINT(F("FAIL START COUNT: "));
            CONSOLE_PRINT_LN(fail_start_count);
            CONSOLE_PRINT(F("FAIL STOP COUNT: "));
            CONSOLE_PRINT_LN(fail_stop_count);
            CONSOLE_PRINT_LN("");   
            CONSOLE_PRINT(F("CURRENT: "));
            CONSOLE_PRINT(efective_current);
            CONSOLE_PRINT_LN(F(" [A]"));
            CONSOLE_PRINT_LN("");
        #endif*/

        if(!bomb_disable_system){
            send_notification = false;

            if(bomb_is_working) {
                if(efective_current < CURRENT_LIMIT) {
                    fail_start_count++;
                } else {
                    fail_start_count = 0;
                }
            } else {
                if(efective_current > CURRENT_LIMIT) {
                    fail_stop_count++;
                } else {
                    fail_stop_count = 0;
                }
            }

            if(fail_start_count > FAIL_MAX_COUNT) {
                bombDisableSystem();
                fail_start_count = 0;
                send_error_start = true;
            }
    
            if(fail_stop_count > FAIL_MAX_COUNT) {
                bombEmergencyStop();
                fail_stop_count = 0;
                send_error_stop = true;
            }
        } else {
            if(!send_notification) {
                if(efective_current > CURRENT_LIMIT) {
                    fail_emergency_stop_count++;
                } else {
                    fail_emergency_stop_count = 0;
                }    
            
                if(fail_emergency_stop_count > FAIL_MAX_COUNT) {
                    send_error_emergency_stop = true;
                    fail_emergency_stop_count = 0;
                    send_notification = true;
                }
            }
        }

        aux_current = 0;
        aux_current_max = 0;
        aux_current_min = 0;
        
        if(bomb_is_working) {
            working_package.average_current += efective_current;

            if(efective_current > working_package.max_current) {working_package.max_current = efective_current;}
            if(efective_current < working_package.min_current) {working_package.min_current = efective_current;}

            count++;
        }

        current_time = millis();
    }
}