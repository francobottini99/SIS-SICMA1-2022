#include "Headers/LCDControler.h"

//---------------------------------GLOBAL VARIABLES DEFINITION---------------------------------

lcd_text LCD_DEFAULT_TEXT[LCD_DEFAULT_TEXT_SIZE] = {lcd_text("", "ESTACION", "DE CONTROL", "", 1000),
                                                    lcd_text("", "EL RELOJ", "NO ESTA FUNCIONADO", "", 1000),
                                                    lcd_text("REGISTRO TEMP. HOY", "MAX: ", "MIN: ", "ACT: ", -1, 0, 0, 0),
                                                    lcd_text("ESTADO DE TANQUE", "NIV: ", "VOL: ", "TIE: ", -1, 0, 0, 0),
                                                    lcd_text("BOMBA ULT. FUNC.", "INI: ", "FIN: ", "TOT: ", -1, 0, 0, 0),
                                                    lcd_text("BOMBA ULT. FUNC.", "TIPO I.: ", "TIPO F.: ", "LTS TOT: ", -1, 0, 0, 0),
                                                    lcd_text("BOMBA ULT. FUNC.", "MAX: ", "MIN: ", "PRO: ", -1, 0, 0, 0),
                                                    lcd_text("", "PUERTA DE ENLACE", "SALDO: $", "", 1000),
                                                    lcd_text("ESTADO SICMA#001", "", "BOM - ", "SIS - ", -1, 0, 0, 0)};

byte arrow_down[] = { 0x00, 0x0E, 0x0E, 0x0E, 0x1F, 0x0E, 0x04, 0x00 };
byte arrow_up[] = { 0x00, 0x04, 0x0E, 0x1F, 0x0E, 0x0E, 0x0E, 0x00 };

LiquidCrystal_I2C lcd(LCD_ADRRESS, LCD_COLS, LCD_ROWS);

lcd_text lcd_buffer[LCD_BUFFER_TEXT_SIZE];
uint8_t lcd_buffer_address = 0;
uint32_t lcd_text_time = 0;
uint32_t lcd_display_time = 0;
uint8_t lcd_default_text_address = 0; 
bool lcd_next_text = true;

char arrow;

//---------------------------------GLOBAL VARIABLES DEFINITION---------------------------------

void lcdDisplayText(lcd_text text, bool show_icon, bool show_arrow_up, bool show_arrow_down) {
    lcd.clear();

    if(show_icon) {
        lcd.setCursor(1, 1); lcd.write(0); lcd.write(1); lcd.write(2);
        lcd.setCursor(1, 2); lcd.write(3); lcd.write(4); lcd.write(5);
        lcd.setCursor(16, 1); lcd.write(0); lcd.write(1); lcd.write(2);
        lcd.setCursor(16, 2); lcd.write(3); lcd.write(4); lcd.write(5);
    }

    if(text.col[0] == -1) text.col[0] =  (20 - strlen(text.lines[0])) / 2;
    lcd.setCursor(text.col[0], 0);
    lcd.print(text.lines[0]);

    if(text.col[1] == -1) text.col[1] =  (20 - strlen(text.lines[1])) / 2;
    lcd.setCursor(text.col[1], 1);
    lcd.print(text.lines[1]);
    if(show_arrow_down) lcd.write(0);
    if(show_arrow_up) lcd.write(1);

    if(text.col[2] == -1) text.col[2] =  (20 - strlen(text.lines[2])) / 2;
    lcd.setCursor(text.col[2], 2);
    lcd.print(text.lines[2]);

    if(text.col[3] == -1) text.col[3] =  (20 - strlen(text.lines[3])) / 2;
    lcd.setCursor(text.col[3], 3);
    lcd.print(text.lines[3]);
}

