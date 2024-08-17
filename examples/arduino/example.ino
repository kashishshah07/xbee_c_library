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
XBeeLR* myXbeeLr;

// XBeeLR payload to send
uint8_t examplePayload[] = {0xC0, 0xC0, 0xC0, 0xFF, 0xEE};
uint16_t payloadLen;
XBeeLRPacket_t packet;

void setup() {
    // Initialize the Arduino Serial port for debugging
    Serial.begin(9600);

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

    // Create an instance of the XBeeLR class
    myXbeeLr = XBeeLRCreate(&XBeeLRCTable, &XBeeLRHTable);

    // Initialize the XBee module
    if (!XBeeInit((XBee*)myXbeeLr, 9600, NULL)) {
        Serial.println("Failed to initialize XBee");
        while (1); // Halt the program on failure
    }

    // Read LoRaWAN DevEUI and print
    uint8_t devEui[17];
    XBeeLRGetDevEUI((XBee*)myXbeeLr, devEui, sizeof(devEui));
    Serial.print("DEVEUI: ");
    Serial.println((char*)devEui);

    // Set LoRaWAN Network Settings
    Serial.println("Configuring...");
    XBeeLRSetAppEUI((XBee*)myXbeeLr, "37D56A3F6CDCF0A5");
    XBeeLRSetAppKey((XBee*)myXbeeLr, "CD32AAB41C54175E9060D86F3A8B7F48");
    XBeeLRSetNwkKey((XBee*)myXbeeLr, "CD32AAB41C54175E9060D86F3A8B7F48");
    XBeeWriteConfig((XBee*)myXbeeLr);
    XBeeApplyChanges((XBee*)myXbeeLr);

    // Connect to LoRaWAN network
    Serial.println("Connecting...");
    if (!XBeeConnect((XBee*)myXbeeLr)) {
        Serial.println("Failed to connect.");
        while (1); // Halt the program on failure
    } else {
        Serial.println("Connected!");
    }

    // Initialize the payload
    payloadLen = sizeof(examplePayload) / sizeof(examplePayload[0]);
    packet.payload = examplePayload;
    packet.payloadSize = payloadLen;
    packet.port = 2;
    packet.ack = 0;
}

void loop() {
    // Let XBee class process any serial data
    XBeeProcess((XBee*)myXbeeLr);

    // Check if 10 seconds have passed
    static uint32_t startTime = millis();
    if (millis() - startTime >= 10000) {
        // Send data if connected, else reconnect
        if (XBeeConnected((XBee*)myXbeeLr)) {
            Serial.print("Sending 0x");
            for (int i = 0; i < payloadLen; i++) {
                Serial.print(examplePayload[i], HEX);
            }
            Serial.println();

            if (XBeeSendData((XBee*)myXbeeLr, &packet)) {
                Serial.println("Failed to send data.");
            } else {
                Serial.println("Data sent successfully.");
            }

            // Update payload
            packet.payload[0] = packet.payload[0] + 1; // change payload
        } else {
            Serial.println("Not connected. Connecting...");
            if (!XBeeConnect((XBee*)myXbeeLr)) {
                Serial.println("Failed to connect.");
            } else {
                Serial.println("Connected!");
            }
        }
        // Reset the start time
        startTime = millis();
    }
}
