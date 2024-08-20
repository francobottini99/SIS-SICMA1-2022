#include "GPRS_Manager.h"

GPRS_Manager::GPRS_Manager(uint8_t gprs_tx_pin, uint8_t gprs_rx_pin, uint8_t gprs_power_pin ,uint32_t gprs_baud_rate) {
    this->gprs_tx_pin = gprs_tx_pin;
    this->gprs_rx_pin = gprs_rx_pin;
    this->gprs_power_pin = gprs_power_pin;
    this->gprs_baud_rate = gprs_baud_rate;

	server_port = 0;
    server_ip = nullptr;
    response_size = -1;
    apn = nullptr;

    gprs_module = new GPRS(this->gprs_tx_pin, this->gprs_rx_pin, this->gprs_baud_rate);
}

GPRS_Manager::~GPRS_Manager() {
    delete gprs_module;
}

char* GPRS_Manager::getServerIP()
{
    return server_ip;
}

uint16_t GPRS_Manager::getServerPort()
{
    return server_port;
}

int GPRS_Manager::getSignalStrength()
{
	int signal = -1;
	gprs_module->getSignalStrength(&signal);
	return signal;
}

char* GPRS_Manager::getApn()
{
    return apn;
}

bool GPRS_Manager::setServerIP(char* server_ip)
{
	if(validateIP(server_ip)){
		this->server_ip = server_ip;
		return true;
	} else {
		return false;
	}
}

bool GPRS_Manager::setServerPort(uint16_t server_port)
{
	if (server_port > 0 && server_port < 65536) {
		this->server_port = server_port;
		return true;
	} else {
		return false;
	}
}

void GPRS_Manager::setApn(char apn[45])
{
    this->apn = apn;
}

bool GPRS_Manager::powerOn()
{
	gprs_module->listen();

	#ifdef CONSOLE_DEBUG
		CONSOLE_PRINT_LN(F("POWER ON..."));
	#endif

	if (!gprs_module->checkPowerUp()) {
		gprs_module->powerUpDown(gprs_power_pin);

		if (gprs_module->checkPowerUp()) {
			#ifdef CONSOLE_DEBUG
				CONSOLE_PRINT_LN(F("POWER ON OK"));
			#endif

			return true;
		} else {
			#ifdef CONSOLE_DEBUG
				CONSOLE_PRINT_LN(F("POWER ON FAIL"));
			#endif

			return false;
		}
	} else {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F("POWER ON OK"));
		#endif

		return true;
	}
}

bool GPRS_Manager::powerOff()
{
	gprs_module->listen();

	#ifdef CONSOLE_DEBUG
		CONSOLE_PRINT_LN(F("POWER OFF..."));
	#endif
	
	if (gprs_module->checkPowerUp()) {
		gprs_module->powerUpDown(gprs_power_pin);

		if (gprs_module->checkPowerUp()) {
			#ifdef CONSOLE_DEBUG
				CONSOLE_PRINT_LN(F("POWER OFF FAIL"));
			#endif

			return false;
		} else {
			#ifdef CONSOLE_DEBUG
				CONSOLE_PRINT_LN(F("POWER OFF OK"));
			#endif

			return true;
		}
	} else {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F("POWER OFF OK"));
		#endif

		return true;
	}
}

bool GPRS_Manager::initGPRS()
{
	gprs_module->listen();

	if (powerOn()) {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F("INIT..."));
		#endif

		if(gprs_module->init()) {
			if(gprs_module->isNetworkRegistered()) {
				#ifdef CONSOLE_DEBUG
					CONSOLE_PRINT_LN(F("INIT OK"));
				#endif

				return true;
			} else {
				#ifdef CONSOLE_DEBUG
					CONSOLE_PRINT_LN(F("INIT FAIL"));
				#endif

				return false;
			}
		} else {
			#ifdef CONSOLE_DEBUG
				CONSOLE_PRINT_LN(F("INIT FAIL"));
			#endif
			
			return false;
		}
	} else {
		return false;
	}
}