void refreshState(StatePackage state_package) {
    char timeDiff[15] = "";

    strcpy(LCD_DEFAULT_TEXT[8].lines[2], "BOM - ");
    strcpy(LCD_DEFAULT_TEXT[8].lines[3], "SIS - ");

    if(state_package.bomb_state == StatePackage::ENABLE) strcat(LCD_DEFAULT_TEXT[8].lines[2], "ENCENDIDA"); 
    else if(state_package.bomb_state == StatePackage::DISABLE) strcat(LCD_DEFAULT_TEXT[8].lines[2], "APAGADA");
    else strcat(LCD_DEFAULT_TEXT[8].lines[2], "...");

    if(state_package.system_state == StatePackage::ENABLE) strcat(LCD_DEFAULT_TEXT[8].lines[3], "HABILITADO"); 
    else if(state_package.system_state == StatePackage::DISABLE) strcat(LCD_DEFAULT_TEXT[8].lines[3], "DESHABILITADO");
    else strcat(LCD_DEFAULT_TEXT[8].lines[3], "...");

    stringTimeDiff(timeDiff, time_last_package, rtc.now().unixtime());

    if(rtc.now().unixtime() - time_last_package < 300) { 
        strcpy(LCD_DEFAULT_TEXT[8].lines[1], "CON");
    } else {
        strcpy(LCD_DEFAULT_TEXT[8].lines[1], "DES");
    } 

    if(rtc.now().unixtime() - time_last_package < 345600) { 
        strcat(LCD_DEFAULT_TEXT[8].lines[1], " - "); 
        strcat(LCD_DEFAULT_TEXT[8].lines[1], timeDiff); 
    }
    else strcat(LCD_DEFAULT_TEXT[8].lines[1], " - INDEFINIDO");
}

void refreshSaldo(const char* saldo) {
    strcpy(LCD_DEFAULT_TEXT[7].lines[2], "SALDO: $ ");
    strcat(LCD_DEFAULT_TEXT[7].lines[2], saldo);
}

void refreshLevelTankInfo(SensedPackage sensed_package, StatePackage state_package) {
    strcpy(LCD_DEFAULT_TEXT[3].lines[1], "NIV: ");
    strcpy(LCD_DEFAULT_TEXT[3].lines[2], "VOL: ");
    strcpy(LCD_DEFAULT_TEXT[3].lines[3], "TIE: ");

    if(sensed_package.time != 0) {
        char timeDiff[15] = "";
        char level[2] = "";
        char vol_max[5] = "";
        char vol_min[5] = "";

        itoa(sensed_package.actual_level, level, 10);
    
        strcat(LCD_DEFAULT_TEXT[3].lines[1], level);       

        if(state_package.bomb_state == StatePackage::ENABLE) { arrow = 'u'; strcat(LCD_DEFAULT_TEXT[3].lines[1], " "); }
        else if(state_package.bomb_state == StatePackage::DISABLE) { arrow = 'd'; strcat(LCD_DEFAULT_TEXT[3].lines[1], " "); }
        else arrow = 'n';

        itoa((sensed_package.actual_level + 1) * 286, vol_max, 10);
        itoa(sensed_package.actual_level * 286, vol_min, 10); 

        strcat(LCD_DEFAULT_TEXT[3].lines[2], vol_min);

        if(sensed_package.actual_level < 7) {
            strcat(LCD_DEFAULT_TEXT[3].lines[2], " - ");
            strcat(LCD_DEFAULT_TEXT[3].lines[2], vol_max);
            strcat(LCD_DEFAULT_TEXT[3].lines[2], " [L]");
        } else {
            strcat(LCD_DEFAULT_TEXT[3].lines[2], " [L]");
        }

        stringTimeDiff(timeDiff, sensed_package.time, rtc.now().unixtime());

        strcat(LCD_DEFAULT_TEXT[3].lines[3], timeDiff);
    } else {
        strcat(LCD_DEFAULT_TEXT[3].lines[1], "...");
        strcat(LCD_DEFAULT_TEXT[3].lines[2], "...");
        strcat(LCD_DEFAULT_TEXT[3].lines[3], "...");
    }
}

