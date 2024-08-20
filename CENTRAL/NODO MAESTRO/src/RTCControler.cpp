#include "Headers/RTCControler.h"

RTC_DS3231 rtc;

void stringDate(char* date, uint32_t unix_time) {    
    DateTime date_time(unix_time);

    strcpy(date, "");
    
    if(date_time.day() < 10) {
        date[0] = '0';
    } else {
        date[0] = date_time.day() / 10 + 48;
    }

    date[1] = date_time.day() % 10 + 48;
    date[2] = '/';
      
    if(date_time.month() < 10) {
        date[3] = '0';
    } else {
        date[3] = date_time.month() / 10 + 48;
    }

    date[4] = date_time.month() % 10 + 48;
    date[5] = '/';

    if(date_time.year() < 10) {
        date[6] = '0';
        date[7] = '0';
        date[8] = '0';
        date[9] = date_time.year()  % 10 + 48;
    } else {
        if(date_time.year() < 100) {
            date[6] = '0';
            date[7] = '0';
            date[8] = date_time.year() / 10 + 48;
            date[9] = date_time.year() % 10 + 48;
        } else {
            if(date_time.year() < 1000) {
                date[6] = '0';
                date[7] = date_time.year() / 100 + 48;
                date[8] = (date_time.year() % 100) / 10 + 48;
                date[9] = (date_time.year() % 100) % 10 + 48;
            } else {
                date[6] = date_time.year() / 1000 + 48;
                date[7] = (date_time.year() % 1000) / 100 + 48;
                date[8] = ((date_time.year() % 1000) % 100) / 10 + 48;
                date[9] = ((date_time.year() % 1000) % 100) % 10 + 48;
            }
        }
    }

    date[10] = '\0';
}

void stringTime(char* time, uint32_t unix_time, bool sec) {   
    DateTime date_time(unix_time);
    
    strcpy(time, "");

    if(date_time.hour() < 10) {
        time[0] = '0';
    } else {
        time[0] = date_time.hour() / 10 + 48;
    }

    time[1] = date_time.hour() % 10 + 48;
    time[2] = ':';

    if(date_time.minute() < 10) {
        time[3] = '0';
    } else {
        time[3] = date_time.minute() / 10 + 48;
    }

    time[4] = date_time.minute() % 10 + 48;

    if(sec) {
        time[5] = ':';

        if(date_time.second() < 10) {
            time[6] = '0';
        } else {
            time[6] = date_time.second() / 10 + 48;
        }

        time[7] = date_time.second() % 10 + 48;
        time[8] = '\0';
    } else {
        time[5] = '\0';
    }
}

void stringTimeDiff(char* time, uint32_t unix_init_time, uint32_t unix_end_time) {
    char aux_hours[6] = "";
    char aux_minuts[3] = "";
    char aux_seconds[3] = "";
    
    if(unix_end_time > unix_init_time) {
        uint32_t diff = unix_end_time - unix_init_time;
        uint16_t hours = diff / 3600;
        uint8_t minuts = (diff - hours * 3600) / 60;
        uint8_t seconds = diff - minuts * 60 - hours * 3600;

        itoa(hours, aux_hours, 10);
        itoa(minuts, aux_minuts, 10);
        itoa(seconds, aux_seconds, 10);

        strcpy(time, "");

        if(hours > 0) {
            if(hours < 10) strcat(time, "0");
            strcat(time, aux_hours);
        } else strcat(time, "00");

        strcat(time, ":");

        if(minuts > 0) {
            if(minuts < 10) strcat(time, "0");
            strcat(time, aux_minuts);
        } else strcat(time, "00");

        strcat(time, ":");

        if(seconds > 0) {
            if(seconds < 10) strcat(time, "0");
            strcat(time, aux_seconds);
        } else strcat(time, "00");

        if(hours > 0) strcat(time, " [HOR]");
        else if(minuts > 0) strcat(time, " [MIN]");
        else if(seconds > 0) strcat(time, " [SEC]");
    } else {
        strcpy(time, "00:00:00");
    }
}