/**
 * @file xbee_lr.c
 * @brief Implementation of XBee LR (LoRaWAN) subclass.
 * 
 * This file contains the implementation of functions specific to the XBee LR module.
 * It includes methods for initializing, sending join requests, and handling other
 * operations unique to the XBee LR subclass.
 * 
 * @version 1.0
 * @date 2024-08-08
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

#include "xbee_lr.h"
#include "xbee_api_frames.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void SendJoinReqApiFrame(XBee* self);

// XBeeLR specific implementations
bool XBeeLR_Init(XBee* self, uint32_t baudrate, const char* device) {
    // Implement XBeeLR initialization
    return self->htable->PortUartInit(baudrate, device) == 0 ? true:false;
}

//Attempts to Connect to network
//Blocking function. @todo: add option for non-blocking
bool XBeeLR_Connect(XBee* self) {
    // Implement XBeeLR specific connection logic using netConfig
    SendJoinReqApiFrame(self);

    //@todo check modem status to determine of successful or not
    return true;
}

bool XBeeLR_Disconnect(XBee* self) {
    // Implement XBeeLR specific disconnection logic
    return true;
}

//Sends data over network
//Blocking function. @todo: add option for non-blocking
bool XBeeLR_SendData(XBee* self, const void* data) {
    // Call the SendTxReqApiFrame function with the example payload
    XBeeLRPacket_t *packet = (XBeeLRPacket_t*) data;
    uint8_t frame_data[128]; // Adjust size as needed based on the frame structure

    // Example frame data for Tx Request
    packet->frameId = self->frameIdCntr;
    frame_data[0] = self->frameIdCntr;  // Frame ID
    frame_data[1] = packet->port;  // LoRaWAN Port
    frame_data[2] = packet->ack & 0x01;  // Transmit Options
    // Add the payload to the frame data
    memcpy(&frame_data[3], packet->payload, packet->payloadSize);

    // Call the api_send_frame function to send the Tx Request API frame
    api_send_frame(self, XBEE_API_TYPE_LR_TX_REQUEST, frame_data, 3 + packet->payloadSize);

    //@todo check transmit status to determine of successful or not
    return true;
}

bool XBeeLR_SoftReset(XBee* self) {
    // Implement XBeeLR specific soft reset logic
    return true;
}

void XBeeLR_HardReset(XBee* self) {
    // Implement XBeeLR specific hard reset logic
}

//Must be called continously
void XBeeLR_Process(XBee* self) {
    // Implement XBeeLR specific process logic
    xbee_api_frame_t frame;
    int status = api_receive_api_frame(self,&frame);
    if (status == API_SEND_SUCCESS) {
        api_handle_frame(self,frame);
    } else {
        port_debug_printf("Error receiving frame.\n");
    }
}

//Check if connected to network
//true if connected
bool XBeeLR_Connected(XBee* self) {
    // Implement logic to check XBeeLR network connection
    uint8_t response = 0;
    uint8_t response_length;
    int status;

    // Send the AT_JS command to query the Join Status
    status = api_send_at_command_and_get_response(self, AT_JS, NULL, &response, &response_length, 5000);

    if (status == API_SEND_SUCCESS) {
        // Print the received reponse
        // port_debug_printf("ATJS Resp: %u \n", response);
        // port_debug_printf("Join Status: %s \n", response ? "Joined" : "Not Joined");
    } else {
        port_debug_printf("Failed to receive AT_JS response, error code: %d\n", status);
    }
    return response;  
}

/* XBeeLR Specific Functions */

//Sends ATAE cmd to set LoRaWAN AppEUI
//Blocking function. Waits until get response or timeouts
bool XBeeLR_SetAppEUI(XBee* self, const char* value) {
    uint8_t response[17];
    uint8_t response_length;
    int status = api_send_at_command_and_get_response(self, AT_AE, value, response, &response_length, 5000);
    if(status != API_SEND_SUCCESS){
        port_debug_printf("Failed to set App EUI\n");
    }
    return status;
}

//Sends ATAK cmd to set LoRaWAN AppKey
//Blocking function. Waits until get response or timeouts
bool XBeeLR_SetAppKey(XBee* self, const char* value) {
    uint8_t response[33];
    uint8_t response_length;
    int status = api_send_at_command_and_get_response(self, AT_AK, value, response, &response_length, 5000);
    if(status != API_SEND_SUCCESS){
        port_debug_printf("Failed to set App Key\n");
    }
    return status;
}

//Sends ATNK cmd to set LoRaWAN NwkKey
//Blocking function. Waits until get response or timeouts
bool XBeeLR_SetNwkKey(XBee* self, const char* value) {
    uint8_t response[33];
    uint8_t response_length;
    int status = api_send_at_command_and_get_response(self, AT_NK, value, response, &response_length, 5000);
    if(status != API_SEND_SUCCESS){
        port_debug_printf("Failed to set Nwk Key\n");
    }
    return status;
}

