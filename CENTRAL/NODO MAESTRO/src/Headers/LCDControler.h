#ifndef __LCD_CONTROLER_H__
#define __LCD_CONTROLER_H__

#include "Headers/main.h"

#include "LiquidCrystal_I2C.h"
#include "Headers/Packages.h"

struct WorkingPackage;
struct SensedPackage;
struct TempPackage;
struct StatePackage;

//---------------------------------TYPE DECLARATION---------------------------------

typedef char lcd_line[21];

struct lcd_text {
    lcd_line lines[4];
    int8_t col[4];
    uint32_t duration;

    lcd_text() {
        strcpy(lines[0], "");
        strcpy(lines[1], "");
        strcpy(lines[2], "");
        strcpy(lines[3], "");
        col[0] = -1;
        col[1] = -1;
        col[2] = -1;
        col[3] = -1;   
        duration = 5000;    
    }

    lcd_text(const char* line_0, const char* line_1, const char* line_2, const char* line_3, uint32_t duration = 5000) {
        strcpy(lines[0], line_0);
        strcpy(lines[1], line_1);
        strcpy(lines[2], line_2);
        strcpy(lines[3], line_3);
        col[0] = -1;
        col[1] = -1;
        col[2] = -1;
        col[3] = -1;
        this->duration = duration;
    }
    
    lcd_text(const char* line_0, const char* line_1, const char* line_2, const char* line_3, int8_t col_0, int8_t col_1, int8_t col_2, int8_t col_3, uint32_t duration = 5000) {
        strcpy(lines[0], line_0);
        strcpy(lines[1], line_1);
        strcpy(lines[2], line_2);
        strcpy(lines[3], line_3);
        col[0] = col_0;
        col[1] = col_1;
        col[2] = col_2;
        col[3] = col_3;
        this->duration = duration;
    }
};

//---------------------------------TYPE DECLARATION---------------------------------

//---------------------------------CONSTANTS DECLARATION---------------------------------

uint8_t const LCD_ADRRESS PROGMEM = 0x27;
uint8_t const LCD_ROWS PROGMEM = 4;
uint8_t const LCD_COLS PROGMEM = 20;
uint8_t const LCD_BUFFER_TEXT_SIZE PROGMEM = 10;
uint8_t const LCD_DEFAULT_TEXT_SIZE PROGMEM = 9;

//---------------------------------CONSTANTS DECLARATION---------------------------------

//---------------------------------GLOBAL VARIABLES DECLARATION---------------------------------

extern lcd_text LCD_DEFAULT_TEXT[];

extern byte arrow_down[];
extern byte arrow_up[];

extern LiquidCrystal_I2C lcd;

extern lcd_text lcd_buffer[];
extern uint8_t lcd_buffer_address;
extern uint32_t lcd_text_time;
extern uint32_t lcd_display_time;
extern uint8_t lcd_default_text_address; 
extern bool lcd_next_text;

//---------------------------------GLOBAL VARIABLES DECLARATION---------------------------------

//---------------------------------FUNCTIONS DECLARATION---------------------------------

void lcdDisplayText(lcd_text text, bool show_icon = false, bool show_arrow_up = false, bool show_arrow_down = false);
void lcdDisplayControl(void);
void lcdDisplayEnqueue(lcd_text text);
void lcdDisplayDequeue(void);
void lcdDisplayEnqueue(const char* msg, uint32_t duration = 5000, uint16_t carry = 0);
void cleanBuffer(char* buffer, uint16_t sizeBuffer);
void refreshLevelTankInfo(SensedPackage sensed_package, StatePackage state_package);
void refreshWorkingInfo(WorkingPackage working_package);
void refreshTempInfo(TempPackage temp_package);
void refreshSaldo(const char* saldo);
void refreshState(StatePackage state_package);

//---------------------------------FUNCTIONS DECLARATION---------------------------------

#endif