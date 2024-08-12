/**
 * @file xbee.c
 * @brief Implementation of the XBee class.
 * 
 * This file contains the implementation of the core functions used to interact
 * with XBee modules using the API frame format. The functions provide an interface
 * for initializing the module, connecting/disconnect to the nework, 
 * sending and receiving data, and handling AT commands.
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
**/

#include "xbee.h"
#include "xbee_api_frames.h" 

// Base class methods

/**
 * @brief Initializes the XBee module.
 * 
 * This function initializes the XBee module by setting the initial frame ID counter 
 * and calling the XBee subclass specific initialization routine.
 * 
 * @param[in] self Pointer to the XBee instance.
 * @param[in] baudrate Baud rate for the serial communication.
 * @param[in] device Path to the serial device (e.g., "/dev/ttyUSB0").
 * 
 * @return True if initialization is successful, otherwise false.
 */
bool XBee_Init(XBee* self, uint32_t baudrate, const char* device) {
    self->frameIdCntr = 1;
    return self->vtable->init(self, baudrate, device);
}

/**
 * @brief Connects the XBeeto the network.
 * 
 * This function connects to the network by calling the XBee subclass specific 
 * connection implementation provided by the XBee subclass. This is a blocking function.
 * 
 * @param[in] self Pointer to the XBee instance.
 * 
 * @return True if the connection is successful, otherwise false.
 */
bool XBee_Connect(XBee* self) {
    return self->vtable->connect(self);
}

/**
 * @brief Disconnects the XBee from the network.
 * 
 * This function closes the connection by calling the platform-specific 
 * close implementation provided by the XBee subclass. This is a blocking function.
 * 
 * @param[in] self Pointer to the XBee instance.
 * 
 * @return True if the disconnection is successful, otherwise false.
 */
bool XBee_Disconnect(XBee* self) {
    return self->vtable->disconnect(self);
}

/**
 * @brief Request XBee to send data over network.
 * 
 * This function sends data over the network by calling the XBee subclass specific 
 * send data implementation provided by the XBee subclass. This is a blocking function.
 * 
 * @param[in] self Pointer to the XBee instance.
 * 
 * @return True if the data was sent successfully, otherwise false.
 */
bool XBee_SendData(XBee* self, const void* data) {
    return self->vtable->send_data(self, data);
}

bool XBee_SoftReset(XBee* self) {
    return self->vtable->soft_reset(self);
}

void XBee_HardReset(XBee* self) {
    self->vtable->hard_reset(self);
}

//Calls XBee subclass Process implementation
//Must be called continously
void XBee_Process(XBee* self) {
    self->vtable->process(self);
}

bool XBee_Connected(XBee* self) {
    return self->vtable->connected(self);
}

//Sends ATWR command
//0 if successful
bool XBee_WriteConfig(XBee* self) {
    uint8_t response[33];
    uint8_t response_length;
    int status = api_send_at_command_and_get_response(self, AT_WR, NULL, response, &response_length, 5000);
    if(status != API_SEND_SUCCESS){
        port_debug_printf("Failed to Write Config\n");
    }
    return status;
}

//Sends ATAC command
//0 if successful
bool XBee_ApplyChanges(XBee* self) {
    uint8_t response[33];
    uint8_t response_length;
    int status = api_send_at_command_and_get_response(self, AT_AC, NULL, response, &response_length, 5000);
    if(status != API_SEND_SUCCESS){
        port_debug_printf("Failed to Apply Changes\n");
    }
    return status;
}


