#include "Headers/NodeControler.h"

//---------------------------------GLOBAL VARIABLES DEFINITION---------------------------------

RF24 radio(NRF24L01_CE_PIN, NRF24L01_CSN_PIN);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

uint32_t time_out_reset = 0;

//---------------------------------GLOBAL VARIABLES DEFINITION---------------------------------

bool recibeDataRF24(RF24NetworkHeader* header, RF24NotificationPackage* paquet) {
    if(network.read(*header, paquet, sizeof(RF24NotificationPackage)) > 0) {
        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F(""));
            CONSOLE_PRINT_LN(F("--[RECIBE PAQUET]--"));
            CONSOLE_PRINT_LN(F("DATA: <RF24NotificationPackage> {"));
            CONSOLE_PRINT(F("   code: "));
            CONSOLE_PRINT_LN((int)paquet->code);
            CONSOLE_PRINT(F("   time: "));
            CONSOLE_PRINT_LN(paquet->time);
            CONSOLE_PRINT_LN(F("}"));
            CONSOLE_PRINT(F("FROM ADDRESS: "));
            CONSOLE_PRINT_LN(header->from_node);
            CONSOLE_PRINT(F("HEADER: "));
            CONSOLE_PRINT_LN((char)header->type);
            CONSOLE_PRINT_LN(F(""));
        #endif

        return true;
    } else {
        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("FAIL DATA READ"));
            CONSOLE_PRINT_LN(F(""));
        #endif

        return false;
    }
}

bool sendDataRF24(const unsigned char header, SensedPackage paquet, const uint8_t node) {
    #ifdef CONSOLE_DEBUG
        CONSOLE_PRINT_LN(F("SEND PAQUET..."));
    #endif

    if (!mesh.write(&paquet, header, sizeof(SensedPackage), node)) {
        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("FAIL SEND PAQUET"));
            CONSOLE_PRINT_LN(F(""));
        #endif

        return false;
    } else {
        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("--[SEND PAQUET]--"));
            CONSOLE_PRINT_LN(F("DATA: <SensedPaquet> {"));
            CONSOLE_PRINT(F("   idTank: "));
            CONSOLE_PRINT_LN(paquet.id_tank);
            CONSOLE_PRINT(F("   idField: "));
            CONSOLE_PRINT_LN(paquet.id_field);
            CONSOLE_PRINT(F("   cycle: "));
            CONSOLE_PRINT_LN(paquet.cycle);
            CONSOLE_PRINT(F("   time: "));
            CONSOLE_PRINT_LN(paquet.time);
            CONSOLE_PRINT(F("   previusLevel: "));
            CONSOLE_PRINT_LN(paquet.previus_level);
            CONSOLE_PRINT(F("   actualLevel: "));
            CONSOLE_PRINT_LN(paquet.actual_level);
            CONSOLE_PRINT_LN(F("}"));
            CONSOLE_PRINT(F("TO NODE: "));
            CONSOLE_PRINT_LN(node);
            CONSOLE_PRINT(F("HEADER: "));
            CONSOLE_PRINT_LN((char)header);
            CONSOLE_PRINT_LN(F(""));
        #endif

        return true;
    }
}

bool sendDataRF24(const unsigned char header, StatePackage paquet, const uint8_t node) {
    #ifdef CONSOLE_DEBUG
        CONSOLE_PRINT_LN(F("SEND PAQUET..."));
    #endif

    if (!mesh.write(&paquet, header, sizeof(StatePackage), node)) {
        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("FAIL SEND PAQUET"));
            CONSOLE_PRINT_LN(F(""));
        #endif

        return false;
    } else {
        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("--[SEND PAQUET]--"));
            CONSOLE_PRINT_LN(F("DATA: <StatePackage> {"));
            CONSOLE_PRINT(F("   bombState: "));
            CONSOLE_PRINT_LN((int)paquet.bomb_state);
            CONSOLE_PRINT(F("   systemState: "));
            CONSOLE_PRINT_LN((int)paquet.system_state);
            CONSOLE_PRINT(F("   time: "));
            CONSOLE_PRINT_LN(paquet.time);
            CONSOLE_PRINT_LN(F("}"));
            CONSOLE_PRINT(F("TO NODE: "));
            CONSOLE_PRINT_LN(node);
            CONSOLE_PRINT(F("HEADER: "));
            CONSOLE_PRINT_LN((char)header);
            CONSOLE_PRINT_LN(F(""));
        #endif

        return true;
    }
}

bool sendDataRF24(const unsigned char header, RF24NotificationPackage paquet, const uint8_t node) {
    #ifdef CONSOLE_DEBUG
        CONSOLE_PRINT_LN(F("SEND PAQUET..."));
    #endif

    if (!mesh.write(&paquet, header, sizeof(RF24NotificationPackage), node)) {
        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("FAIL SEND PAQUET"));
            CONSOLE_PRINT_LN(F(""));
        #endif

        return false;
    } else {
        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("--[SEND PAQUET]--"));
            CONSOLE_PRINT_LN(F("DATA: <RF24NotificationPackage> {"));
            CONSOLE_PRINT(F("   code: "));
            CONSOLE_PRINT_LN((int)paquet.code);
            CONSOLE_PRINT(F("   time: "));
            CONSOLE_PRINT_LN(paquet.time);
            CONSOLE_PRINT_LN(F("}"));
            CONSOLE_PRINT(F("TO NODE: "));
            CONSOLE_PRINT_LN(node);
            CONSOLE_PRINT(F("HEADER: "));
            CONSOLE_PRINT_LN((char)header);
            CONSOLE_PRINT_LN(F(""));
        #endif

        return true;
    }
}

