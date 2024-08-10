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

typedef struct {
    // XBeeLR specific network configuration parameters
    XBeeNetworkConfig common;  // Inherit common parameters
    // Add specific parameters for XBeeLR network
} XBeeLRNetworkConfig;

typedef struct {
    // XBeeLR specific serial configuration parameters
    XBeeSerialConfig common;  // Inherit common parameters
    // Add specific parameters for XBeeLR serial
} XBeeLRSerialConfig;

XBeeLR* XBeeLR_Create(void);
bool XBeeLR_GetDevEUI(XBee* self);

#endif // XBEELR_H
