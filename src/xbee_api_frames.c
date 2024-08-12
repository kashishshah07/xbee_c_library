/**
 * @file xbee_api_frames.c
 * @brief XBee API Frames
 * @version 1.0
 * @date 2024-08-08
 * 
 * @license MIT
 * 
 * @author Felix Galindo, Digi International
 * @contact felix.galindo@digi.com
 */

#include "xbee_api_frames.h"
#include "xbee.h"
#include "port.h"
#include <stdio.h>
#include <string.h>

// API Frame Functions

// Calculate checksum for API frame
static uint8_t calculate_checksum(const uint8_t *frame, uint16_t len) {
    uint8_t sum = 0;
    for (uint16_t i = 3; i < len; i++) {
        sum += frame[i];
    }
    return 0xFF - sum;
}

//Sends XBee API frame 
//Returns 0 if successful
int api_send_frame(XBee* self,uint8_t frame_type, const uint8_t *data, uint16_t len) {
    uint8_t frame[256];
    uint16_t frame_length = 0;
    self->frameIdCntr++;
    if(self->frameIdCntr == 0) self->frameIdCntr = 1; //reset frame counter when 0

    // Start delimiter
    frame[frame_length++] = 0x7E;

    // Length MSB and LSB
    frame[frame_length++] = (len + 1) >> 8;
    frame[frame_length++] = (len + 1) & 0xFF;

    // Frame type
    frame[frame_length++] = frame_type;

    // Frame data
    memcpy(&frame[frame_length], data, len);
    frame_length += len;

    // Calculate and add checksum
    frame[frame_length] = calculate_checksum(frame, frame_length);
    frame_length++;

    // Print the API frame in hex format
    API_FRAME_DEBUG_PRINT("Sending API Frame: ");
    for (uint16_t i = 0; i < frame_length; i++) {
        API_FRAME_DEBUG_PRINT("0x%02X ", frame[i]);
    }
    API_FRAME_DEBUG_PRINT("\n");

    // Send frame via UART
    int uart_status = self->htable->PortUartWrite(frame, frame_length);
    if (uart_status != 0) {
        return API_SEND_ERROR_UART_FAILURE;
    }

    // Return success if everything went well
    return API_SEND_SUCCESS;
}

// Function to send AT command through API frame mode
//Returns 0 if successful
int api_send_at_command(XBee* self,at_command_t command, const uint8_t *parameter, uint8_t param_length) {
    uint8_t frame_data[128];
    uint16_t frame_length = 0;

   // Check if the parameter length is too large
    if (param_length > 128) {
        return API_SEND_ERROR_FRAME_TOO_LARGE;
    }

    // Frame ID
    frame_data[frame_length++] = self->frameIdCntr;

    // AT Command (2 bytes)
    const char *cmd_str = at_command_to_string(command);
    frame_data[frame_length++] = cmd_str[0];
    frame_data[frame_length++] = cmd_str[1];

    if (cmd_str == NULL) {
        return API_SEND_ERROR_INVALID_COMMAND;
    }

    // AT Command Parameter
    if (param_length > 0) {
        memcpy(&frame_data[frame_length], parameter, param_length);
        frame_length += param_length;
    }

    // Print the AT command and parameter in a readable format
    API_FRAME_DEBUG_PRINT("Sending AT Command: %s\n", cmd_str);
    if (param_length > 0) {
        API_FRAME_DEBUG_PRINT("Parameter: ");
        for (uint8_t i = 0; i < param_length; i++) {
            API_FRAME_DEBUG_PRINT("0x%02X ", parameter[i]);
        }
        API_FRAME_DEBUG_PRINT("\n");
    } else {
        API_FRAME_DEBUG_PRINT("No Parameters\n");
    }

    // Use api_send_frame to send the complete frame
    return api_send_frame(self, XBEE_API_TYPE_AT_COMMAND, frame_data, frame_length);
}