bool GPRS_Manager::restartGPRS()
{
	gprs_module->listen();

	#ifdef CONSOLE_DEBUG
		CONSOLE_PRINT_LN(F("RESTART..."));
	#endif

	if (powerOff()) {
		return initGPRS();
	}
	else {
		return false;
	}
}

bool GPRS_Manager::sendSms(const char* msg, const char* phone)
{
	gprs_module->listen();
	
	#ifdef CONSOLE_DEBUG
    	CONSOLE_PRINT_LN(F("SEND SMS..."));
	#endif

	if (gprs_module->sendSMS(phone, msg)) {
        #ifdef CONSOLE_DEBUG
			CONSOLE_PRINT(F("SEND MSG: "));
			CONSOLE_PRINT_LN(msg);    
        	CONSOLE_PRINT(F("PHONE: "));
        	CONSOLE_PRINT_LN(phone);    
			CONSOLE_PRINT_LN("");
		#endif

		return true;
	} else {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F("SEND SMS FAIL"));
			CONSOLE_PRINT_LN(F(""));
		#endif

		return false;
	}
}

bool GPRS_Manager::getSms(char* bufferMsg, const uint16_t bufferSizeMsg, char* phone, char* datetime)
{
	gprs_module->listen();

	char messageIndex = gprs_module->isSMSunread();

	if (messageIndex > 0) {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F("SMS READ..."));
		#endif

		cleanBuffer(bufferMsg, bufferSizeMsg);

		uint32_t time_out = millis();

		while(!gprs_module->readSMS(messageIndex, bufferMsg, bufferSizeMsg, phone, datetime) && millis() - time_out < 60000) {
			#ifdef CONSOLE_DEBUG
				CONSOLE_PRINT_LN(F("FAIL SMS READ"));
			#endif

			delay(500);

			#ifdef CONSOLE_DEBUG
				CONSOLE_PRINT_LN("");
				CONSOLE_PRINT_LN(F("SMS READ..."));
			#endif
		} 
		
		if(millis() - time_out < 60000) {
			#ifdef CONSOLE_DEBUG
				CONSOLE_PRINT_LN("");
				CONSOLE_PRINT(F("RECIBE MSG: "));
        		CONSOLE_PRINT_LN(bufferMsg);
        		CONSOLE_PRINT(F("PHONE: "));
        		CONSOLE_PRINT_LN(phone);    
        		CONSOLE_PRINT_LN("");
			#endif

			deleteSms(messageIndex);

			return true;
		} else {	
			deleteSms(messageIndex);

			return false;
		}
	} else {
		return false;
	}
}

bool GPRS_Manager::deleteSms(char index) {
	#ifdef CONSOLE_DEBUG
		CONSOLE_PRINT_LN(F("DELETE MSG..."));
	#endif

	uint32_t time_out = millis();

	while(!gprs_module->deleteSMS(index) && millis() - time_out < 60000) {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F("DELETE ERROR"));
		#endif

		delay(500);

		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F(""));
			CONSOLE_PRINT_LN(F("DELETE MSG..."));
		#endif
	}

	if(millis() - time_out < 60000) {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F("DELETE OK"));
			CONSOLE_PRINT_LN(F(""));
		#endif
		
		return true;
	} else {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F("DELETE ERROR"));
			CONSOLE_PRINT_LN(F(""));
		#endif

		return false;
	}
}

bool GPRS_Manager::deleteAllSms() {
	#ifdef CONSOLE_DEBUG
		CONSOLE_PRINT_LN(F("DELETE ALL SMS..."));
	#endif
	
	uint32_t time_out = millis();

	while(!gprs_module->deleteAllSMS() && millis() - time_out < 60000) {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F("DELETE ERROR"));
		#endif

		delay(500);

		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F(""));
			CONSOLE_PRINT_LN(F("DELETE ALL MSG..."));
		#endif

		return true;
	} 
	
	if(millis() - time_out < 60000) {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F("DELETE ALL COMPLETE"));
			CONSOLE_PRINT_LN(F(""));
		#endif

		return true;
	} else {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F("DELETE ALL FAIL"));
			CONSOLE_PRINT_LN(F(""));
		#endif

		return false;
	}
}

