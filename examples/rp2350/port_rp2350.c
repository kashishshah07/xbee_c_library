/**
 * @file example.c
 * @brief Example application demonstrating the use of the XBee library on RP2040 (Raspberry Pi Pico).
 * 
 * This file contains a sample application that demonstrates how to use the XBee library 
 * to communicate with XBee modules on an RP2040 platform. It showcases basic operations 
 * such as initializing the XBee module, connecting to the network, and transmitting & receiving data.
 * 
 * @version 1.0
 * @date 2024-08-13
 * 
 * @license MIT
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * @author Felix Galindo
 * @contact felix.galindo@digi.com
 */

#include "xbee_api_frames.h"
#include "xbee_at_cmds.h"
#include "xbee_lr.h"
#include "port.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

#define DEBUG_UART uart0      ///< UART used for debugging output (printf redirection)
#define DEBUG_BAUDRATE 115200 ///< Baud rate for the debug UART
#define DEBUG_TX_PIN 0        ///< TX pin for UART0 (adjust as needed)
#define DEBUG_RX_PIN 1        ///< RX pin for UART0 (adjust as needed)

/**
 * @brief Initialize the debug UART and configure `printf` to use it.
 * 
 * This function initializes the debug UART (e.g., `uart0`) and sets up `printf` 
 * to use this UART for debugging output.
 */