//Sends ATDE cmd to read LoRaWAN DevEUI
//Blocking function. Waits until get response or timeouts
bool XBeeLR_GetDevEUI(XBee* self, uint8_t* response_buffer, uint8_t buffer_size) {
    // Clear buffer
    if(buffer_size < 17){
        return false;
    }
    memset(response_buffer,0,buffer_size);

    // Send the AT_DE command to query the DevEUI
    uint8_t response_length;
    int status = api_send_at_command_and_get_response(self, AT_DE, NULL, response_buffer, &response_length, 5000);

    if (status != API_SEND_SUCCESS) {
        port_debug_printf("Failed to receive AT_DE response, error code: %d\n", status);
        return false;
    }
    return true;  
}

// XBeeLR private functions

/**
 * @brief Sends a join request API frame.
 * 
 * This function sends a join request API frame to the XBee network,
 * which is part of the joining process for LR modules.
 * 
 * @param[in] self Pointer to the XBee instance.
 */
static void SendJoinReqApiFrame(XBee* self) {
    uint8_t frame_id = self->frameIdCntr;

    // Call the api_send_frame function to send the Join Request API frame
    api_send_frame(self, XBEE_API_TYPE_LR_JOIN_REQUEST, &frame_id, 1);
}

//Parses RX_PACKET frame and calls recieve callback function
static void XBeeLR_Handle_Rx_Packet(XBee* self, void *param) {

    if(param == NULL) return;

    xbee_api_frame_t *frame = (xbee_api_frame_t *)param;
    if (frame->type != XBEE_API_TYPE_LR_RX_PACKET && frame->type != XBEE_API_TYPE_LR_EXPLICIT_RX_PACKET) return;

    XBeeLRPacket_t * packet = (XBeeLRPacket_t*)malloc(sizeof(XBeeLRPacket_t));
    memset(packet,0,sizeof(XBeeLRPacket_t));

    API_FRAME_DEBUG_PRINT("Received Packet: ");
    for (int i = 1; i < frame->length; i++) {
        API_FRAME_DEBUG_PRINT("%02X ", frame->data[i]);
    }
    API_FRAME_DEBUG_PRINT("\n");

    if(frame->type == XBEE_API_TYPE_LR_EXPLICIT_RX_PACKET){
        packet->port = frame->data[1];
        packet->rssi = frame->data[2];
        packet->snr = frame->data[3];
        packet->dr = frame->data[4];
        packet->counter = frame->data[5] << 24 | frame->data[6] << 16 | frame->data[7] << 8 | frame->data[8];
        packet->payload = &(frame->data[9]);
        packet->payloadSize = frame->length - 9;
    }else{
        packet->port = frame->data[1];
        packet->payload = &(frame->data[2]);
        packet->payloadSize = frame->length - 2;
    }

    if(self->ctable->OnReceiveCallback){
        self->ctable->OnReceiveCallback(self,packet);
    }

    free(packet);
    // Add further processing as needed
}

void XBeeLR_Handle_Transmit_Status(XBee* self, void *param) {

    if(param == NULL) return;

    xbee_api_frame_t *frame = (xbee_api_frame_t *)param;
    if (frame->type != XBEE_API_TYPE_TX_STATUS) return;

    XBeeLRPacket_t * packet = (XBeeLRPacket_t*)malloc(sizeof(XBeeLRPacket_t));
    memset(packet,0,sizeof(XBeeLRPacket_t));

    API_FRAME_DEBUG_PRINT("Received Transmit Status Frame: ");
    for (int i = 1; i < frame->length; i++) {
        API_FRAME_DEBUG_PRINT("0x%02X ", frame->data[i]);
    }
    API_FRAME_DEBUG_PRINT("\n");

    packet->frameId = frame->data[1];
    packet->status = frame->data[2];

    if(self->ctable->OnSendCallback){
        self->ctable->OnSendCallback(self,packet);
    }

    // Add further processing as needed
}

// VTable for XBeeLR
const XBeeVTable XBeeLR_VTable = {
    .init = XBeeLR_Init,
    .process = XBeeLR_Process,
    .connect = XBeeLR_Connect,
    .disconnect = XBeeLR_Disconnect,
    .send_data = XBeeLR_SendData,
    .soft_reset = XBeeLR_SoftReset,
    .hard_reset = XBeeLR_HardReset,
    .connected = XBeeLR_Connected,
    .handle_rx_packet_frame = XBeeLR_Handle_Rx_Packet,
    .handle_transmit_status_frame = XBeeLR_Handle_Transmit_Status,
};


// Constructor for XBeeLR
XBeeLR* XBeeLR_Create(const XBeeCTable* cTable, const XBeeHTable* hTable) {
    XBeeLR* instance = (XBeeLR*)malloc(sizeof(XBeeLR));
    instance->base.vtable = &XBeeLR_VTable;
    instance->base.htable = hTable;
    instance->base.ctable = cTable;
    return instance;
}
