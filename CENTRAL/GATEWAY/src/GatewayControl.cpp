#include "Headers/GatewayContol.h"

bool flag = false;

bool recibe_buffer = false;
bool recibe_notification = false;

bool send_buffer = false;
bool send_notification = false;

char gateway_read_notification_buffer[16];
char gateway_write_notification_buffer[16];

GatewayNotificationPackage* gateway_read_notification_package = nullptr;
GatewayNotificationPackage* gateway_write_notification_package = nullptr;

char* gateway_read_buffer = nullptr;
char* gateway_write_buffer = nullptr;

uint16_t size_read_buffer = 0;
uint16_t len_write_buffer = 0;
uint16_t len_data = 0;

uint32_t nC = 0;

bool inTransmission = false;

char tag = C_NULL;
char request = C_NULL;

void receiveEvent(int bytes) {
   char c;

   if (inTransmission) {
      uint32_t time_out = millis();

     while(Wire.available() && millis() - time_out < 5000)   {
         c = Wire.read(); 

         if(c != C_END_TRASMISSION && c != C_FAIL_TRASMISSION) {
            if(tag == C_BUFFER) gateway_read_buffer[nC++] = c;
            else if(tag == C_NOTIFICATION) gateway_read_notification_buffer[nC++] = c;
         } else break;
      }
   } else {
      if(Wire.available()) {
         c = Wire.read();
      }
   }
   
   if(c == C_START_TRASMISSION) {
      if(tag == C_BUFFER)  cleanBuffer(gateway_read_buffer, size_read_buffer); 
      else if(tag == C_NOTIFICATION)  cleanBuffer(gateway_read_notification_buffer, sizeof(gateway_read_notification_buffer));

      inTransmission = true;
      nC = 0;
   } else if(c == C_END_TRASMISSION) {
      if(tag == C_BUFFER)  {
         gateway_read_buffer[nC++] = '\0';

         #ifdef CONSOLE_DEBUG
            consolePrint_Recibe(gateway_read_buffer);
         #endif
      } else if(tag == C_NOTIFICATION)  {
         gateway_read_notification_buffer[nC++] = '\0';

         #ifdef CONSOLE_DEBUG
            consolePrint_Recibe(gateway_read_notification_buffer);
         #endif

         gateway_read_notification_package->copy(GatewayNotificationPackage(gateway_read_notification_buffer, ";"));

         #ifdef CONSOLE_DEBUG
            consolePrint_Recibe(gateway_read_notification_package);
         #endif

         flag = true;
      }

      tag = C_NULL;
      inTransmission = false;
   } else if(c == C_FAIL_TRASMISSION) {
      if(inTransmission) {
         if(tag == C_NOTIFICATION) recibe_notification = true;
         else if(tag == C_BUFFER) recibe_buffer = true;
      
         tag = C_NULL;
         inTransmission = false;
      }
   } else if(c == C_RECIBE) {
      if(send_notification) send_notification = false;
      else if(send_buffer) send_buffer = false;
   } else if(c == C_NOTIFICATION || c == C_BUFFER) {
      tag = c;
   } else {
      request = c;
   }
}

void requestEvent() {
   if(request == C_ASK_SEND) {
      if(send_notification) Wire.write(C_NOTIFICATION);
      else if(send_buffer) Wire.write(C_BUFFER);
      else Wire.write(C_NULL);
   } else if(request == C_ASK_FOR_LENGTH) {     
      if(send_notification) {
         gateway_write_notification_package->toString(gateway_write_notification_buffer, ";");
         len_data = strlen(gateway_write_notification_buffer);
      } 
      else if(send_buffer) len_data = strlen(gateway_write_buffer);

      Wire.write(len_data);
      nC = 0;
   } else if(request == C_ASK_RECIBE) {
      if(recibe_notification) { Wire.write(C_NOTIFICATION); recibe_notification = false; }
      else if(recibe_buffer) { Wire.write(C_BUFFER); recibe_buffer = false; }
      else Wire.write(C_NULL);
   } else if(request == C_ASK_FOR_DATA) {
      if(nC < (unsigned int)(len_data / BUFFER_LENGTH)) {
         if(send_notification) Wire.write(gateway_write_notification_buffer + nC * BUFFER_LENGTH, BUFFER_LENGTH);
         else if(send_buffer) Wire.write(gateway_write_buffer + nC * BUFFER_LENGTH, BUFFER_LENGTH);
         nC ++;
      } else {
         if(send_notification) Wire.write(gateway_write_notification_buffer + nC * BUFFER_LENGTH, (len_data % BUFFER_LENGTH)); 
         else if(send_buffer) Wire.write(gateway_write_buffer + nC * BUFFER_LENGTH, (len_data % BUFFER_LENGTH));
      }

      #ifdef CONSOLE_DEBUG
         if(send_notification) {consolePrint_Send(gateway_write_notification_package); consolePrint_Send(gateway_write_notification_buffer);}
         else if(send_buffer) consolePrint_Send(gateway_write_buffer);
      #endif
   }

   request = C_NULL;
}