void refreshTempInfo(TempPackage temp_package) {
    char max_temp[10] = "";
    char min_temp[10] = "";
    char act_temp[10] = "";
    char time[6];

    strcpy(LCD_DEFAULT_TEXT[2].lines[1], "MAX: ");
    strcpy(LCD_DEFAULT_TEXT[2].lines[2], "MIN: ");
    strcpy(LCD_DEFAULT_TEXT[2].lines[3], "ACT: ");
    
    if(temp_package.time != 0) {
        itoa(temp_package.max_temp, max_temp, 10);
        stringTime(time, temp_package.max_time, false);

        strcat(LCD_DEFAULT_TEXT[2].lines[1], max_temp);
        strcat(LCD_DEFAULT_TEXT[2].lines[1], " [C] H: ");
        strcat(LCD_DEFAULT_TEXT[2].lines[1], time);
    } else strcat(LCD_DEFAULT_TEXT[2].lines[1], "...");

    if(temp_package.time != 0) {
        itoa(temp_package.min_temp, min_temp, 10);
        stringTime(time, temp_package.min_time, false);

        strcat(LCD_DEFAULT_TEXT[2].lines[2], min_temp);
        strcat(LCD_DEFAULT_TEXT[2].lines[2], " [C] H: ");
        strcat(LCD_DEFAULT_TEXT[2].lines[2], time);
    } else strcat(LCD_DEFAULT_TEXT[2].lines[2], "...");

    if(temp_package.time != 0) {
        itoa(temp_package.act_temp, act_temp, 10);
        stringTime(time, temp_package.time, false);

        strcat(LCD_DEFAULT_TEXT[2].lines[3], act_temp);
        strcat(LCD_DEFAULT_TEXT[2].lines[3], " [C] H: ");
        strcat(LCD_DEFAULT_TEXT[2].lines[3], time);
    } else strcat(LCD_DEFAULT_TEXT[2].lines[3], "...");
}

