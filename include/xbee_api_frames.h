/**
 * @file xbee_api_frames.h
 * @brief XBee API Frames Header
 * @version 1.0
 * @date 2024-08-08
 * 
 * @license MIT
 * 
 * @author Felix Galindo, Digi International
 * @contact felix.galindo@digi.com
 */

#ifndef XBEE_FRAMES_H
#define XBEE_FRAMES_H

#include "xbee_at_cmds.h"

#define XBEE_MAX_FRAME_DATA_SIZE 256

// Enum for different API frame types
typedef enum {

    /**< XBee Common API Frames */
    XBEE_API_TYPE_AT_COMMAND = 0x08,
    XBEE_API_TYPE_AT_RESPONSE = 0x88,
    XBEE_API_TYPE_MODEM_STATUS = 0x8A,

    /**< XBee LR Specific API Frames */
    XBEE_API_TYPE_TX_STATUS = 0x89,
    XBEE_API_TYPE_RX_PACKET = 0x90,

    // Add other types as needed
} xbee_api_frame_type_t;

// Structure to represent an API frame
typedef struct {
    xbee_api_frame_type_t type;  // Type of the API frame
    uint16_t length;             // Length of the frame data
    uint8_t checksum;             // checksum of the API frame
    uint8_t data[XBEE_MAX_FRAME_DATA_SIZE]; // Frame data
} xbee_api_frame_t;

// Function prototypes
int api_receive_api_frame(xbee_api_frame_t *frame);
void api_send_at_command(at_command_t command, const uint8_t *parameter, uint8_t param_length);
void xbee_handle_at_response(xbee_api_frame_t *frame);
void xbee_handle_modem_status(xbee_api_frame_t *frame);
void xbee_handle_rx_packet(xbee_api_frame_t *frame);

#endif // XBEE_FRAMES_H