void recibeBufferFromGateway(char* buffer, size_t size_buffer) {
   gateway_read_buffer = buffer;
   size_read_buffer = size_buffer;
   recibe_buffer = true;
   delay(300);
}

void sendToGateway(char* buffer, size_t size_buffer) {
   gateway_write_buffer = buffer;
   len_write_buffer = size_buffer;
   send_buffer = true;
   delay(300);
}

void recibePackageFromGateway(GatewayNotificationPackage* notification_package) {
   gateway_read_notification_package = notification_package;
   recibe_notification = true;
   delay(300);
}

void sendToGateway(GatewayNotificationPackage* notification_package) {
   gateway_write_notification_package = notification_package;
   send_notification = true;
   delay(300);
}

void sendToGateway(GatewayNotificationPackage* notification_package, char* buffer, size_t size_buffer) {
   gateway_write_notification_package = notification_package;
   gateway_write_buffer = buffer;
   len_write_buffer = size_buffer;

   send_notification = true;
   send_buffer = true;
   delay(300);
}

void cleanBuffer(char* buffer, uint16_t size_buffer) {
    for (uint16_t i = 0; i < size_buffer; i++) {
        buffer[i] = '\0';
    }
}

#ifdef CONSOLE_DEBUG
   void consolePrint_Recibe(GatewayNotificationPackage* notification_package) {
      CONSOLE_PRINT_LN(F("--[GATEWAY RECIBE DATA]--"));
      CONSOLE_PRINT_LN(F("DATA: <GatewayNotificationPackage> {"));
      CONSOLE_PRINT(F("   code: "));
      CONSOLE_PRINT_LN((int)notification_package->code);
      CONSOLE_PRINT(F("   associated_data: "));
      CONSOLE_PRINT_LN((int)notification_package->associated_data);
      CONSOLE_PRINT_LN(F("}"));
      CONSOLE_PRINT(F("ORIGIN NODE ID: "));
      CONSOLE_PRINT_LN(notification_package->nodeId);
      CONSOLE_PRINT_LN(F(""));
   }

   void consolePrint_Send(GatewayNotificationPackage* notification_package) {
      CONSOLE_PRINT_LN(F("--[GATEWAY SEND DATA]--"));
      CONSOLE_PRINT_LN(F("DATA: <GatewayNotificationPackage> {"));
      CONSOLE_PRINT(F("   code: "));
      CONSOLE_PRINT_LN((int)notification_package->code);
      CONSOLE_PRINT(F("   associated_data: "));
      CONSOLE_PRINT_LN((int)notification_package->associated_data);
      CONSOLE_PRINT_LN(F("}"));
      CONSOLE_PRINT(F("DESINATION NODE ID: "));
      CONSOLE_PRINT_LN(notification_package->nodeId);
      CONSOLE_PRINT_LN(F(""));
   }

   void consolePrint_Recibe(const char* data) {
      CONSOLE_PRINT_LN(F("--[GATEWAY RECIBE DATA]--"));
      CONSOLE_PRINT_LN(F("DATA: <const char> {"));
      CONSOLE_PRINT_LN(data);
      CONSOLE_PRINT_LN(F("}"));
      CONSOLE_PRINT_LN(F(""));
   }

   void consolePrint_Send(const char* data) {
      CONSOLE_PRINT_LN(F("--[GATEWAY SEND DATA]--"));
      CONSOLE_PRINT_LN(F("DATA: <const char> {"));
      CONSOLE_PRINT_LN(data);
      CONSOLE_PRINT_LN(F("}"));
      CONSOLE_PRINT_LN(F(""));
   }
#endif