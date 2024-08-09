#include "xbee.h"
#include "xbee_api_frames.h"  // Include your API frame functions here

// Base class methods
void XBee_Connect(XBee* self, const void* config) {
    self->vtable->connect(self, config);
}

void XBee_Disconnect(XBee* self) {
    self->vtable->disconnect(self);
}

void XBee_SendData(XBee* self, const uint8_t* data, uint16_t length) {
    self->vtable->send_data(self, data, length);
}

void XBee_ReceiveData(XBee* self, uint8_t* buffer, uint16_t buffer_size) {
    self->vtable->receive_data(self, buffer, buffer_size);
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

bool XBee_CheckNetworkConnection(XBee* self) {
    return self->vtable->check_network_connection(self);
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


