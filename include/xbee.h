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
#include <stdlib.h>
#include "port.h"

// Abstract base class for XBee
typedef struct XBee XBee;

// Function pointers for methods in the base class
typedef struct {
    bool (*init)(XBee* self, uint32_t baudrate, const char* device);
    bool (*connect)(XBee* self);
    bool (*disconnect)(XBee* self);
    bool (*send_data)(XBee* self, const void* data);
    bool (*soft_reset)(XBee* self);
    void (*hard_reset)(XBee* self);
    void (*process)(XBee* self);
    bool (*connected)(XBee* self);
    void (*handle_rx_packet_frame)(XBee* self, void *frame);
    void (*handle_transmit_status_frame)(XBee* self, void *frame);
} XBeeVTable;

// Function pointers for hardware abstraction methods in the base class
typedef struct {
    uart_status_t (*PortUartRead)(uint8_t *buf, int len, int *bytes_read);
    int (*PortUartWrite)(uint8_t *data, uint16_t len);
    uint32_t (*PortMillis)(void);
    void (*PortFlushRx)(void);
    int (*PortUartInit)(uint32_t baudrate, const char *device);
    void (*PortDelay)(uint32_t ms);
} XBeeHTable;

// Function pointers for callback methods in the base class
typedef struct {
    void (*OnReceiveCallback)(XBee* self, void * data);
    void (*OnConnectCallback)(XBee* self);
    void (*OnDisconnectCallback)(XBee* self);
    void (*OnSendCallback)(XBee* self, void * data);
} XBeeCTable;

struct XBee {
    // Add common XBee attributes here
    const XBeeVTable* vtable;
    const XBeeHTable* htable;
    const XBeeCTable* ctable;
    uint8_t frameIdCntr;
};

// Interface functions to call the methods
bool XBee_Init(XBee* self, uint32_t baudrate, const char* device);
bool XBee_Connect(XBee* self);
bool XBee_Disconnect(XBee* self);
bool XBee_SendData(XBee* self, const void*);
bool XBee_SoftReset(XBee* self);
void XBee_HardReset(XBee* self);
void XBee_Process(XBee* self);
bool XBee_Connected(XBee* self);
bool XBee_WriteConfig(XBee* self);
bool XBee_ApplyChanges(XBee* self);

#endif // XBEE_H
