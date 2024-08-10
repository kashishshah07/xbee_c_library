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

// Subclass for XBeeLR
typedef struct {
    XBee base;  // Inherit from XBee
    // Add XBeeLR specific attributes here
} XBeeLR;

XBeeLR* XBeeLR_Create(void);
bool XBeeLR_GetDevEUI(XBee* self, uint8_t* response_buffer, uint8_t buffer_size);

#endif // XBEELR_H
