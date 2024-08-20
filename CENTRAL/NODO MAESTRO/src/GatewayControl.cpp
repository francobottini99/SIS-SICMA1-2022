#include "Headers/GatewayControl.h"

bool getRecibe(char* data, size_t responseLenght);
bool getSend(const char* data, const char tag);
void cleanBuffer(char* buffer, uint16_t size_buffer);

#ifdef CONSOLE_DEBUG
   void consolePrint_Send(GatewayNotificationPackage* gateway_notification_pakcage);
   void consolePrint_Send(const char* data);
   void consolePrint_Recibe(const char* data);
   void consolePrint_Recibe(GatewayNotificationPackage* gateway_notification_pakcage);
#endif

char gateway_buffer[SIZE_GATEWAY_BUFFER];

uint16_t size_data;

bool recibePackageFromGateway(GatewayNotificationPackage* gateway_notification_package, size_t responseLenght) {
   char aux_buffer[16];

   if(getRecibe(aux_buffer, responseLenght)) {
      gateway_notification_package->copy(GatewayNotificationPackage(aux_buffer, ";"));

      #ifdef CONSOLE_DEBUG
         consolePrint_Recibe(gateway_notification_package);
      #endif

      return true;
   } else return false;
}

bool recibeBufferFromGateway(char* buffer, size_t responseLenght) {
   if(getRecibe(buffer, responseLenght)) {
      #ifdef CONSOLE_DEBUG
         consolePrint_Recibe(buffer);
      #endif

      return true;
   } else return false;
}

bool sendToGateway(GatewayNotificationPackage gateway_notification_package, const char* data) {
   char aux_buffer[16];
   
   strcpy(gateway_buffer, data);   

   gateway_notification_package.toString(aux_buffer, ";");

   size_data = strlen(aux_buffer);
   
   if(getSend(aux_buffer, C_NOTIFICATION)) {
      #ifdef CONSOLE_DEBUG
         consolePrint_Send(&gateway_notification_package);
      #endif

      return true;
   } else return false;
}

bool sendToGateway(GatewayNotificationPackage gateway_notification_package, GatewaySmsPackage data) {
   char aux_buffer[16];
   
   data.toString(gateway_buffer, ";");   

   gateway_notification_package.toString(aux_buffer, ";");
   
   size_data = strlen(aux_buffer);

   if(getSend(aux_buffer, C_NOTIFICATION)) {
      #ifdef CONSOLE_DEBUG
         consolePrint_Send(&gateway_notification_package);
      #endif

      return true;
   } else return false;
}

bool sendToGateway(GatewayNotificationPackage gateway_notification_package) {
   char aux_buffer[16];

   gateway_notification_package.toString(aux_buffer, ";");
   
   size_data = strlen(aux_buffer);

   if(getSend(aux_buffer, C_NOTIFICATION)) {
      #ifdef CONSOLE_DEBUG
         consolePrint_Send(&gateway_notification_package);
      #endif

      return true;
   } else return false;
}

bool getSend(const char* data, const char tag) {
   uint16_t nC = 0;
   uint32_t time_out_while = millis();

   Wire.beginTransmission(GATEWAY_ADRRESS);
   Wire.write(tag);
   Wire.endTransmission(false);

   Wire.beginTransmission(GATEWAY_ADRRESS);
   Wire.write(C_START_TRASMISSION);
   Wire.endTransmission(false);

   while (nC < size_data && millis() - time_out_while < 5000) {
      Wire.beginTransmission(GATEWAY_ADRRESS);

      for (int i = 0; i < BUFFER_LENGTH; i++)
      {
         if(data[nC] == '\0') break;
         Wire.write((byte*)&data[nC++], sizeof(byte)); 
      }

      Wire.endTransmission(false);

      if(data[nC] == '\0') break;
   }

   if(millis() - time_out_while < 5000) {
      Wire.beginTransmission(GATEWAY_ADRRESS);
      Wire.write(C_END_TRASMISSION);
      Wire.endTransmission();

      #ifdef CONSOLE_DEBUG   
         consolePrint_Send(data);
      #endif

      return true;
   } else {
      Wire.beginTransmission(GATEWAY_ADRRESS);
      Wire.write(C_FAIL_TRASMISSION);
      Wire.endTransmission();

      return false;
   }
}

