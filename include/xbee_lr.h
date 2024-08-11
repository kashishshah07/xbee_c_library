/**
 * @file xbee_lr.c
 * @brief XBee LR Subclass Header
 * @version 1.0
 * @date 2024-08-08
 * 
 * @license MIT
 * 
 * @author Felix Galindo, Digi International
 * @contact felix.galindo@digi.com
 */

#ifndef XBEELR_H
#define XBEELR_H

#include "xbee.h"

// Structure for XBee LR LoRaWAN packet
typedef struct XBeeLRPacket_s{
    uint8_t port;
    uint8_t payloadSize;
    uint8_t *payload;
    uint8_t ack;
    uint8_t status;
    uint8_t frameId;
    //For RX only
    int8_t rssi;
    int8_t snr;
    uint8_t dr;
    uint32_t counter;
}XBeeLRPacket_t;

// Function pointers for callback methods in the base class
typedef struct {
    void (*OnReceiveCallback)(XBee* self, XBeeLRPacket_t data);
    void (*OnConnectCallback)(XBee* self);
    void (*OnDisconnectCallback)(XBee* self);
    void (*OnSendCallback)(XBee* self, XBeeLRPacket_t data);
} XBeeCTable;

// Subclass for XBeeLR
typedef struct {
    XBee base;  // Inherit from XBee
    // Add XBeeLR specific attributes here like methods specific to an XBee type
    const XBeeCTable* ctable;
} XBeeLR;


XBeeLR* XBeeLR_Create(const XBeeCTable* cTable, const XBeeHTable* hTable);
bool XBeeLR_GetDevEUI(XBee* self, uint8_t* response_buffer, uint8_t buffer_size);
bool XBeeLR_SetAppEUI(XBee* self, const char* value);
bool XBeeLR_SetAppKey(XBee* self, const char* value);
bool XBeeLR_SetNwkKey(XBee* self, const char* value);

#endif // XBEELR_H
