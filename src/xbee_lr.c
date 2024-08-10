/**
 * @file xbee_lr.c
 * @brief XBee LR Subclass Source 
 * @version 1.0
 * @date 2024-08-08
 * 
 * @license MIT
 * 
 * @author Felix Galindo, Digi International
 * @contact felix.galindo@digi.com
 */

#include "xbee_lr.h"
#include "xbee_api_frames.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static void SendTxReqApiFrame(const uint8_t* payload, uint16_t payload_len, uint8_t port, uint8_t options);
static void SendJoinReqApiFrame();

// XBeeLR specific implementations
void XBeeLR_Connect(XBee* self) {
    // Implement XBeeLR specific connection logic using netConfig
    SendJoinReqApiFrame();
}

void XBeeLR_Disconnect(XBee* self) {
    // Implement XBeeLR specific disconnection logic
}

void XBeeLR_SendData(XBee* self, const uint8_t* data, uint16_t length) {
    // Call the SendTxReqApiFrame function with the example payload
    SendTxReqApiFrame(data, length, 1 ,0);
}

void XBeeLR_SoftReset(XBee* self) {
    // Implement XBeeLR specific soft reset logic
}

void XBeeLR_HardReset(XBee* self) {
    // Implement XBeeLR specific hard reset logic
}


void XBeeLR_Process(XBee* self) {
    // Implement XBeeLR specific process logic
    xbee_api_frame_t frame;
    int status = api_receive_api_frame(&frame);
    if (status == 0) {
        api_handle_frame(frame);
    } else if (status == -1) {
        port_debug_printf("Error receiving frame.\n");
    }
}


void XBeeLR_ConfigureNetwork(XBee* self, const void* config) {
    uint8_t response = 0;
    uint8_t response_length;
    int status;
    status += api_send_at_command_and_get_response(AT_AE, "37D56A3F6CDCF0A5", &response, &response_length, 5000);
    status += api_send_at_command_and_get_response(AT_AK, "BD32AAB41C54175E9060D86F3A8B7F42", &response, &response_length, 5000);
    status += api_send_at_command_and_get_response(AT_NK, "BD32AAB41C54175E9060D86F3A8B7F42", &response, &response_length, 5000);

    if(status == 0){
        status = api_send_at_command_and_get_response(AT_WR, NULL, &response, &response_length, 5000);
        status = api_send_at_command_and_get_response(AT_AC, NULL, &response, &response_length, 5000);
    }else{
        port_debug_printf("Failed to configure network\n");
    }

    // api_send_at_command(AT_AE,"37D56A3F6CDCF0A5", 16);
    // api_send_at_command(AT_AK,"BD32AAB41C54175E9060D86F3A8B7F42", 32);
    // api_send_at_command(AT_NK,"BD32AAB41C54175E9060D86F3A8B7F42", 32);
    // api_send_at_command(AT_WR, NULL, 0);
    // api_send_at_command(AT_AC, NULL, 0);
    // Implement XBeeLR specific network configuration logic
}

void XBeeLR_ConfigureSerial(XBee* self, const void* config) {
    // Implement XBeeLR specific serial configuration logic
}

uint8_t XBeeLR_Connected(XBee* self) {
    // Implement logic to check XBeeLR network connection
    uint8_t response = 0;
    uint8_t response_length;
    int status;

    // Send the AT_JS command to query the Join Status
    status = api_send_at_command_and_get_response(AT_JS, NULL, &response, &response_length, 5000);

    if (status == 0) {
        // Print the received reponse
        // port_debug_printf("ATJS Resp: %u \n", response);
        // port_debug_printf("Join Status: %s \n", response ? "Joined" : "Not Joined");
    } else {
        port_debug_printf("Failed to receive AT_JS response, error code: %d\n", status);
    }
    return response;  
}

bool XBeeLR_GetDevEUI(XBee* self, uint8_t* response_buffer, uint8_t buffer_size) {
    // Clear buffer
    if(buffer_size < 17){
        return false;
    }
    memset(response_buffer,0,buffer_size);

    // Send the AT_DE command to query the DevEUI
    uint8_t response_length;
    int status = api_send_at_command_and_get_response(AT_DE, NULL, response_buffer, &response_length, 5000);

    if (status != 0) {
        port_debug_printf("Failed to receive AT_DE response, error code: %d\n", status);
        return false;
    }
    return true;  
}

// VTable for XBeeLR
const XBeeVTable XBeeLR_VTable = {
    .process = XBeeLR_Process,
    .connect = XBeeLR_Connect,
    .disconnect = XBeeLR_Disconnect,
    .send_data = XBeeLR_SendData,
    .soft_reset = XBeeLR_SoftReset,
    .hard_reset = XBeeLR_HardReset,
    .configure_network = XBeeLR_ConfigureNetwork,
    .configure_serial = XBeeLR_ConfigureSerial,
    .connected = XBeeLR_Connected,
};

// Constructor for XBeeLR
XBeeLR* XBeeLR_Create(void) {
    XBeeLR* instance = (XBeeLR*)malloc(sizeof(XBeeLR));
    instance->base.vtable = &XBeeLR_VTable;
    instance->base.onReceive = NULL;
    instance->base.onConnect = NULL;
    instance->base.onDisconnect = NULL;
    instance->base.onSend = NULL;
    return instance;
}


// XBeeLR private functions
static void SendJoinReqApiFrame() {
    uint8_t frame_id = 1;

    // Call the api_send_frame function to send the Join Request API frame
    api_send_frame(XBEE_API_TYPE_LR_JOIN_REQUEST, &frame_id, 1);
}

static void SendTxReqApiFrame(const uint8_t* payload, uint16_t payload_len, uint8_t port, uint8_t options) {
    uint8_t frame_data[128]; // Adjust size as needed based on the frame structure

    // Example frame data for Tx Request
    frame_data[0] = 0x01;  // Frame ID
    frame_data[1] = port;  // LoRaWAN Port
    frame_data[2] = options;  // Transmit Options
    // Add the payload to the frame data
    memcpy(&frame_data[3], payload, payload_len);

    // Call the api_send_frame function to send the Tx Request API frame
    api_send_frame(XBEE_API_TYPE_LR_TX_REQUEST, frame_data, 3 + payload_len);
}