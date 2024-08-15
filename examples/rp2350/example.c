/**
 * @file example.c
 * @brief Example application demonstrating the use of the XBee library on RP2350 (Raspberry Pi Pico).
 * 
 * This file contains a sample application that demonstrates how to use the XBee library 
 * to communicate with XBee modules on a RP2350 (Raspberry Pi Pico). It showcases basic operations 
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
#include <pico/stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Callback function triggered when data is received from the XBee module.
 * 
 * This function is called when the XBee module receives data. It processes the 
 * incoming data, extracts relevant information, and handles it accordingly. 
 * The function is typically registered as a callback to be executed automatically 
 * upon data reception.
 * 
 * @param[in] frame Pointer to the received XBee API frame.
 * 
 * @return void This function does not return a value.
 */
void OnReceiveCallback(XBee* self, void* data) {
    XBeeLRPacket_t* packet = (XBeeLRPacket_t*) data;
    port_debug_printf("Received Packet: ");
    for (int i = 0; i < packet->payloadSize; i++) {
        port_debug_printf("0x%02X ", packet->payload[i]);
    }
    port_debug_printf("\n");
    port_debug_printf("Ack %u\n", packet->ack);
    port_debug_printf("Port %u\n", packet->port);
    port_debug_printf("RSSI %d\n", packet->rssi);
    port_debug_printf("SNR %d\n", packet->snr);
    port_debug_printf("Downlink Counter %lu\n", packet->counter);
}

/**
 * @brief Callback function triggered after data is sent from the XBee module.
 * 
 * This function is called when the XBee module completes sending data. It handles 
 * any post-send operations, such as logging the transmission status or updating 
 * the state of the application. The function is typically registered as a callback 
 * to be executed automatically after data is transmitted.
 * 
 * @param[in] frameId The ID of the API frame that was sent.
 * @param[in] status The status code indicating the result of the send operation.
 * 
 * @return void This function does not return a value.
 */
void OnSendCallback(XBee* self, void* data) {
    XBeeLRPacket_t* packet = (XBeeLRPacket_t*) data;
    switch(packet->status){
        case 0:
            port_debug_printf("Send successful (frameId: 0x%02X)\n",packet->frameId);
            break;
        case 0x01:
            port_debug_printf("Send failed (frameId: 0x%02X) (reason: Ack Failed)\n",packet->frameId);
            break;
        case 0x022:
            port_debug_printf("Send failed (frameId: 0x%02X) (reason: Not Connected)\n",packet->frameId);
            break;
        default:
            port_debug_printf("Send failed (frameId: 0x%02X) (reason: 0x%02X)\n",packet->frameId, packet->status);
            break;
    }
}

int main() {

    // Initialize the RP2350 platform (Pico SDK)
    stdio_init_all();

    // Hardware Abstraction Function Pointer Table for XBeeLR (needs to be set!)
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
        .OnReceiveCallback = OnReceiveCallback, // can be left as all NULL if no callbacks needed
        .OnSendCallback = NULL,                 // can be left as all NULL if no callbacks needed
    };

    port_delay(20000); //workaround to give time for debug usb port connection after booting

    port_debug_printf("XBee LR Example App\n");

    // Create an instance of the XBeeLR class
    XBeeLR* my_xbee_lr = XBeeLR_Create(&XBeeLR_CTable, &XBeeLR_HTable);


    // Initialize the XBee module
    if (!XBee_Init((XBee*)my_xbee_lr, 9600, NULL)) {
        port_debug_printf("Failed to initialize XBee\n");
        return -1;
    }

    // Read LoRaWAN DevEUI and print
    uint8_t dev_eui[17];
    if(!XBeeLR_GetDevEUI((XBee*)my_xbee_lr, dev_eui, sizeof(dev_eui))){
        XBeeLR_GetDevEUI((XBee*)my_xbee_lr, dev_eui, sizeof(dev_eui));
    }
    port_debug_printf("DEVEUI: %s\n", dev_eui);

    // Set LoRaWAN Network Settings
    port_debug_printf("Configuring...\n");
    XBeeLR_SetAppEUI((XBee*)my_xbee_lr, "37D56A3F6CDCF0A5");
    XBeeLR_SetAppKey((XBee*)my_xbee_lr, "CD32AAB41C54175E9060D86F3A8B7F48");
    XBeeLR_SetNwkKey((XBee*)my_xbee_lr, "CD32AAB41C54175E9060D86F3A8B7F48");
    XBeeLR_SetAPIOptions((XBee*)my_xbee_lr, (const uint8_t[]){0x01});
    XBee_WriteConfig((XBee*)my_xbee_lr);
    XBee_ApplyChanges((XBee*)my_xbee_lr);

    // Connect to LoRaWAN network
    port_debug_printf("Connecting...\n");
    if (!XBee_Connect((XBee*)my_xbee_lr)) {
        port_debug_printf("Failed to connect.\n");
    } else {
        port_debug_printf("Connected!\n");
    }

    // XBeeLR payload to send
    uint8_t example_payload[] = {0xC0, 0xC0, 0xC0, 0xFF, 0xEE};
    uint16_t payload_len = sizeof(example_payload) / sizeof(example_payload[0]);
    XBeeLRPacket_t packet = {
        .payload = example_payload,
        .payloadSize = payload_len,
        .port = 2,
        .ack = 0,
    };

    // Main loop
    uint32_t start_time = port_millis();
    while (1) {
        // Let XBee class process any serial data
        XBee_Process((XBee*)my_xbee_lr);

        // Check if 10 seconds have passed
        if (port_millis() - start_time >= 10000) {
            // Send data if connected, else reconnect
            if (XBee_Connected((XBee*)my_xbee_lr)) {
                port_debug_printf("Sending 0x");
                for (int i = 0; i < payload_len; i++) {
                    port_debug_printf("%02X", example_payload[i]);
                }
                port_debug_printf("\n");
                if (XBee_SendData((XBee*)my_xbee_lr, &packet)) {
                    port_debug_printf("Failed to send data.\n");
                } else {
                    port_debug_printf("Data sent successfully.\n");
                }

                // Update payload
                packet.payload[0] = packet.payload[0] + 1; // change payload
            } else {
                port_debug_printf("Not connected. Connecting...\n");
                if (!XBee_Connect((XBee*)my_xbee_lr)) {
                    port_debug_printf("Failed to connect.\n");
                } else {
                    port_debug_printf("Connected!\n");
                }
            }
            // Reset the start time
            start_time = port_millis();
        }
        port_delay(1);
    }
    return 0;
}