bool sendDataRF24(const unsigned char header, TempPackage paquet, const uint8_t node) {
    #ifdef CONSOLE_DEBUG
        CONSOLE_PRINT_LN(F("SEND PAQUET..."));
    #endif

    if (!mesh.write(&paquet, header, sizeof(TempPackage), node)) {
        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("FAIL SEND PAQUET"));
            CONSOLE_PRINT_LN(F(""));
        #endif

        return false;
    } else {
        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("--[SEND PAQUET]--"));
            CONSOLE_PRINT_LN(F("DATA: <TempPackage> {"));
            CONSOLE_PRINT(F("   maxTemp: "));
            CONSOLE_PRINT_LN(paquet.max_temp);
            CONSOLE_PRINT(F("   minTemp: "));
            CONSOLE_PRINT_LN(paquet.min_temp);
            CONSOLE_PRINT(F("   actTemp: "));
            CONSOLE_PRINT_LN(paquet.act_temp);
            CONSOLE_PRINT(F("   time: "));
            CONSOLE_PRINT_LN(paquet.time);
            CONSOLE_PRINT_LN(F("}"));
            CONSOLE_PRINT(F("TO NODE: "));
            CONSOLE_PRINT_LN(node);
            CONSOLE_PRINT(F("HEADER: "));
            CONSOLE_PRINT_LN((char)header);
            CONSOLE_PRINT_LN(F(""));
        #endif

        return true;
    }
}

bool sendDataRF24(const unsigned char header, WorkingPackage paquet, const uint8_t node) {
    #ifdef CONSOLE_DEBUG
        CONSOLE_PRINT_LN(F("SEND PAQUET..."));
    #endif
    
    if (!mesh.write(&paquet, header, sizeof(WorkingPackage), node)) {
        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("FAIL SEND PAQUET"));
            CONSOLE_PRINT_LN(F(""));
        #endif

        return false;
    } else {
        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("--[SEND PAQUET]--"));
            CONSOLE_PRINT_LN(F("DATA: <WorkingPackage> {"));
            CONSOLE_PRINT(F("   maxCurrent: "));
            CONSOLE_PRINT_LN(paquet.max_current);
            CONSOLE_PRINT(F("   minCurrent: "));
            CONSOLE_PRINT_LN(paquet.min_current);
            CONSOLE_PRINT(F("   avergeCurrent: "));
            CONSOLE_PRINT_LN(paquet.average_current);
            CONSOLE_PRINT(F("   timeInit: "));
            CONSOLE_PRINT_LN(paquet.time_init);
            CONSOLE_PRINT(F("   timeStop: "));
            CONSOLE_PRINT_LN(paquet.time_stop);
            CONSOLE_PRINT(F("   time: "));
            CONSOLE_PRINT_LN(paquet.time);
            CONSOLE_PRINT_LN(F("}"));
            CONSOLE_PRINT(F("TO NODE: "));
            CONSOLE_PRINT_LN(node);
            CONSOLE_PRINT(F("HEADER: "));
            CONSOLE_PRINT_LN((char)header);
            CONSOLE_PRINT_LN(F(""));
        #endif

        return true;
    }
}

bool connectRF24() {
    #ifdef CONSOLE_DEBUG
        CONSOLE_PRINT_LN(F("CONNECT..."));
    #endif

    if(mesh.checkConnection()) {
        time_out_reset = rtc->now().unixtime();
        
        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("CONNECTION OK"));
            CONSOLE_PRINT_LN(F("")); 
        #endif

        return true;
    } else {
        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("CONNECTION ERROR"));
            CONSOLE_PRINT_LN(F(""));
        #endif
        
        if(rtc->now().unixtime() - time_out_reset > 60) {
            if(!renewAdressRF24()) {
                radio.powerDown();
                delay(250);
                mesh.setNodeID(NODE_ID);
                mesh.begin(100, RF24_250KBPS);
                delay(250);
                radio.setPALevel(RF24_PA_MAX);
                radio.powerUp();
            }

            time_out_reset = rtc->now().unixtime();
        }

        return false;
    }
}

bool renewAdressRF24() {
    #ifdef CONSOLE_DEBUG
        CONSOLE_PRINT_LN(F("RENEWING ADRRESS..."));
    #endif

    if(mesh.renewAddress()) {
        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("ADRRESS OK"));
            CONSOLE_PRINT_LN(F(""));
        #endif

        return true;
    } else {
        #ifdef CONSOLE_DEBUG
            CONSOLE_PRINT_LN(F("ADRRESS FAIL"));
            CONSOLE_PRINT_LN(F(""));
        #endif

        return false;
    }
}