bool getRecibe(char* data, size_t responseLenght) {
   uint16_t nC = 0;
   uint32_t time_out_while = millis();
  
   Wire.beginTransmission(GATEWAY_ADRRESS);
   Wire.write(C_ASK_FOR_DATA);
   Wire.endTransmission();

   for (int requestIndex = 0; requestIndex <= (int)(responseLenght / BUFFER_LENGTH); requestIndex++) {
      Wire.requestFrom(GATEWAY_ADRRESS, requestIndex < (int)(responseLenght / BUFFER_LENGTH) ? BUFFER_LENGTH : responseLenght % BUFFER_LENGTH);
      
      while (Wire.available() && millis() - time_out_while < 5000) {
         data[nC++] = (char)Wire.read();
      }

      if(millis() - time_out_while > 5000) break;
   }

   if(millis() - time_out_while < 5000) {
      data[nC++] = '\0';
      
      Wire.beginTransmission(GATEWAY_ADRRESS);
      Wire.write(C_RECIBE);
      Wire.endTransmission();

      return true;
   } else {
      Wire.beginTransmission(GATEWAY_ADRRESS);
      Wire.write(C_FAIL_TRASMISSION);
      Wire.endTransmission();

      strcpy(data, "");

      return false;   
   }
}

bool sendGatewayBuffer() {
   size_data = strlen(gateway_buffer);

   if(getSend(gateway_buffer, C_BUFFER)) {
      cleanBuffer(gateway_buffer, size_data);
      size_data = 0;

      return true;
   } else return false;
}

char askRecibe() {
   Wire.beginTransmission(GATEWAY_ADRRESS);
   Wire.write(C_ASK_RECIBE);
   Wire.endTransmission();

   Wire.requestFrom(GATEWAY_ADRRESS, sizeof(char));

   return (char)Wire.read();   
}

char askSend() {
   Wire.beginTransmission(GATEWAY_ADRRESS);
   Wire.write(C_ASK_SEND);
   Wire.endTransmission();

   Wire.requestFrom(GATEWAY_ADRRESS, sizeof(char));

   return (char)Wire.read();   
}

size_t askForLength() {
   Wire.beginTransmission(GATEWAY_ADRRESS);
   Wire.write(C_ASK_FOR_LENGTH);
   Wire.endTransmission();

   Wire.requestFrom(GATEWAY_ADRRESS, sizeof(size_t));
   size_t responseLenght = Wire.read();
   return responseLenght;
}

void cleanBuffer(char* buffer, uint16_t size_buffer) {
    for (uint16_t i = 0; i < size_buffer; i++) {
        buffer[i] = '\0';
    }
}

#ifdef CONSOLE_DEBUG
   void consolePrint_Send(GatewayNotificationPackage* gateway_notification_pakcage) {
      CONSOLE_PRINT_LN(F("--[GATEWAY SEND DATA]--"));
      CONSOLE_PRINT_LN(F("DATA: <GatewayNotificationPackage> {"));
      CONSOLE_PRINT(F("   code: "));
      CONSOLE_PRINT_LN(gateway_notification_pakcage->code);
      CONSOLE_PRINT(F("   associate_data: "));
      CONSOLE_PRINT_LN(gateway_notification_pakcage->associated_data);
      CONSOLE_PRINT_LN(F("}"));
      CONSOLE_PRINT(F("ORIGIN NODE ID: "));
      CONSOLE_PRINT_LN(gateway_notification_pakcage->nodeId);
      CONSOLE_PRINT_LN(F(""));  
   }

   void consolePrint_Send(const char* data) {
      CONSOLE_PRINT_LN(F("--[GATEWAY SEND DATA]--"));
      CONSOLE_PRINT_LN(F("DATA: <const char> {"));
      CONSOLE_PRINT_LN(data);
      CONSOLE_PRINT_LN(F("}"));
   }

   void consolePrint_Recibe(const char* data) {
      CONSOLE_PRINT_LN(F("--[GATEWAY RECIBE DATA]--"));
      CONSOLE_PRINT_LN(F("DATA: <const char> {"));
      CONSOLE_PRINT_LN(data);
      CONSOLE_PRINT_LN(F("}"));
   }

   void consolePrint_Recibe(GatewayNotificationPackage* gateway_notification_pakcage) {
      CONSOLE_PRINT_LN(F("--[GATEWAY RECIBE DATA]--"));
      CONSOLE_PRINT_LN(F("DATA: <GatewayNotificationPackage> {"));
      CONSOLE_PRINT(F("   code: "));
      CONSOLE_PRINT_LN(gateway_notification_pakcage->code);
      CONSOLE_PRINT(F("   associate_data: "));
      CONSOLE_PRINT_LN(gateway_notification_pakcage->associated_data);
      CONSOLE_PRINT_LN(F("}"));
      CONSOLE_PRINT(F("DESINATION NODE ID: "));
      CONSOLE_PRINT_LN(gateway_notification_pakcage->nodeId);
      CONSOLE_PRINT_LN(F(""));
   }
#endif