//Checks for received api frames and populates frame pointer
//Returns 0 if successful
int api_receive_api_frame(XBee* self,xbee_api_frame_t *frame) {
    if (!frame) {
        API_FRAME_DEBUG_PRINT("Error: Invalid frame pointer. The frame pointer passed to the function is NULL.\n");
        return -1;
    }

    memset(frame, 0, sizeof(xbee_api_frame_t));

    // Attempt to read the start delimiter
    uint8_t start_delimiter;
    int bytes_received;
    uart_status_t status = self->htable->PortUartRead(&start_delimiter, 1, &bytes_received);

    if (status != UART_SUCCESS || bytes_received <= 0) {
        if (status == UART_ERROR_TIMEOUT) {
            API_FRAME_DEBUG_PRINT("Error: Timeout occurred while waiting to read start delimiter. No data received within %d ms.\n", UART_READ_TIMEOUT_MS);
        } else {
            API_FRAME_DEBUG_PRINT("Error: Failed to read start delimiter. Status: %d, Bytes received: %d\n", status, bytes_received);
        }
        return -2;
    }
    API_FRAME_DEBUG_PRINT("Start delimiter received: 0x%02X\n", start_delimiter);

    if (start_delimiter != 0x7E) {
        API_FRAME_DEBUG_PRINT("Error: Invalid start delimiter. Expected 0x7E, but received 0x%02X.\n", start_delimiter);
        return -3;
    }

    // Read length
    uint8_t length_bytes[2];
    status = self->htable->PortUartRead(length_bytes, 2, &bytes_received);

    if (status != UART_SUCCESS || bytes_received != 2) {
        if (status == UART_ERROR_TIMEOUT) {
            API_FRAME_DEBUG_PRINT("Error: Timeout occurred while waiting to read frame length. No data received within %d ms.\n", UART_READ_TIMEOUT_MS);
        } else {
            API_FRAME_DEBUG_PRINT("Error: Failed to read frame length. Status: %d, Bytes received: %d\n", status, bytes_received);
        }
        return -4;
    }

    uint16_t length = (length_bytes[0] << 8) | length_bytes[1];
    API_FRAME_DEBUG_PRINT("Frame length received: %d bytes\n", length);

    if (length > XBEE_MAX_FRAME_DATA_SIZE) {
        API_FRAME_DEBUG_PRINT("Error: Frame length exceeds buffer size. Received length: %d bytes, but maximum allowed is %d bytes.\n", length, XBEE_MAX_FRAME_DATA_SIZE);
        return -5;
    }

    // Read the frame data
    status = self->htable->PortUartRead(frame->data, length, &bytes_received);

    if (status != UART_SUCCESS || bytes_received != length) {
        if (status == UART_ERROR_TIMEOUT) {
            API_FRAME_DEBUG_PRINT("Error: Timeout occurred while waiting to read frame data. Expected %d bytes, but received %d bytes within %d ms.\n", length, bytes_received, UART_READ_TIMEOUT_MS);
        } else {
            API_FRAME_DEBUG_PRINT("Error: Failed to read complete frame data. Status: %d, Expected: %d bytes, Received: %d bytes.\n", status, length, bytes_received);
        }
        return -6;
    }

    // Print the received frame data
    API_FRAME_DEBUG_PRINT("Complete frame data received: ");
    for (int i = 0; i < bytes_received; i++) {
        API_FRAME_DEBUG_PRINT("0x%02X ", frame->data[i]);
    }
    API_FRAME_DEBUG_PRINT("\n");

    // Read the checksum
    status = self->htable->PortUartRead(&(frame->checksum), 1, &bytes_received);

    if (status != UART_SUCCESS || bytes_received != 1) {
        if (status == UART_ERROR_TIMEOUT) {
            API_FRAME_DEBUG_PRINT("Error: Timeout occurred while waiting to read checksum. Expected 1 byte, but received %d bytes within %d ms.\n", bytes_received, UART_READ_TIMEOUT_MS);
        } else {
            API_FRAME_DEBUG_PRINT("Error: Failed to read complete frame data. Status: %d, Expected: 1 bytes, Received: %d bytes.\n", status,bytes_received);
        }
        return -7;
    }

    // Populate frame structure
    frame->length = length;
    frame->type = frame->data[0];

    // Check and verify the checksum
    uint8_t checksum = frame->checksum;
    for (int i = 0; i < (length); i++) {
        checksum += frame->data[i];
    }
    if (checksum != 0xFF) {
        API_FRAME_DEBUG_PRINT("Error: Invalid checksum. Expected 0xFF, but calculated 0x%02X.\n", checksum);
        return -8;
    }

    return 0; // Successfully received a frame
}

