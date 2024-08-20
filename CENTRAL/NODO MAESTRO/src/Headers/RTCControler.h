#ifndef __RTC_CONTROLER_H__
#define __RTC_CONTROLER_H__

#include "Headers/main.h"

#include "RTClib.h"

//---------------------------------CONSTANTS DECLARATION---------------------------------

//---------------------------------CONSTANTS DECLARATION---------------------------------

//---------------------------------GLOBAL VARIABLES DECLARATION---------------------------------

extern RTC_DS3231 rtc;

//---------------------------------GLOBAL VARIABLES DECLARATION---------------------------------

//---------------------------------FUNCTIONS DECLARATION---------------------------------

void stringDate(char* date, uint32_t unix_time = rtc.now().unixtime());
void stringTime(char* time, uint32_t unix_time = rtc.now().unixtime(), bool sec = true);
void stringTimeDiff(char* time, uint32_t unix_init_time, uint32_t unix_end_time);

//---------------------------------FUNCTIONS DECLARATION---------------------------------

#endif