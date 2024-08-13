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


/**
 * @brief Checks if the XBee LR module is connected to the LoRaWAN network.
 * 
 * This function sends an AT command (`AT_JS`) to the XBee LR module to query the 
 * Join Status, determining whether the module is currently connected to the LoRaWAN network. 
 * It returns true if the module is connected (i.e., has joined the network) and false otherwise. 
 * The function also handles the communication with the module and provides debug output in case 
 * of communication errors.
 * 
 * @param[in] self Pointer to the XBee instance.
 * 
 * @return bool Returns true if the XBee LR module is connected to the network, otherwise false.
 */
bool XBeeLR_Connected(XBee* self) {
    // Implement logic to check XBeeLR network connection
    uint8_t response = 0;
    uint8_t response_length;
    int status;

    // Send the AT_JS command to query the Join Status
    status = api_send_at_command_and_get_response(self, AT_JS, NULL, &response, &response_length, 5000);

    if (status == API_SEND_SUCCESS) {
        // Print the received reponse
        // XBEE_DEBUG_PRINT_ENABLED("ATJS Resp: %u \n", response);
        // XBEE_DEBUG_PRINT_ENABLED("Join Status: %s \n", response ? "Joined" : "Not Joined");
    } else {
        XBEE_DEBUG_PRINT_ENABLED("Failed to receive AT_JS response, error code: %d\n", status);
    }
    return response;  
}

/**
 * @brief Initializes the XBee LR module for communication.
 * 
 * This function initializes the XBee LR module by setting up the necessary 
 * serial communication parameters, such as the baud rate and device path. 
 * It relies on platform-specific UART initialization provided by the hardware 
 * abstraction layer. The function returns true if initialization is successful.
 * 
 * @param[in] self Pointer to the XBee instance.
 * @param[in] baudrate The baud rate for serial communication.
 * @param[in] device The path to the serial device (e.g., "/dev/ttyUSB0").
 * 
 * @return bool Returns true if the initialization is successful, otherwise false.
 */
bool XBeeLR_Init(XBee* self, uint32_t baudrate, const char* device) {
    // Implement XBeeLR initialization
    return (self->htable->PortUartInit(baudrate, device)) == UART_SUCCESS ? true:false;
}

/**
 * @brief Processes incoming data and events for the XBee LR module.
 * 
 * This function must be called continuously in the main loop of the application. 
 * It handles the reception and processing of API frames from the XBee LR module.
 * The function checks for incoming frames, and if a frame is successfully received,
 * it is processed accordingly. 
 * 
 * @param[in] self Pointer to the XBee instance.
 * 
 * @return void This function does not return a value.
 */
void XBeeLR_Process(XBee* self) {
    // Implement XBeeLR specific process logic
    xbee_api_frame_t frame;
    int status = api_receive_api_frame(self,&frame);
    if (status == API_SEND_SUCCESS) {
        api_handle_frame(self,frame);
    } else if (status != API_RECEIVE_ERROR_TIMEOUT_START_DELIMITER) {
        XBEE_DEBUG_PRINT_ENABLED("Error receiving frame.\n");
    }
}


/**
 * @brief Attempts to connect to the LoRaWAN network using the XBee LR module.
 * 
 * This function initiates the connection process to a LoRaWAN network by sending 
 * a join request. The function is currently blocking, meaning it waits until 
 * the connection attempt is completed before returning. A future enhancement (@todo) 
 * could add support for non-blocking operation.
 * 
 * @param[in] self Pointer to the XBee instance.
 * 
 * @return bool Returns true if the connection process was initiated
 * 
 */
bool XBeeLR_Connect(XBee* self) {
    // Implement XBeeLR specific connection logic 
    SendJoinReqApiFrame(self);

    // Start the timeout timer
    uint32_t start_time = port_millis();

    while ((port_millis() - start_time) < CONNECTION_TIMEOUT_MS) {
        if (XBeeLR_Connected(self)) {
            XBEE_DEBUG_PRINT_ENABLED("Successfully Joined\n");
            return true; // Successfully joined
        }

        port_delay(500); // Delay between checks 
    }

    XBEE_DEBUG_PRINT_ENABLED("Failed to Join\n");
    return false; // Timeout reached without successful join
}

/**
 * @brief Disconnects from the LoRaWAN network using the XBee LR module.
 * 
 * This function handles the disconnection process from a LoRaWAN network. It ensures 
 * that the XBee LR module is properly disconnected and that any necessary cleanup 
 * is performed. The function is currently blocking.
 * 
 * @param[in] self Pointer to the XBee instance.
 * 
 * @return bool Returns true if the disconnection process was initiated.
 */