//Calls registered handlers
void api_handle_frame(XBee* self, xbee_api_frame_t frame){
    switch (frame.type) {
        case XBEE_API_TYPE_AT_RESPONSE:
            xbee_handle_at_response(self, &frame);
            break;
        case XBEE_API_TYPE_MODEM_STATUS:
            xbee_handle_modem_status(self, &frame);
            break;
        case XBEE_API_TYPE_TX_STATUS:
            if(self->vtable->handle_transmit_status_frame){
                self->vtable->handle_transmit_status_frame(self, &frame);
            }
            break;
        case XBEE_API_TYPE_LR_RX_PACKET:
        case XBEE_API_TYPE_LR_EXPLICIT_RX_PACKET:
            if(self->vtable->handle_rx_packet_frame){
                self->vtable->handle_rx_packet_frame(self, &frame);
            }
            break;
        default:
            API_FRAME_DEBUG_PRINT("Received unknown frame type: 0x%02X\n", frame.type);
            break;
    }
}

//Sends AT Cmd via API frame and waits for response
//Returns 0 if successful 
int api_send_at_command_and_get_response(XBee* self, at_command_t command, const char *parameter, uint8_t *response_buffer, 
    uint8_t *response_length, uint32_t timeout_ms) {
    // Send the AT command using API frame
    uint8_t param_length = (parameter != NULL) ? strlen(parameter) : 0;
    api_send_at_command(self, command, (const uint8_t *)parameter, param_length);

    // Get the start time using the platform-specific function
    uint32_t start_time = self->htable->PortMillis();
    
    // Wait and receive the response within the timeout period
    xbee_api_frame_t frame;
    int status;

    while (1) {
        // Attempt to receive the API frame
        status = api_receive_api_frame(self, &frame);

        // Check if a valid frame was received
        if (status == 0) {
            // Check if the received frame is an AT response
            if (frame.type == XBEE_API_TYPE_AT_RESPONSE) {

                // Extract the AT command response
                *response_length = frame.length - 5;  // Subtract the frame ID and AT command bytes
                API_FRAME_DEBUG_PRINT("response_length: %u\n", *response_length);
                if(frame.data[4] == 0){
                    if((response_buffer != NULL) && (*response_length)){
                        memcpy(response_buffer, &frame.data[5], *response_length);
                    }
                }else{
                    API_FRAME_DEBUG_PRINT("API Frame AT CMD Error.\n");
                    return API_SEND_AT_CMD_ERROR;
                }
            
                // Return success
                return API_SEND_SUCCESS;
            } 
            else{
                api_handle_frame(self, frame);
            }
        }

        // Check if the timeout period has elapsed using platform-specific time
        if ((self->htable->PortMillis() - start_time) >= timeout_ms) {
            API_FRAME_DEBUG_PRINT("Timeout waiting for AT response.\n");
            return API_SEND_AT_CMD_RESONSE_TIMEOUT;
        }
        
        self->htable->PortDelay(1);
    }
}

//Print out AT Response
void xbee_handle_at_response(XBee* self, xbee_api_frame_t *frame) {
    // The first byte of frame->data is the Frame ID
    uint8_t frame_id = frame->data[1];

    // The next two bytes are the AT command
    char at_command[3];
    at_command[0] = frame->data[2];
    at_command[1] = frame->data[3];
    at_command[2] = '\0'; // Null-terminate the string

    // The next byte is the command status
    uint8_t command_status = frame->data[4];

    // Print the basic information
    API_FRAME_DEBUG_PRINT("AT Response:\n");
    API_FRAME_DEBUG_PRINT("  Frame ID: %d\n", frame_id);
    API_FRAME_DEBUG_PRINT("  AT Command: %s\n", at_command);
    API_FRAME_DEBUG_PRINT("  Command Status: %d\n", command_status);

    // Check if there is additional data in the frame
    if (frame->length > 5) {
        API_FRAME_DEBUG_PRINT("  Data: ");
        // Print the remaining data bytes
        API_FRAME_DEBUG_PRINT("%s\n", &(frame->data[5]));
    } else {
        API_FRAME_DEBUG_PRINT("  No additional data.\n");
    }
}

//Should be moved to be handled by user?
void xbee_handle_modem_status(XBee* self, xbee_api_frame_t *frame) {
    if (frame->type != XBEE_API_TYPE_MODEM_STATUS) return;

    API_FRAME_DEBUG_PRINT("Modem Status: %d\n", frame->data[1]);
    // Add further processing as needed
}
