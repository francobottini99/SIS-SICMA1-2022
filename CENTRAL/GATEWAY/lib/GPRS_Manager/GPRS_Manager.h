#ifndef __GPRS_MANAGER_H__
#define __GPRS_MANAGER_H__

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
#include <GPRS_Shield_Arduino.h>

class GPRS_Manager
{
private:
    uint8_t gprs_tx_pin;
    uint8_t gprs_rx_pin;
    uint8_t gprs_power_pin;
    uint32_t gprs_baud_rate;
    
    int16_t response_size;

    GPRS* gprs_module;
    
    char* apn;
    char* server_ip;
	uint16_t server_port;

    bool powerOn();
	bool powerOff();
	bool initHTTP();
	bool termHTTP();

    void cleanBuffer(char* buffer, uint16_t sizeBuffer);
    bool validateNumber(char* number);
	bool validateIP(char* ip);
public:
    GPRS_Manager(uint8_t gprs_tx_pin, uint8_t gprs_rx_pin, uint8_t gprs_power_pin ,uint32_t gprs_baud_rate);
    ~GPRS_Manager();

    bool setServerIP(char* server_ip);
	bool setServerPort(uint16_t server_port);
    void setApn(char apn[45]);

	char* getServerIP();
	uint16_t getServerPort();
    char* getApn();
    int getSignalStrength(); 

    bool initGPRS();
    bool restartGPRS();

    bool openConn();
    bool closeConn();

    bool deleteAllSms();
    bool deleteSms(char index);

    bool sendSms(const char* msg, const char* phone);

    bool sendSmsAllContacts(const char* msg);

	bool getSms(char* bufferMsg, const uint16_t bufferSizeMsg, char* phone, char* datetime);

    bool sendGET(const __FlashStringHelper* request);
    bool sendPOST(const __FlashStringHelper* request, const char* data);
    bool getResponse(char* buffer, uint16_t bufferSize);
};

#endif