bool XBeeLR_Disconnect(XBee* self) {
    // Implement XBeeLR specific disconnection logic
    return true;
}

/**
 * @brief Sends data over the network using the XBee LR module.
 * 
 * This function constructs and sends a data packet over the network using an XBee LR module.
 * The function is currently blocking, meaning it waits until the data is fully transmitted
 * before returning. A future enhancement (@todo) could add support for non-blocking operation.
 * 
 * @param[in] self Pointer to the XBee instance.
 * @param[in] data Pointer to the data to be sent, encapsulated in an XBeeLRPacket_t structure.
 * 
 * @return xbee_delivery_status_t, 0 if successful
 * 
 */
uint8_t XBeeLR_SendData(XBee* self, const void* data) {
    // Prepare and send the API frame
    XBeeLRPacket_t *packet = (XBeeLRPacket_t*) data;
    uint8_t frame_data[128];  // Adjust size as needed
    packet->frameId = self->frameIdCntr;
    frame_data[0] = self->frameIdCntr;
    frame_data[1] = packet->port;
    frame_data[2] = packet->ack & 0x01;
    memcpy(&frame_data[3], packet->payload, packet->payloadSize);

    // Send the frame
    int send_status = api_send_frame(self, XBEE_API_TYPE_LR_TX_REQUEST, frame_data, 3 + packet->payloadSize);
    if (send_status != API_SEND_SUCCESS) {
        return false;  // Failed to send the frame
    }

    // Block and wait for the XBEE_API_TYPE_TX_STATUS frame
    uint32_t start_time = port_millis();  // Get the current time in milliseconds

    self->tx_status_received = false;  // Reset the status flag before waiting

    while ((port_millis() - start_time) < SEND_DATA_TIMEOUT_MS) {
        // Process incoming frames using XBeeLR_Process
        XBeeLR_Process(self);

        // Check if the status frame was received
        if (self->tx_status_received) {
            // Return the delivery status
            XBEE_DEBUG_PRINT_ENABLED("TX Delivery Status 0x%02X\n", self->delivery_status );
            return self->delivery_status;
        }

        // Add a small delay here to avoid busy-waiting
        port_delay(10);  // Delay for 10 ms
    }

    // Timeout reached without receiving the expected frame
    XBEE_DEBUG_PRINT_ENABLED("Failed to receive TX Request Status frame\n");
    return 0xFF;  // Indicate failure or timeout
}

bool XBeeLR_SoftReset(XBee* self) {
    // Implement XBeeLR specific soft reset logic
    return true;
}

void XBeeLR_HardReset(XBee* self) {
    // Implement XBeeLR specific hard reset logic
}

/* XBeeLR Specific Functions */

/**
 * @brief Sends the AT_AE command to set the LoRaWAN AppEUI on the XBee LR module.
 * 
 * This function configures the LoRaWAN AppEUI (Application Identifier) on the XBee LR module 
 * by sending the AT command `AT_AE` with the specified AppEUI value. The function is blocking, 
 * meaning it waits for a response from the module or until a timeout occurs. If the command 
 * fails to send or the module does not respond, a debug message is printed.
 * 
 * @param[in] self Pointer to the XBee instance.
 * @param[in] value The AppEUI to be set, provided as a string.
 * 
 * @return bool Returns true if the AppEUI was successfully set, otherwise false.
 */
bool XBeeLR_SetAppEUI(XBee* self, const char* value) {
    uint8_t response[17];
    uint8_t response_length;
    int status = api_send_at_command_and_get_response(self, AT_AE, value, response, &response_length, 5000);
    if(status != API_SEND_SUCCESS){
        XBEE_DEBUG_PRINT_ENABLED("Failed to set App EUI\n");
    }
    return status;
}

/**
 * @brief Sends the AT_AK command to set the LoRaWAN AppKey on the XBee LR module.
 * 
 * This function configures the LoRaWAN AppKey (Application Key) on the XBee LR module 
 * by sending the AT command `AT_AK` with the specified AppKey value. The function is 
 * blocking, meaning it waits for a response from the module or until a timeout occurs. 
 * If the command fails to send or the module does not respond, a debug message is printed.
 * 
 * @param[in] self Pointer to the XBee instance.
 * @param[in] value The AppKey to be set, provided as a string.
 * 
 * @return bool Returns true if the AppKey was successfully set, otherwise false.
 */