bool GPRS_Manager::sendSmsAllContacts(const char* msg)
{
	char number[16];
	char name[25];
	int* type = nullptr;

	gprs_module->listen();

	int i = 0;
	while (gprs_module->getBookEntry(i, number, type, name)) {
		sendSms(msg, number);
		i++;
	} 
  
  	if (i == 0) {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F("NO CONTACTS IN CHIP")); CONSOLE_PRINT_LN(""); 
		#endif

		return false; 
	}
	else return true;
}

bool GPRS_Manager::openConn()
{
    if(apn != nullptr) {
		gprs_module->listen();

		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F("CONNECTING..."));
		#endif

		if (gprs_module->isCloseBearer()) {
			if (gprs_module->openBearer(apn)) {
				#ifdef CONSOLE_DEBUG
            		CONSOLE_PRINT_LN(F("CONNECT OK"));
            	#endif

				return true;
        	} else {
				#ifdef CONSOLE_DEBUG
            		CONSOLE_PRINT_LN(F("CONNECT FAIL"));
            	#endif

				return false;
			}
		} else {
			#ifdef CONSOLE_DEBUG
				CONSOLE_PRINT_LN(F("CONNECT OK"));
			#endif

			return true;
		}
    } else {
		#ifdef CONSOLE_DEBUG
        	CONSOLE_PRINT_LN(F("NO SET APN"));
        #endif

		return false;
    }
}

bool GPRS_Manager::closeConn()
{
	gprs_module->listen();

	#ifdef CONSOLE_DEBUG
		CONSOLE_PRINT_LN(F("DISCONNECTING..."));
	#endif

	if (gprs_module->isOpenBearer()) {
		if (gprs_module->closeBearer()) {
    	    #ifdef CONSOLE_DEBUG
				CONSOLE_PRINT_LN(F("DISCONNECTED OK"));
    	    #endif

			return true;
 	   } else {
			#ifdef CONSOLE_DEBUG
				CONSOLE_PRINT_LN(F("DISCONNECTED FAIL"));
        	#endif

			return false;
		}
	} else {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F("DISCONNECTED OK"));
		#endif

		return true;
	}
}

bool GPRS_Manager::initHTTP()
{
	gprs_module->listen();

	if (gprs_module->httpInitialize()) {
		return true;
	} else {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F("INIT HTTP SERVICE FAIL"));
		#endif

		return false;
	}
}

bool GPRS_Manager::termHTTP()
{
	gprs_module->listen();
	
	if (gprs_module->httpTerminate()) {
		return true;
	} else {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F("TERM HTTP SERVICE FAIL"));
		#endif

		return false;
	}
}

bool GPRS_Manager::sendGET(const __FlashStringHelper* request)
{
	if(server_ip != nullptr) {
		if(server_port != 0) {
			gprs_module->listen();

			#ifdef CONSOLE_DEBUG
				CONSOLE_PRINT_LN(F("SEND GET..."));
			#endif

			if (gprs_module->isOpenBearer()) {
				if (initHTTP()) {
					response_size = gprs_module->httpSendGetRequest(server_ip, request, server_port);

					if (response_size > -1) {
						#ifdef CONSOLE_DEBUG
							CONSOLE_PRINT_LN(F("SEND GET OK"));
						#endif

						return true;					
					} else {
						#ifdef CONSOLE_DEBUG
							CONSOLE_PRINT_LN(F("SEND GET FAIL"));
						#endif

						termHTTP();
						return false;
					}
				} else {
					return false;
				}
			} else {
				#ifdef CONSOLE_DEBUG
					CONSOLE_PRINT_LN(F("NOT CONNECTED"));
				#endif
				
				return false;
			}
		} else {
			#ifdef CONSOLE_DEBUG
				CONSOLE_PRINT_LN(F("NO SET SERVER PORT"));
			#endif

			return false;
		}
	} else {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F("NO SET SERVER IP"));
		#endif

		return false;
	}
}

