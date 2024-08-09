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
#include "uart.h"
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

void api_send_frame(uint8_t frame_type, const uint8_t *data, uint16_t len) {
    uint8_t frame[256];
    uint16_t frame_length = 0;

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
    printf("Sending API Frame: ");
    for (uint16_t i = 0; i < frame_length; i++) {
        printf("0x%02X ", frame[i]);
    }
    printf("\n");

    // Send frame via UART
    uart_write(frame, frame_length);
}

// Function to send AT command through API frame mode and print it
void api_send_at_command(at_command_t command, const uint8_t *parameter, uint8_t param_length) {
    uint8_t frame_data[128];
    uint16_t frame_length = 0;

    // Frame ID (0x01 for reliable, 0x00 for no response required)
    frame_data[frame_length++] = 0x01;

    // AT Command (2 bytes)
    const char *cmd_str = at_command_to_string(command);
    frame_data[frame_length++] = cmd_str[0];
    frame_data[frame_length++] = cmd_str[1];

    // AT Command Parameter
    if (param_length > 0) {
        memcpy(&frame_data[frame_length], parameter, param_length);
        frame_length += param_length;
    }

    // Print the AT command and parameter in a readable format
    printf("Sending AT Command: %s\n", cmd_str);
    if (param_length > 0) {
        printf("Parameter: ");
        for (uint8_t i = 0; i < param_length; i++) {
            printf("0x%02X ", parameter[i]);
        }
        printf("\n");
    } else {
        printf("No Parameters\n");
    }

    // Use api_send_frame to send the complete frame
    api_send_frame(0x08, frame_data, frame_length);
}

int api_receive_api_frame(xbee_api_frame_t *frame) {
    if (!frame) {
        printf("Error: Invalid frame pointer. The frame pointer passed to the function is NULL.\n");
        return -1;
    }

    memset(frame, 0, sizeof(frame));

    // Attempt to read the start delimiter
    uint8_t start_delimiter;
    int bytes_received;
    uart_status_t status = uart_read(&start_delimiter, 1, &bytes_received);

    if (status != UART_SUCCESS || bytes_received <= 0) {
        if (status == UART_ERROR_TIMEOUT) {
            printf("Error: Timeout occurred while waiting to read start delimiter. No data received within %d ms.\n", UART_READ_TIMEOUT_MS);
        } else {
            printf("Error: Failed to read start delimiter. Status: %d, Bytes received: %d\n", status, bytes_received);
        }
        return -2;
    }
    printf("Start delimiter received: 0x%02X\n", start_delimiter);

    if (start_delimiter != 0x7E) {
        printf("Error: Invalid start delimiter. Expected 0x7E, but received 0x%02X.\n", start_delimiter);
        return -3;
    }

    // Read length
    uint8_t length_bytes[2];
    status = uart_read(length_bytes, 2, &bytes_received);

    if (status != UART_SUCCESS || bytes_received != 2) {
        if (status == UART_ERROR_TIMEOUT) {
            printf("Error: Timeout occurred while waiting to read frame length. No data received within %d ms.\n", UART_READ_TIMEOUT_MS);
        } else {
            printf("Error: Failed to read frame length. Status: %d, Bytes received: %d\n", status, bytes_received);
        }
        return -4;
    }

    uint16_t length = (length_bytes[0] << 8) | length_bytes[1];
    printf("Frame length received: %d bytes\n", length);

    if (length > XBEE_MAX_FRAME_DATA_SIZE) {
        printf("Error: Frame length exceeds buffer size. Received length: %d bytes, but maximum allowed is %d bytes.\n", length, XBEE_MAX_FRAME_DATA_SIZE);
        return -5;
    }

    // Read the frame data
    status = uart_read(frame->data, length, &bytes_received);

    if (status != UART_SUCCESS || bytes_received != length) {
        if (status == UART_ERROR_TIMEOUT) {
            printf("Error: Timeout occurred while waiting to read frame data. Expected %d bytes, but received %d bytes within %d ms.\n", length, bytes_received, UART_READ_TIMEOUT_MS);
        } else {
            printf("Error: Failed to read complete frame data. Status: %d, Expected: %d bytes, Received: %d bytes.\n", status, length, bytes_received);
        }
        return -6;
    }

    // Print the received frame data
    printf("Complete frame data received: ");
    for (int i = 0; i < bytes_received; i++) {
        printf("0x%02X ", frame->data[i]);
    }
    printf("\n");

    // Read the checksum
    status = uart_read(&(frame->checksum), 1, &bytes_received);

    if (status != UART_SUCCESS || bytes_received != 1) {
        if (status == UART_ERROR_TIMEOUT) {
            printf("Error: Timeout occurred while waiting to read checksum. Expected 1 byte, but received %d bytes within %d ms.\n", bytes_received, UART_READ_TIMEOUT_MS);
        } else {
            printf("Error: Failed to read complete frame data. Status: %d, Expected: 1 bytes, Received: %d bytes.\n", status,bytes_received);
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
        printf("Error: Invalid checksum. Expected 0xFF, but calculated 0x%02X.\n", checksum);
        return -8;
    }

    return 0; // Successfully received a frame
}

void xbee_handle_at_response(xbee_api_frame_t *frame) {
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
    printf("AT Response:\n");
    printf("  Frame ID: %d\n", frame_id);
    printf("  AT Command: %s\n", at_command);
    printf("  Command Status: %d\n", command_status);

    // Check if there is additional data in the frame
    if (frame->length > 5) {
        printf("  Data: ");
        // Print the remaining data bytes
        printf("%s\n", &(frame->data[5]));
    } else {
        printf("  No additional data.\n");
    }
}

void xbee_handle_modem_status(xbee_api_frame_t *frame) {
    if (frame->type != XBEE_API_TYPE_MODEM_STATUS) return;

    printf("Modem Status: %d\n", frame->data[1]);
    // Add further processing as needed
}

void xbee_handle_rx_packet(xbee_api_frame_t *frame) {
    if (frame->type != XBEE_API_TYPE_RX_PACKET) return;

    printf("Received Packet: ");
    for (int i = 1; i < frame->length; i++) {
        printf("%02X ", frame->data[i]);
    }
    printf("\n");
    // Add further processing as needed
}


void SendJoinReqApiFrame() {
    uint8_t frame_data[1]; // Adjust size as needed based on the frame structure

    // Example frame data for Join Request (you need to adjust based on actual protocol)
    frame_data[0] = 0x01;  // Frame ID
    // Add other relevant data to frame_data based on protocol specification
    // ...

    // Call the api_send_frame function to send the Join Request API frame
    api_send_frame(0x14, frame_data, sizeof(frame_data));
}

void SendTxReqApiFrame(const uint8_t* payload, uint16_t payload_len, uint8_t port, uint8_t options) {
    uint8_t frame_data[128]; // Adjust size as needed based on the frame structure

    // Example frame data for Tx Request
    frame_data[0] = 0x50;  // API Frame Tx Request ID
    frame_data[1] = 0x01;  // Frame ID
    frame_data[3] = port;  // LoRaWAN Port
    frame_data[4] = options;  // Transmit Options
    // Add the payload to the frame data
    memcpy(&frame_data[5], payload, payload_len);

    // Call the api_send_frame function to send the Tx Request API frame
    api_send_frame(0x50, frame_data, 4 + payload_len);
}