/**
 * @file example.ino
 * @brief Example application demonstrating the use of the XBee library on Arduino.
 * 
 * This file contains a sample application that demonstrates how to use the XBee library 
 * to communicate with XBee modules on an Arduino platform. It showcases basic operations 
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
#include <Arduino.h>

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
    Serial.print("Received Packet: ");
    for (int i = 1; i < packet->payloadSize; i++) {
        Serial.print(packet->payload[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    Serial.print("Ack: ");
    Serial.println(packet->ack);
    Serial.print("Port: ");
    Serial.println(packet->port);
    Serial.print("RSSI: ");
    Serial.println(packet->rssi);
    Serial.print("SNR: ");
    Serial.println(packet->snr);
    Serial.print("Downlink Counter: ");
    Serial.println(packet->counter);
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
            Serial.print("Send successful (frameId: 0x");
            Serial.print(packet->frameId, HEX);
            Serial.println(")");
            break;
        case 0x01:
            Serial.print("Send failed (frameId: 0x");
            Serial.print(packet->frameId, HEX);
            Serial.println(") (reason: Ack Failed)");
            break;
        case 0x022:
            Serial.print("Send failed (frameId: 0x");
            Serial.print(packet->frameId, HEX);
            Serial.println(") (reason: Not Connected)");
            break;
        default:
            Serial.print("Send failed (frameId: 0x");
            Serial.print(packet->frameId, HEX);
            Serial.print(") (reason: 0x");
            Serial.print(packet->status, HEX);
            Serial.println(")");
            break;
    }
}

// Global XBeeLR instance
XBeeLR* my_xbee_lr;

// XBeeLR payload to send
uint8_t example_payload[] = {0xC0, 0xC0, 0xC0, 0xFF, 0xEE};
uint16_t payload_len;
XBeeLRPacket_t packet;

void setup() {
    // Initialize the Arduino Serial port for debugging
    Serial.begin(9600);

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

    // Create an instance of the XBeeLR class
    my_xbee_lr = XBeeLR_Create(&XBeeLR_CTable, &XBeeLR_HTable);

    // Initialize the XBee module
    if (!XBee_Init((XBee*)my_xbee_lr, 9600, NULL)) {
        Serial.println("Failed to initialize XBee");
        while (1); // Halt the program on failure
    }

    // Read LoRaWAN DevEUI and print
    uint8_t dev_eui[17];
    XBeeLR_GetDevEUI((XBee*)my_xbee_lr, dev_eui, sizeof(dev_eui));
    Serial.print("DEVEUI: ");
    Serial.println((char*)dev_eui);

    // Set LoRaWAN Network Settings
    Serial.println("Configuring...");
    XBeeLR_SetAppEUI((XBee*)my_xbee_lr, "37D56A3F6CDCF0A5");
    XBeeLR_SetAppKey((XBee*)my_xbee_lr, "CD32AAB41C54175E9060D86F3A8B7F48");
    XBeeLR_SetNwkKey((XBee*)my_xbee_lr, "CD32AAB41C54175E9060D86F3A8B7F48");
    XBee_WriteConfig((XBee*)my_xbee_lr);
    XBee_ApplyChanges((XBee*)my_xbee_lr);

    // Connect to LoRaWAN network
    Serial.println("Connecting...");
    if (!XBee_Connect((XBee*)my_xbee_lr)) {
        Serial.println("Failed to connect.");
        while (1); // Halt the program on failure
    } else {
        Serial.println("Connected!");
    }

    // Initialize the payload
    payload_len = sizeof(example_payload) / sizeof(example_payload[0]);
    packet.payload = example_payload;
    packet.payloadSize = payload_len;
    packet.port = 2;
    packet.ack = 0;
}

void loop() {
    // Let XBee class process any serial data
    XBee_Process((XBee*)my_xbee_lr);

    // Check if 10 seconds have passed
    static uint32_t start_time = millis();
    if (millis() - start_time >= 10000) {
        // Send data if connected, else reconnect
        if (XBee_Connected((XBee*)my_xbee_lr)) {
            Serial.print("Sending 0x");
            for (int i = 0; i < payload_len; i++) {
                Serial.print(example_payload[i], HEX);
            }
            Serial.println();

            if (XBee_SendData((XBee*)my_xbee_lr, &packet)) {
                Serial.println("Failed to send data.");
            } else {
                Serial.println("Data sent successfully.");
            }

            // Update payload
            packet.payload[0] = packet.payload[0] + 1; // change payload
        } else {
            Serial.println("Not connected. Connecting...");
            if (!XBee_Connect((XBee*)my_xbee_lr)) {
                Serial.println("Failed to connect.");
            } else {
                Serial.println("Connected!");
            }
        }
        // Reset the start time
        start_time = millis();
    }
}