bool GPRS_Manager::sendPOST(const __FlashStringHelper* request, const char* data)
{
	if(server_ip != nullptr) {
		if(server_port != 0) {
			gprs_module->listen();
			
			#ifdef CONSOLE_DEBUG
				CONSOLE_PRINT_LN(F("SEND POST..."));
			#endif

			if (gprs_module->isOpenBearer()) {
				if (initHTTP()) {
					int length = 0;

					for (int i = 0; data[i]; i++) {
						length++;
					}

					response_size = gprs_module->httpSendPostRequest(server_ip, request, data, length, server_port);

					if (response_size > 0) {
						#ifdef CONSOLE_DEBUG
							CONSOLE_PRINT_LN(F("SEND POST OK"));
						#endif

						return true;
					} else {
						#ifdef CONSOLE_DEBUG
							CONSOLE_PRINT_LN(F("SEND POST FAIL"));
						#endif

						termHTTP();
						return false;
					}
				} else {
					return false;
				}
			} else {
				#ifdef CONSOLE_DEBUG
					CONSOLE_PRINT_LN(F("NOT CONNECTED"));
				#endif

				return false;
			}
		} else {
			#ifdef CONSOLE_DEBUG
				CONSOLE_PRINT_LN(F("NO SET SERVER PORT"));
			#endif

			return false;
		}
	} else {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F("NO SET SERVER IP"));
		#endif
		
		return false;
	}
}


bool GPRS_Manager::getResponse(char* buffer, uint16_t bufferSize)
{
	if (response_size > 0) {
		if ((uint16_t)response_size + 10 <= bufferSize) {
			gprs_module->listen();

			cleanBuffer(buffer, bufferSize);

			if (gprs_module->httpReadResponseData(buffer, bufferSize)) {
				#ifdef CONSOLE_DEBUG
					CONSOLE_PRINT_LN(F("RESPONSE OK"));
				#endif

				response_size = 0;
				termHTTP();
				return true;
			}
			else {
				#ifdef CONSOLE_DEBUG
					CONSOLE_PRINT_LN(F("RESPONSE FAIL"));
				#endif

				return false;
			}
		}
		else {
			#ifdef CONSOLE_DEBUG
				CONSOLE_PRINT_LN(F("SHORT BUFFER"));
			#endif

			return false;
		}
	}
	else {
		#ifdef CONSOLE_DEBUG
			CONSOLE_PRINT_LN(F("NO RESPONSE PENDING"));
		#endif
		
		return false;
	}
}

void GPRS_Manager::cleanBuffer(char* buffer, uint16_t sizeBuffer) {
    for (uint16_t i = 0; i < sizeBuffer; i++) {
        buffer[i] = '\0';
    }
}

bool GPRS_Manager::validateNumber(char* number)
{
    while (*number) {
        if (!isdigit(*number)) {
            return false;
        }
        number++;
    }
    return true;
}

bool GPRS_Manager::validateIP(char* ip)
{
    int num, dots = 0;
    char* ptr;
	char* ip_copy = new char();

	for (int i = 0; i < 16; i++)
	{
		ip_copy[i] = ip[i];
	}

    if (ip_copy == NULL) { return false; }

    ptr = strtok(ip_copy, ".");

    if (ptr == NULL) { return false; }

    while (ptr) {
        if (!validateNumber(ptr)) { return false; }

        num = atoi(ptr);
        if (num >= 0 && num <= 255) {
            ptr = strtok(NULL, ".");

            if (ptr != NULL) { dots++; }
        }
        else {
            return false;
        }
    }

    if (dots != 3) { return false; }

	delete ip_copy;

    return true;
}