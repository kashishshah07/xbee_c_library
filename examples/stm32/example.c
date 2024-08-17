/**
 * @file example.c
 * @brief Example application demonstrating the use of the XBee library on STM32.
 * 
 * This file contains a sample application that demonstrates how to use the XBee library 
 * to communicate with XBee modules on an STM32 platform. It showcases basic operations 
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
#include "stm32f4xx_hal.h"  // Include STM32 HAL header for the target device

UART_HandleTypeDef huartDebug;  // Handle for debug UART

/**
 * @brief Redirect printf output to the specified UART.
 * 
 * This function overrides the fputc function to allow printf to output via the selected UART port.
 * 
 * @param ch The character to be printed.
 * @param f The file pointer (unused).
 * 
 * @return int The printed character.
 */
int fputc(int ch, FILE *f) {
    HAL_UART_Transmit(&huartDebug, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

/**
 * @brief Initialize the debug UART port for printf redirection.
 * 
 * This function initializes a UART port to be used for printf output.
 */
void DebugUartInit() {
    huartDebug.Instance = USART2;  // Use USART2 for debugging (adjust as necessary)
    huartDebug.Init.BaudRate = 115200;
    huartDebug.Init.WordLength = UART_WORDLENGTH_8B;
    huartDebug.Init.StopBits = UART_STOPBITS_1;
    huartDebug.Init.Parity = UART_PARITY_NONE;
    huartDebug.Init.Mode = UART_MODE_TX_RX;
    huartDebug.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huartDebug.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huartDebug);
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

/**
 * @brief Main entry point for the application.
 * 
 * This function initializes the STM32 hardware, sets up the XBee module, and enters
 * a loop to process incoming and outgoing data with the XBee module.
 * 
 * @return int Returns 0 on successful execution.
 */
int main() {
    HAL_Init();  // Initialize the Hardware Abstraction Layer (HAL)
    
    // Configure the system clock
    SystemClock_Config();  // Define this function based on your specific clock configuration

    // Initialize the debug UART
    DebugUartInit();
    
    const XBeeHTable XBeeLRHTable = {
        .PortUartRead = portUartRead,
        .PortUartWrite = portUartWrite,
        .PortMillis = portMillis,
        .PortFlushRx = portFlushRx,
        .PortUartInit = portUartInit,
        .PortDelay = portDelay,
    };

    const XBeeCTable XBeeLRCTable = {
        .OnReceiveCallback = OnReceiveCallback,
        .OnSendCallback = OnSendCallback,
    };

    XBeeLR* myXbeeLr = XBeeLRCreate(&XBeeLRCTable, &XBeeLRHTable);

    if (!XBeeInit((XBee*)myXbeeLr, 9600, NULL)) {
        printf("Failed to initialize XBee\n");
        return -1;
    }

    // Read and print the LoRaWAN Device EUI
    uint8_t devEui[17];
    XBeeLRGetDevEUI((XBee*)myXbeeLr, devEui, sizeof(devEui));
    printf("DEVEUI: %s\n", devEui);

    // Set LoRaWAN network settings
    printf("Configuring...\n");
    XBeeLRSetAppEUI((XBee*)myXbeeLr, "37D56A3F6CDCF0A5");
    XBeeLRSetAppKey((XBee*)myXbeeLr, "CD32AAB41C54175E9060D86F3A8B7F48");
    XBeeLRSetNwkKey((XBee*)myXbeeLr, "CD32AAB41C54175E9060D86F3A8B7F48");
    XBeeWriteConfig((XBee*)myXbeeLr);
    XBeeApplyChanges((XBee*)myXbeeLr);

    // Connect to the LoRaWAN network
    printf("Connecting...\n");
    if (!XBeeConnect((XBee*)myXbeeLr)) {
        printf("Failed to connect.\n");
        return -1;
    } else {
        printf("Connected!\n");
    }

    // Prepare the XBeeLR payload to send
    uint8_t examplePayload[] = {0xC0, 0xC0, 0xC0, 0xFF, 0xEE};
    uint16_t payloadLen = sizeof(examplePayload) / sizeof(examplePayload[0]);
    XBeeLRPacket_t packet = {
        .payload = examplePayload,
        .payloadSize = payloadLen,
        .port = 2,
        .ack = 0,
    };

    uint32_t startTime = portMillis();
    while (1) {
        XBeeProcess((XBee*)myXbeeLr);

        // Check if 10 seconds have passed
        if (portMillis() - startTime >= 10000) {
            if (XBeeConnected((XBee*)myXbeeLr)) {
                printf("Sending 0x");
                for (int i = 0; i < payloadLen; i++) {
                    printf("%02X", examplePayload[i]);
                }
                printf("\n");

                if (XBeeSendData((XBee*)myXbeeLr, &packet)) {
                    printf("Failed to send data.\n");
                } else {
                    printf("Data sent successfully.\n");
                }

                // Update payload
                packet.payload[0] = packet.payload[0] + 1; // Increment payload for next transmission
            } else {
                printf("Not connected. Reconnecting...\n");
                if (!XBeeConnect((XBee*)myXbeeLr)) {
                    printf("Failed to reconnect.\n");
                }
            }
            startTime = portMillis();  // Reset the start time
        }
    }
}