void refreshWorkingInfo(WorkingPackage working_package) {   
    char max_c[8] = "";
    char min_c[8] = "";
    char average_c[8] = "";
    char time[6] = "";
    char timeDiff[15] = "";
    char vol_max[5] = "";
    char vol_min[5] = "";

    strcpy(LCD_DEFAULT_TEXT[4].lines[1], "INI: ");
    strcpy(LCD_DEFAULT_TEXT[4].lines[2], "FIN: ");
    strcpy(LCD_DEFAULT_TEXT[4].lines[3], "TOT: ");

    strcpy(LCD_DEFAULT_TEXT[5].lines[1], "INI: ");
    strcpy(LCD_DEFAULT_TEXT[5].lines[2], "FIN: ");
    strcpy(LCD_DEFAULT_TEXT[5].lines[3], "ABS: ");

    strcpy(LCD_DEFAULT_TEXT[6].lines[1], "MAX: ");
    strcpy(LCD_DEFAULT_TEXT[6].lines[2], "MIN: ");
    strcpy(LCD_DEFAULT_TEXT[6].lines[3], "PRO: ");

    if(working_package.time != 0 && working_package.time_init != 0) {
        stringTime(time, working_package.time_init, false);
        strcat(LCD_DEFAULT_TEXT[4].lines[1], time);
    } else strcat(LCD_DEFAULT_TEXT[4].lines[1], "...");

    if(working_package.time != 0 && working_package.time_init != 0 && working_package.time_stop != 0) {
        stringTime(time, working_package.time_stop, false);
        strcat(LCD_DEFAULT_TEXT[4].lines[2], time);
    } else strcat(LCD_DEFAULT_TEXT[4].lines[2], "...");

    if(working_package.time != 0 && working_package.time_init != 0) {
        if(working_package.time_stop != 0) stringTimeDiff(timeDiff, working_package.time_init, working_package.time_stop);
        else stringTimeDiff(timeDiff, working_package.time_init, rtc.now().unixtime());

        strcat(LCD_DEFAULT_TEXT[4].lines[3], timeDiff);
    } else strcat(LCD_DEFAULT_TEXT[4].lines[3], "...");

    if(working_package.type_init == WorkingPackage::WT_AUTO) strcat(LCD_DEFAULT_TEXT[5].lines[1], "AUTOMATICO");
    else if(working_package.type_init == WorkingPackage::WT_MANUAL) strcat(LCD_DEFAULT_TEXT[5].lines[1], "MANUAL");
    else strcat(LCD_DEFAULT_TEXT[5].lines[1], "...");

    if(working_package.type_stop == WorkingPackage::WT_AUTO) strcat(LCD_DEFAULT_TEXT[5].lines[2], "AUTOMATICO");
    else if(working_package.type_stop == WorkingPackage::WT_MANUAL) strcat(LCD_DEFAULT_TEXT[5].lines[2], "MANUAL");
    else if(working_package.type_stop == WorkingPackage::WT_EMERGENCY) strcat(LCD_DEFAULT_TEXT[5].lines[2], "EMERGENCIA");
    else if(working_package.type_stop == WorkingPackage::WT_DISABLE) strcat(LCD_DEFAULT_TEXT[5].lines[2], "SIS. DESHAB.");
    else strcat(LCD_DEFAULT_TEXT[5].lines[2], "...");

    if(working_package.time != 0 && working_package.time_init != 0 && working_package.time_stop != 0) {
        if(working_package.level_stop - working_package.level_init > 0) {
            itoa((working_package.level_stop - working_package.level_init) * 286, vol_max, 10);
            itoa((working_package.level_stop - working_package.level_init - 1) * 286, vol_min, 10); 
        } else {
            itoa(286, vol_max, 10);
            itoa(0, vol_min, 10); 
        }

        strcat(LCD_DEFAULT_TEXT[5].lines[3], vol_min);

        if(working_package.level_stop - working_package.level_init < 7) {
            strcat(LCD_DEFAULT_TEXT[5].lines[3], " - ");
            strcat(LCD_DEFAULT_TEXT[5].lines[3], vol_max);
            strcat(LCD_DEFAULT_TEXT[5].lines[3], " [L]");
        } else {
            strcat(LCD_DEFAULT_TEXT[5].lines[3], " [L]");
        }
    } else strcat(LCD_DEFAULT_TEXT[5].lines[3], "...");

    if(working_package.time != 0 && working_package.time_init != 0 && working_package.time_stop != 0) {
        toStringFloat(working_package.max_current, max_c);
        
        strcat(LCD_DEFAULT_TEXT[6].lines[1], max_c);
        strcat(LCD_DEFAULT_TEXT[6].lines[1], " [A]");
    } else strcat(LCD_DEFAULT_TEXT[6].lines[1], "...");
    
    if(working_package.time != 0 && working_package.time_init != 0 && working_package.time_stop != 0) {
        toStringFloat(working_package.min_current, min_c);
        
        strcat(LCD_DEFAULT_TEXT[6].lines[2], min_c);
        strcat(LCD_DEFAULT_TEXT[6].lines[2], " [A]");
    } else strcat(LCD_DEFAULT_TEXT[6].lines[2], "...");

    if(working_package.time != 0 && working_package.time_init != 0 && working_package.time_stop != 0) {
        toStringFloat(working_package.average_current, average_c);
        
        strcat(LCD_DEFAULT_TEXT[6].lines[3], average_c);
        strcat(LCD_DEFAULT_TEXT[6].lines[3], " [A]");
    } else strcat(LCD_DEFAULT_TEXT[6].lines[3], "...");
}

