#include "Headers/MemoryControler.h"

void saveSaldoNumber(const char* saldoNumber, uint8_t size) {
    for (int i = 0; saldoNumber[i]; i++) {
		EEPROM.write(DIR_SALDO_NUMBER + i, saldoNumber[i]);
    }

	for (int i = size; i < 15; i++) {
		EEPROM.write(DIR_SALDO_NUMBER + i, 0);
	}
}

void saveSaldoMsg(const char* saldoMsg, uint8_t size) {
	for (int i = 0; saldoMsg[i]; i++) {
		EEPROM.write(DIR_SALDO_MSG + i, saldoMsg[i]);
	}

	for (int i = size; i < 30; i++) {
		EEPROM.write(DIR_SALDO_MSG + i, 0);
	}
}

bool readSaldoNumber(char* saldoNumber) {
	int i = 0;
	while (true) {
		EEPROM.get(DIR_SALDO_NUMBER + i, saldoNumber[i]);
		if (!saldoNumber[i]) { break; }
		i++;
	} 

	if (saldoNumber[0] == 0) {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F("POHONE BALANCE INQUIRY = \"NULL\""));
		#endif

		return false;
	}
	else {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT(F("POHONE BALANCE INQUIRY = \""));
			CONSOLE_PRINT(saldoNumber);
			CONSOLE_PRINT_LN(F("\""));
		#endif
		
		return true;
	}
}

bool readSaldoMsg(char* saldoMsg) {
	int i = 0;
	while (true) {
		EEPROM.get(DIR_SALDO_MSG + i, saldoMsg[i]);
		if (!saldoMsg[i]) { break; }
		i++;
	}

	if (saldoMsg[0] == 0) {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F("MESSAGE BALANCE INQUIRY = \"NULL\""));
		#endif

		return false;
	}
	else {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT(F("MESSAGE BALANCE INQUIRY = \""));
			CONSOLE_PRINT(saldoMsg);
			CONSOLE_PRINT_LN(F("\""));
		#endif

		return true;
	}
}