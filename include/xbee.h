/**
 * @file xbee.c
 * @brief XBee Class Header
 * @version 1.0
 * @date 2024-08-08
 * 
 * @license MIT
 * 
 * @author Felix Galindo, Digi International
 * @contact felix.galindo@digi.com
 */

#ifndef XBEE_H
#define XBEE_H

#include <stdint.h>
#include <stdbool.h>
#include "port.h"

// Abstract base class for XBee
typedef struct XBee XBee;

// Callback function types
typedef void (*OnReceiveCallback)(XBee* self, const uint8_t* data, uint16_t length);
typedef void (*OnConnectCallback)(XBee* self);
typedef void (*OnDisconnectCallback)(XBee* self);
typedef void (*OnSendCallback)(XBee* self, const uint8_t* data, uint16_t length);

// Function pointers for methods in the base class
typedef struct {
    void (*connect)(XBee* self);
    void (*disconnect)(XBee* self);
    void (*send_data)(XBee* self, const uint8_t* data, uint16_t length);
    void (*receive_data)(XBee* self, uint8_t* buffer, uint16_t buffer_size);
    void (*soft_reset)(XBee* self);
    void (*hard_reset)(XBee* self);
    void (*process)(XBee* self);
    void (*configure_network)(XBee* self, const void* config);
    void (*configure_serial)(XBee* self, const void* config);
    bool (*connected)(XBee* self);
} XBeeVTable;

struct XBee {
    const XBeeVTable* vtable;
    // Add common XBee attributes here (e.g., baud rate, network settings)

    // Callback function pointers
    OnReceiveCallback onReceive;
    OnConnectCallback onConnect;
    OnDisconnectCallback onDisconnect;
    OnSendCallback onSend;
};

// Interface functions to call the methods
void XBee_Connect(XBee* self);
void XBee_Disconnect(XBee* self);
void XBee_SendData(XBee* self, const uint8_t* data, uint16_t length);
void XBee_SoftReset(XBee* self);
void XBee_HardReset(XBee* self);
void XBee_Process(XBee* self);
void XBee_ConfigureNetwork(XBee* self, const void* config);
void XBee_ConfigureSerial(XBee* self, const void* config);
bool XBee_Connected(XBee* self);

// Methods to set callback functions
void XBee_SetOnReceiveCallback(XBee* self, OnReceiveCallback callback);
void XBee_SetOnConnectCallback(XBee* self, OnConnectCallback callback);
void XBee_SetOnDisconnectCallback(XBee* self, OnDisconnectCallback callback);
void XBee_SetOnSendCallback(XBee* self, OnSendCallback callback);

#endif // XBEE_H
