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
void XBee_Connect(XBee* self) {
    self->vtable->connect(self);
}

void XBee_Disconnect(XBee* self) {
    self->vtable->disconnect(self);
}

void XBee_SendData(XBee* self, const uint8_t* data, uint16_t length) {
    self->vtable->send_data(self, data, length);
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

void XBee_ConfigureNetwork(XBee* self, const void* config) {
    self->vtable->configure_network(self, config);
}

void XBee_ConfigureSerial(XBee* self, const void* config) {
    self->vtable->configure_serial(self, config);
}

uint8_t XBee_Connected(XBee* self) {
    return self->vtable->connected(self);
}

// Methods to set callback functions
void XBee_SetOnReceiveCallback(XBee* self, OnReceiveCallback callback) {
    self->onReceive = callback;
}

void XBee_SetOnConnectCallback(XBee* self, OnConnectCallback callback) {
    self->onConnect = callback;
}

void XBee_SetOnDisconnectCallback(XBee* self, OnDisconnectCallback callback) {
    self->onDisconnect = callback;
}

void XBee_SetOnSendCallback(XBee* self, OnSendCallback callback) {
    self->onSend = callback;
}