void Debug_UART_Init() {
    uart_init(DEBUG_UART, DEBUG_BAUDRATE);
    gpio_set_function(DEBUG_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(DEBUG_RX_PIN, GPIO_FUNC_UART);

    // Configure stdio to use the debug UART
    stdio_uart_init_full(DEBUG_UART, DEBUG_TX_PIN, DEBUG_RX_PIN, DEBUG_BAUDRATE);
}

int main() {
    // Initialize the standard I/O library for I/O operations (required by the SDK)
    stdio_init_all();

    // Initialize the debug UART for printf redirection
    Debug_UART_Init();

    printf("Debug UART initialized. Starting XBee example...\n");

    // Harware Abstraction Function Pointer Table for XBeeLR
    const XBeeHTable XBeeLR_HTable = {
        .PortUartRead = port_uart_read,
        .PortUartWrite = port_uart_write,
        .PortMillis = port_millis,
        .PortFlushRx = port_flush_rx,
        .PortUartInit = port_uart_init,
        .PortDelay = port_delay,
    };

    // Callback Function Pointer Table for XBeeLR
    const XBeeCTable XBeeLR_CTable = {
        .OnReceiveCallback = OnReceiveCallback, // You can set this to NULL if no callbacks are needed
        .OnSendCallback = OnSendCallback,       // You can set this to NULL if no callbacks are needed
    };

    // Create an instance of the XBeeLR class
    XBeeLR* my_xbee_lr = XBeeLR_Create(&XBeeLR_CTable, &XBeeLR_HTable);

    // Initialize the communication UART (uart1) for the XBee module
    if (!XBee_Init((XBee*)my_xbee_lr, COMM_BAUDRATE, NULL)) {
        printf("Failed to initialize XBee communication UART\n");
        return -1;
    }

    // Read and print the LoRaWAN Device EUI
    uint8_t dev_eui[17];
    XBeeLR_GetDevEUI((XBee*)my_xbee_lr, dev_eui, sizeof(dev_eui));
    printf("DEVEUI: %s\n", dev_eui);

    // Set LoRaWAN network settings
    printf("Configuring LoRaWAN network settings...\n");
    XBeeLR_SetAppEUI((XBee*)my_xbee_lr, "37D56A3F6CDCF0A5");
    XBeeLR_SetAppKey((XBee*)my_xbee_lr, "CD32AAB41C54175E9060D86F3A8B7F48");
    XBeeLR_SetNwkKey((XBee*)my_xbee_lr, "CD32AAB41C54175E9060D86F3A8B7F48");
    XBee_WriteConfig((XBee*)my_xbee_lr);
    XBee_ApplyChanges((XBee*)my_xbee_lr);

    // Connect to the LoRaWAN network
    printf("Connecting to LoRaWAN network...\n");
    if (!XBee_Connect((XBee*)my_xbee_lr)) {
        printf("Failed to connect to LoRaWAN network.\n");
        return -1;
    } else {
        printf("Connected to LoRaWAN network!\n");
    }

    // Prepare the XBeeLR payload to send
    uint8_t example_payload[] = {0xC0, 0xC0, 0xC0, 0xFF, 0xEE};
    uint16_t payload_len = sizeof(example_payload) / sizeof(example_payload[0]);
    XBeeLRPacket_t packet = {
        .payload = example_payload,
        .payloadSize = payload_len,
        .port = 2,
        .ack = 0,
    };

    uint32_t start_time = port_millis();
    while (1) {
        // Process any incoming data from the XBee module
        XBee_Process((XBee*)my_xbee_lr);

        // Check if 10 seconds have passed
        if (port_millis() - start_time >= 10000) {
            if (XBee_Connected((XBee*)my_xbee_lr)) {
                printf("Sending data: 0x");
                for (int i = 0; i < payload_len; i++) {
                    printf("%02X", example_payload[i]);
                }
                printf("\n");

                if (XBee_SendData((XBee*)my_xbee_lr, &packet)) {
                    printf("Failed to send data.\n");
                } else {
                    printf("Data sent successfully.\n");
                }

                // Update payload for the next transmission
                packet.payload[0] = packet.payload[0] + 1; // Increment payload
            } else {
                printf("Not connected. Attempting to reconnect...\n");
                if (!XBee_Connect((XBee*)my_xbee_lr)) {
                    printf("Failed to reconnect.\n");
                }
            }
            // Reset the start time
            start_time = port_millis();
        }
    }

    return 0;
}

/**
 * @brief Callback function triggered when data is received from the XBee module.
 * 
 * This function is called when the XBee module receives data. It processes the 
 * incoming data, extracts relevant information, and handles it accordingly. 
 * The function is typically registered as a callback to be executed automatically 
 * upon data reception.
 * 
 * @param[in] self Pointer to the XBee instance that received the data.
 * @param[in] data Pointer to the received data.
 * 
 * @return void This function does not return a value.
 */
void OnReceiveCallback(XBee* self, void* data) {
    XBeeLRPacket_t* packet = (XBeeLRPacket_t*) data;
    printf("Received Packet: ");
    for (int i = 1; i < packet->payloadSize; i++) {
        printf("%02X ", packet->payload[i]);
    }
    printf("\nAck: %u\nPort: %u\nRSSI: %d\nSNR: %d\nDownlink Counter: %lu\n", 
            packet->ack, packet->port, packet->rssi, packet->snr, packet->counter);
}

/**
 * @brief Callback function triggered after data is sent from the XBee module.
 * 
 * This function is called when the XBee module completes sending data. It handles 
 * any post-send operations, such as logging the transmission status or updating 
 * the state of the application. The function is typically registered as a callback 
 * to be executed automatically after data is transmitted.
 * 
 * @param[in] self Pointer to the XBee instance that sent the data.
 * @param[in] data Pointer to the data structure containing the sent data.
 * 
 * @return void This function does not return a value.
 */
void OnSendCallback(XBee* self, void* data) {
    XBeeLRPacket_t* packet = (XBeeLRPacket_t*) data;
    switch (packet->status) {
        case 0:
            printf("Send successful (frameId: 0x%02X)\n", packet->frameId);
            break;
        case 0x01:
            printf("Send failed (frameId: 0x%02X) (reason: Ack Failed)\n", packet->frameId);
            break;
        default:
            printf("Send failed (frameId: 0x%02X) (reason: 0x%02X)\n", packet->frameId, packet->status);
            break;
    }
}
