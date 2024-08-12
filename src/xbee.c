/**
 * @file xbee.c
 * @brief XBee Class Source
 * @version 1.0
 * @date 2024-08-08
 * 
 * @license MIT
 * 
 * @author Felix Galindo, Digi International
 * @contact felix.galindo@digi.com
 */

#include "xbee.h"
#include "xbee_api_frames.h" 

// Base class methods


bool XBee_Init(XBee* self, uint32_t baudrate, const char* device) {
    self->frameIdCntr = 1;
    return self->vtable->init(self, baudrate, device);
}

//Calls XBee subclass connect implementation
bool XBee_Connect(XBee* self) {
    return self->vtable->connect(self);
}

//Calls XBee subclass disconnect implementation
bool XBee_Disconnect(XBee* self) {
    return self->vtable->disconnect(self);
}

//Calls XBee subclass SendData implementation
bool XBee_SendData(XBee* self, const void* data) {
    return self->vtable->send_data(self, data);
}

bool XBee_SoftReset(XBee* self) {
    return self->vtable->soft_reset(self);
}

void XBee_HardReset(XBee* self) {
    self->vtable->hard_reset(self);
}

//Calls XBee subclass Process implementation
//Must be called continously
void XBee_Process(XBee* self) {
    self->vtable->process(self);
}

bool XBee_Connected(XBee* self) {
    return self->vtable->connected(self);
}

//Sends ATWR command
//0 if successful
bool XBee_WriteConfig(XBee* self) {
    uint8_t response[33];
    uint8_t response_length;
    int status = api_send_at_command_and_get_response(self, AT_WR, NULL, response, &response_length, 5000);
    if(status != API_SEND_SUCCESS){
        port_debug_printf("Failed to Write Config\n");
    }
    return status;
}

//Sends ATAC command
//0 if successful
bool XBee_ApplyChanges(XBee* self) {
    uint8_t response[33];
    uint8_t response_length;
    int status = api_send_at_command_and_get_response(self, AT_AC, NULL, response, &response_length, 5000);
    if(status != API_SEND_SUCCESS){
        port_debug_printf("Failed to Apply Changes\n");
    }
    return status;
}


