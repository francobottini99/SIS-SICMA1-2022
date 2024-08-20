#ifndef __LEVEL_SENSOR_H__
#define __LEVEL_SENSOR_H__

/*#ifndef CONSOLE_DEBUG
    #define CONSOLE_DEBUG
#endif*/

#ifdef CONSOLE_DEBUG
    #ifndef CONSOLE_PRINT
        #define CONSOLE_PRINT(x) Serial.print(x);
    #endif
#endif

#ifdef CONSOLE_DEBUG
    #ifndef CONSOLE_PRINT_LN
        #define CONSOLE_PRINT_LN(x) Serial.println(x);
    #endif
#endif

#include <Arduino.h>
#include <math.h>

class LevelSensor
{
private:
    uint8_t count;
    uint8_t actual_level;
    uint8_t previus_level;
    uint8_t auxiliar_level;

    uint8_t* measurements_array;
    uint8_t** measurements_bits_table;

    const uint8_t* contol_pins; 
    uint8_t read_pin;

    uint8_t number_of_levels;
    uint8_t number_of_control_bits;

    uint32_t time_level;
    
    void getMeasurementsArray();
public:
    LevelSensor(const uint8_t resolution_bits, const uint8_t* contol_pins, const uint8_t read_pin);
    ~LevelSensor();

    uint8_t getActualLevel();
    uint8_t getPreviusLevel();

    bool changeLevel(uint16_t time_to_confirm);

    #ifdef CONSOLE_DEBUG
        void printMeasurementsArray();
	    void printMeasurementsBitsTable();
    #endif
};

#endif