bool XBeeLR_SetAppKey(XBee* self, const char* value) {
    uint8_t response[33];
    uint8_t response_length;
    int status = api_send_at_command_and_get_response(self, AT_AK, value, response, &response_length, 5000);
    if(status != API_SEND_SUCCESS){
        XBEE_DEBUG_PRINT_ENABLED("Failed to set App Key\n");
    }
    return status;
}

/**
 * @brief Sends the AT_NK command to set the LoRaWAN NwkKey on the XBee LR module.
 * 
 * This function configures the LoRaWAN NwkKey (Network Key) on the XBee LR module 
 * by sending the AT command `AT_NK` with the specified NwkKey value. The function 
 * is blocking, meaning it waits for a response from the module or until a timeout 
 * occurs. If the command fails to send or the module does not respond, a debug 
 * message is printed.
 * 
 * @param[in] self Pointer to the XBee instance.
 * @param[in] value The NwkKey to be set, provided as a string.
 * 
 * @return bool Returns true if the NwkKey was successfully set, otherwise false.
 */
bool XBeeLR_SetNwkKey(XBee* self, const char* value) {
    uint8_t response[33];
    uint8_t response_length;
    int status = api_send_at_command_and_get_response(self, AT_NK, value, response, &response_length, 5000);
    if(status != API_SEND_SUCCESS){
        XBEE_DEBUG_PRINT_ENABLED("Failed to set Nwk Key\n");
    }
    return status;
}

/**
 * @brief Sends the AT_DE command to read the LoRaWAN DevEUI from the XBee LR module.
 * 
 * This function retrieves the LoRaWAN DevEUI (Device Extended Unique Identifier) 
 * from the XBee LR module by sending the AT command `AT_DE`. The function is 
 * blocking, meaning it waits for a response from the module or until a timeout occurs. 
 * If the command fails to send or the module does not respond, a debug message is printed. 
 * The DevEUI is stored in the provided response buffer.
 * 
 * @param[in] self Pointer to the XBee instance.
 * @param[out] response_buffer Buffer to store the retrieved DevEUI.
 * @param[in] buffer_size Size of the response buffer (should be at least 17 bytes).
 * 
 * @return bool Returns true if the DevEUI was successfully retrieved, otherwise false.
 */
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
        XBEE_DEBUG_PRINT_ENABLED("Failed to receive AT_DE response, error code: %d\n", status);
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

/**
 * @brief Parses an RX_PACKET frame and invokes the receive callback function.
 * 
 * This function processes a received RX_PACKET frame specific to the XBee LR module. 
 * It parses the frame data, extracting relevant information such as the port number, 
 * RSSI, SNR, and payload. If the frame is of type `XBEE_API_TYPE_LR_RX_PACKET` or 
 * `XBEE_API_TYPE_LR_EXPLICIT_RX_PACKET`, the parsed data is stored in an `XBeeLRPacket_t` 
 * structure, and the receive callback function (`OnReceiveCallback`) is called with this 
 * data. The function ensures that the callback is only invoked if the receive data is valid.
 * 
 * @param[in] self Pointer to the XBee instance.
 * @param[in] param Pointer to the received API frame data.
 * 
 * @return void This function does not return a value.
 */
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

    // Store the delivery status in the XBee instance
    self->delivery_status = frame->data[2];  // Extract Delivery Status

    // Set the tx_status_received flag to indicate the status frame was received
    self->tx_status_received = true;

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

/**
 * @brief Constructor for creating an XBeeLR instance.
 * 
 * This function allocates memory for a new XBeeLR instance and initializes it 
 * with the provided callback table (`cTable`) and handler table (`hTable`). 
 * The function sets up the virtual table (`vtable`) for XBee LR-specific operations 
 * and assigns the callback and handler tables to the instance. The newly created 
 * instance is then returned to the caller.
 * 
 * @param[in] cTable Pointer to the callback table containing function pointers for handling XBee events.
 * @param[in] hTable Pointer to the handler table containing platform-specific function implementations.
 * 
 * @return XBeeLR* Pointer to the newly created XBeeLR instance.
 */
XBeeLR* XBeeLR_Create(const XBeeCTable* cTable, const XBeeHTable* hTable) {
    XBeeLR* instance = (XBeeLR*)malloc(sizeof(XBeeLR));
    instance->base.vtable = &XBeeLR_VTable;
    instance->base.htable = hTable;
    instance->base.ctable = cTable;
    return instance;
}

void XBeeLR_Destroy(XBeeLR* self) {
    free(self);
}
