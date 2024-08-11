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
#include "xbee.h"

#define XBEE_MAX_FRAME_DATA_SIZE 256
#define API_SEND_SUCCESS 0
#define API_SEND_ERROR_TIMEOUT -1
#define API_SEND_ERROR_INVALID_COMMAND -2
#define API_SEND_ERROR_UART_FAILURE -3
#define API_SEND_ERROR_FRAME_TOO_LARGE -4
#define API_SEND_AT_CMD_ERROR -5
#define API_SEND_AT_CMD_RESONSE_TIMEOUT -6

#define API_FRAME_DEBUG_PRINT_ENABLED 1
#if API_FRAME_DEBUG_PRINT_ENABLED
#define API_FRAME_DEBUG_PRINT(...)             port_debug_printf(__VA_ARGS__)
#else
#define API_FRAME_DEBUG_PRINT(...)
#endif

// Enum for different API frame types
typedef enum {

    /**< XBee Common API Frames */
    XBEE_API_TYPE_AT_COMMAND = 0x08,
    XBEE_API_TYPE_TX_REQUEST = 0x10,
    XBEE_API_TYPE_MODEM_STATUS = 0x8A,
    XBEE_API_TYPE_AT_RESPONSE = 0x88,
    XBEE_API_TYPE_TX_STATUS = 0x89,

    /**< XBee LR Specific API Frames */
    XBEE_API_TYPE_LR_JOIN_REQUEST = 0x14,
    XBEE_API_TYPE_LR_TX_REQUEST = 0x50,
    XBEE_API_TYPE_LR_RX_PACKET = 0xD0,
    XBEE_API_TYPE_LR_EXPLICIT_RX_PACKET = 0xD1,

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
int api_receive_api_frame(XBee* self,xbee_api_frame_t *frame);
int api_send_at_command(XBee* self,at_command_t command, const uint8_t *parameter, uint8_t param_length);
int api_send_frame(XBee* self,uint8_t frame_type, const uint8_t *data, uint16_t len);
int api_send_at_command_and_get_response(XBee* self,at_command_t command, const char *parameter, uint8_t *response_buffer, uint8_t *response_length, uint32_t timeout_ms);
void api_handle_frame(XBee* self,xbee_api_frame_t frame);
void xbee_handle_at_response(XBee* self,xbee_api_frame_t *frame);
void xbee_handle_modem_status(XBee* self,xbee_api_frame_t *frame);
void xbee_handle_rx_packet(XBee* self,xbee_api_frame_t *frame);


#endif // XBEE_FRAMES_H
