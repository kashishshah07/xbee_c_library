#ifndef XBEE3_H
#define XBEE3_H

#include "xbee.h"

// Subclass for XBee3
typedef struct {
    XBee base;  // Inherit from XBee
    // Add XBee3 specific attributes here
} XBee3;

typedef struct {
    // XBee3 specific network configuration parameters
    XBeeNetworkConfig common;  // Inherit common parameters
    // Add specific parameters for XBee3 network
} XBee3NetworkConfig;

typedef struct {
    // XBee3 specific serial configuration parameters
    XBeeSerialConfig common;  // Inherit common parameters
    // Add specific parameters for XBee3 serial
} XBee3SerialConfig;

XBee3* XBee3_Create(void);

#endif // XBEE3_H
