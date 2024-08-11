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
int XBee_Init(XBee* self, uint32_t baudrate, const char* device) {
    self->frameIdCntr = 1;
    return self->vtable->init(self, baudrate, device);
}

void XBee_Connect(XBee* self) {
    self->vtable->connect(self);
}

void XBee_Disconnect(XBee* self) {
    self->vtable->disconnect(self);
}

void XBee_SendData(XBee* self, const void* data) {
    self->vtable->send_data(self, data);
}

void XBee_SoftReset(XBee* self) {
    self->vtable->soft_reset(self);
}

void XBee_HardReset(XBee* self) {
    self->vtable->hard_reset(self);
}

void XBee_Process(XBee* self) {
    self->vtable->process(self);
}

uint8_t XBee_Connected(XBee* self) {
    return self->vtable->connected(self);
}

bool XBee_WriteConfig(XBee* self) {
    uint8_t response[33];
    uint8_t response_length;
    int status = api_send_at_command_and_get_response(self, AT_WR, NULL, response, &response_length, 5000);
    if(status != API_SEND_SUCCESS){
        port_debug_printf("Failed to Write Config\n");
    }
    return status;
}

bool XBee_ApplyChanges(XBee* self) {
    uint8_t response[33];
    uint8_t response_length;
    int status = api_send_at_command_and_get_response(self, AT_AC, NULL, response, &response_length, 5000);
    if(status != API_SEND_SUCCESS){
        port_debug_printf("Failed to Apply Changes\n");
    }
    return status;
}