void lcdDisplayControl() {  
    if(lcd_next_text) {
        if(lcd_buffer_address > 0) {
            lcdDisplayText(lcd_buffer[0]);
            lcd_display_time = lcd_buffer[0].duration;
            lcdDisplayDequeue();
        } else {
            char time[9], date[11];
            
            stringDate(date);
            stringTime(time);

            strcpy(LCD_DEFAULT_TEXT[1].lines[1], date);
            strcpy(LCD_DEFAULT_TEXT[1].lines[2], time);

            if(lcd_default_text_address == 3 && arrow == 'u') lcdDisplayText(LCD_DEFAULT_TEXT[lcd_default_text_address], false, true);
            else if(lcd_default_text_address == 3 && arrow == 'd') lcdDisplayText(LCD_DEFAULT_TEXT[lcd_default_text_address], false, false, true);
            else lcdDisplayText(LCD_DEFAULT_TEXT[lcd_default_text_address]);
            
            lcd_display_time = LCD_DEFAULT_TEXT[lcd_default_text_address].duration;

            if(lcd_default_text_address < LCD_DEFAULT_TEXT_SIZE - 1) lcd_default_text_address++;
            else lcd_default_text_address = 0;
        }

        lcd_text_time = millis();
        lcd_next_text = false;
    }

    if(millis() - lcd_text_time > lcd_display_time) {
        lcd_next_text = true;
    }
}

void lcdDisplayEnqueue(lcd_text text) {
    if (lcd_buffer_address >= LCD_BUFFER_TEXT_SIZE)
    {
        lcdDisplayDequeue();
    }

    lcd_buffer[lcd_buffer_address] = text;
    lcd_buffer_address++;
}

void lcdDisplayDequeue() {
    for (int i = 1; i < LCD_BUFFER_TEXT_SIZE; i++)
    {
        lcd_buffer[i - 1] = lcd_buffer[i];
    }

    lcd_buffer_address--;
}

void lcdDisplayEnqueue(const char* msg, uint32_t duration, uint16_t carry) {
    char aux_buffer[256];
    char line_0[21] = "";
    char line_1[21] = "";
    char line_2[21] = "";
    char line_3[21] = "";
    strcpy(aux_buffer, msg);
    char *token = strtok(aux_buffer, " ");
    int count = 0;
    int sum = 0;
    bool add = false;
    uint32_t time_out_while = millis();

    while (token != NULL && millis() - time_out_while < 5000) {
        sum += strlen(token);

        if(sum > 20) {
            count++;
            sum = strlen(token);
        }

        if(count == 0) {
            strcat(line_0, token);
        } else if(count == 1) {
            strcat(line_1, token);
        } else if (count == 2) {
            strcat(line_2, token);
        } else if (count == 3) {
            strcat(line_3, token);
        } else {
            add = true;
        }

        token = strtok(NULL, " ");

        if(sum + strlen(token) <= 20 && token != NULL) {
            if(count == 0) {
                strcat(line_0, " ");
            } else if(count == 1) {;
                strcat(line_1, " ");
            } else if (count == 2) {
                strcat(line_2, " ");
            } else if (count == 3) {
                strcat(line_3, " ");
            }

            sum++;
        } else if(token == NULL) {
            add = true;
        }

        if(add) {
            if(line_1[0] == '\0') lcdDisplayEnqueue(lcd_text("", line_0, "", "", duration));
            else if(line_2[0] == '\0') lcdDisplayEnqueue(lcd_text("", line_0, line_1, "", duration));
            else if(line_3[0] == '\0') lcdDisplayEnqueue(lcd_text(line_0, line_1, line_2, "", duration));
            else lcdDisplayEnqueue(lcd_text(line_0, line_1, line_2, line_3, duration));

            strcpy(line_0, "");
            strcpy(line_1, "");
            strcpy(line_2, "");
            strcpy(line_3, "");

            count = 0;
            sum = 0;
            add = false;
        }
    }    
}