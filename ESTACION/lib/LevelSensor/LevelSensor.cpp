#include "LevelSensor.h"

LevelSensor::LevelSensor(const uint8_t number_of_levels, const uint8_t* contol_pins, const uint8_t read_pin) {
    count = 0;
    this->read_pin = read_pin;
    this->number_of_levels = number_of_levels;
    this->contol_pins = contol_pins;

    int i = 0;
    do {
        i++;
    } while(pow(2, i) - this->number_of_levels < 0);

    this->number_of_levels = pow(2, i);
    number_of_control_bits = i;

    measurements_array = new uint8_t[this->number_of_levels];

    measurements_bits_table = new uint8_t*[this->number_of_levels];
    for (int i = 0; i < this->number_of_levels; i++)
    {
        measurements_bits_table[i] = new uint8_t[number_of_control_bits];
    }

    for (int i = number_of_control_bits - 1; i >= 0; i--) {
        int c = 1;
        int val = 0;

        for (int j = 0; j < this->number_of_levels; j++) {
            if (c >= pow(2, i)) {
                c = 0;
                val = (val == 0) ? 1 : 0;
            } 

            measurements_bits_table[j][number_of_control_bits - 1 - i] = val;
            c++;
        }
    }

    if (getActualLevel() == number_of_levels) {
        previus_level = getActualLevel() - 1;
    }
    else {
        previus_level = getActualLevel() + 1;
    }

    auxiliar_level = previus_level; 
    time_level = millis();
}

LevelSensor::~LevelSensor() {
    for (int i = 0; i < number_of_levels; i++)
    {
        delete [] measurements_bits_table[i];
    }
    
    delete [] measurements_bits_table;
    delete [] measurements_array;
}

uint8_t LevelSensor::getActualLevel() {
    actual_level = 0;
    getMeasurementsArray();

    for (int i = number_of_levels - 1; i >= 0; i--) {
        if (measurements_array[i] == 1) {
            actual_level = i + 1;
            break;
        }
    }

    return actual_level;
}

uint8_t LevelSensor::getPreviusLevel() {
    return previus_level;
}

void LevelSensor::getMeasurementsArray() {
    for (int i = 0; i < number_of_levels; i++) {
        for (int j = number_of_control_bits - 1; j >= 0; j--) {
            digitalWrite(contol_pins[number_of_control_bits - 1 - j], measurements_bits_table[i][j]);
        }

        measurements_array[i] = digitalRead(read_pin);
    }
}

bool LevelSensor::changeLevel(uint16_t time_to_confirm) {
    getActualLevel();

    if(actual_level == auxiliar_level) {
        time_level = millis();
    }

    if(millis() - time_level > time_to_confirm) {
        previus_level = auxiliar_level;
        auxiliar_level = actual_level;
        
        return true;
    } else {
        return false;
    }
}

#ifdef CONSOLE_DEBUG
    void LevelSensor::printMeasurementsArray() {
        getMeasurementsArray();

        for (int i = 0; i < number_of_levels; i++) {
            CONSOLE_PRINT(measurements_array[i]);
        }

        CONSOLE_PRINT_LN(F(""));
    }

    void LevelSensor::printMeasurementsBitsTable() {
        for (int i = 0; i < number_of_levels; i++)
        {
            for (int j = 0; j < number_of_control_bits; j++)
            {
                CONSOLE_PRINT(measurements_bits_table[i][j]);
            }

            CONSOLE_PRINT_LN(F(""));
        }

        CONSOLE_PRINT_LN(F(""));
    }
#endif
