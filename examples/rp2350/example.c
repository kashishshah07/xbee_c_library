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
    portDebugPrintf("Received Packet: ");
    for (int i = 0; i < packet->payloadSize; i++) {
        portDebugPrintf("0x%02X ", packet->payload[i]);
    }
    portDebugPrintf("\n");
    portDebugPrintf("Ack %u\n", packet->ack);
    portDebugPrintf("Port %u\n", packet->port);
    portDebugPrintf("RSSI %d\n", packet->rssi);
    portDebugPrintf("SNR %d\n", packet->snr);
    portDebugPrintf("Downlink Counter %lu\n", packet->counter);
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
            portDebugPrintf("Send successful (frameId: 0x%02X)\n",packet->frameId);
            break;
        case 0x01:
            portDebugPrintf("Send failed (frameId: 0x%02X) (reason: Ack Failed)\n",packet->frameId);
            break;
        case 0x022:
            portDebugPrintf("Send failed (frameId: 0x%02X) (reason: Not Connected)\n",packet->frameId);
            break;
        default:
            portDebugPrintf("Send failed (frameId: 0x%02X) (reason: 0x%02X)\n",packet->frameId, packet->status);
            break;
    }
}

int main() {

    // Initialize the RP2350 platform (Pico SDK)
    stdio_init_all();

    // Hardware Abstraction Function Pointer Table for XBeeLR (needs to be set!)
    const XBeeHTable XBeeLRHTable = {
        .PortUartRead = portUartRead,
        .PortUartWrite = portUartWrite,
        .PortMillis = portMillis,
        .PortFlushRx = portFlushRx,
        .PortUartInit = portUartInit,
        .PortDelay = portDelay,
    };

    // Callback Function Pointer Table for XBeeLR
    const XBeeCTable XBeeLRCTable = {
        .OnReceiveCallback = OnReceiveCallback, // can be left as all NULL if no callbacks needed
        .OnSendCallback = NULL,                 // can be left as all NULL if no callbacks needed
    };

    portDelay(20000); //workaround to give time for debug usb port connection after booting

    portDebugPrintf("XBee LR Example App\n");

    // Create an instance of the XBeeLR class
    XBeeLR* myXbeeLr = XBeeLRCreate(&XBeeLRCTable, &XBeeLRHTable);


    // Initialize the XBee module
    if (!XBeeInit((XBee*)myXbeeLr, 9600, NULL)) {
        portDebugPrintf("Failed to initialize XBee\n");
        return -1;
    }

    // Read LoRaWAN DevEUI and print
    uint8_t devEui[17];
    if(!XBeeLRGetDevEUI((XBee*)myXbeeLr, devEui, sizeof(devEui))){
        XBeeLRGetDevEUI((XBee*)myXbeeLr, devEui, sizeof(devEui));
    }
    portDebugPrintf("DEVEUI: %s\n", devEui);

    // Set LoRaWAN Network Settings
    portDebugPrintf("Configuring...\n");
    XBeeLRSetAppEUI((XBee*)myXbeeLr, "37D56A3F6CDCF0A5");
    XBeeLRSetAppKey((XBee*)myXbeeLr, "CD32AAB41C54175E9060D86F3A8B7F48");
    XBeeLRSetNwkKey((XBee*)myXbeeLr, "CD32AAB41C54175E9060D86F3A8B7F48");
    XBeeSetAPIOptions((XBee*)myXbeeLr, (const uint8_t[]){0x01});
    XBeeWriteConfig((XBee*)myXbeeLr);
    XBeeApplyChanges((XBee*)myXbeeLr);

    // Connect to LoRaWAN network
    portDebugPrintf("Connecting...\n");
    if (!XBeeConnect((XBee*)myXbeeLr)) {
        portDebugPrintf("Failed to connect.\n");
    } else {
        portDebugPrintf("Connected!\n");
    }

    // XBeeLR payload to send
    uint8_t examplePayload[] = {0xC0, 0xC0, 0xC0, 0xFF, 0xEE};
    uint16_t payloadLen = sizeof(examplePayload) / sizeof(examplePayload[0]);
    XBeeLRPacket_t packet = {
        .payload = examplePayload,
        .payloadSize = payloadLen,
        .port = 2,
        .ack = 0,
    };

    // Main loop
    uint32_t startTime = portMillis();
    while (1) {
        // Let XBee class process any serial data
        XBeeProcess((XBee*)myXbeeLr);

        // Check if 10 seconds have passed
        if (portMillis() - startTime >= 10000) {
            // Send data if connected, else reconnect
            if (XBeeConnected((XBee*)myXbeeLr)) {
                portDebugPrintf("Sending 0x");
                for (int i = 0; i < payloadLen; i++) {
                    portDebugPrintf("%02X", examplePayload[i]);
                }
                portDebugPrintf("\n");
                if (XBeeSendData((XBee*)myXbeeLr, &packet)) {
                    portDebugPrintf("Failed to send data.\n");
                } else {
                    portDebugPrintf("Data sent successfully.\n");
                }

                // Update payload
                packet.payload[0] = packet.payload[0] + 1; // change payload
            } else {
                portDebugPrintf("Not connected. Connecting...\n");
                if (!XBeeConnect((XBee*)myXbeeLr)) {
                    portDebugPrintf("Failed to connect.\n");
                } else {
                    portDebugPrintf("Connected!\n");
                }
            }
            // Reset the start time
            startTime = portMillis();
        }
        portDelay(1);
    }
    return 0;
}
