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
        switch (frame.type) {
            case XBEE_API_TYPE_AT_RESPONSE:
                xbee_handle_at_response(&frame);
                break;
            case XBEE_API_TYPE_MODEM_STATUS:
                xbee_handle_modem_status(&frame);
                break;
            case XBEE_API_TYPE_RX_PACKET:
                xbee_handle_rx_packet(&frame);
                break;
            default:
                printf("Received unknown frame type: %d\n", frame.type);
                break;
        }
    } else if (status == -1) {
        printf("Error receiving frame.\n");
    }
}


void XBeeLR_ConfigureNetwork(XBee* self, const void* config) {
    const XBeeLRNetworkConfig* netConfig = (const XBeeLRNetworkConfig*)config;
    api_send_at_command(AT_AE,"37D56A3F6CDCF0A5", 16);
    api_send_at_command(AT_AK,"BD32AAB41C54175E9060D86F3A8B7F42", 32);
    api_send_at_command(AT_NK,"BD32AAB41C54175E9060D86F3A8B7F42", 32);
    api_send_at_command(AT_WR, NULL, 0);
    api_send_at_command(AT_AC, NULL, 0);
    // Implement XBeeLR specific network configuration logic
}

void XBeeLR_ConfigureSerial(XBee* self, const void* config) {
    const XBeeLRSerialConfig* serialConfig = (const XBeeLRSerialConfig*)config;
    // Implement XBeeLR specific serial configuration logic
}

bool XBeeLR_Connected(XBee* self) {
    // Implement logic to check XBeeLR network connection
    return true;  // Placeholder
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
    uint8_t frame_data[1]; // Adjust size as needed based on the frame structure

    // Example frame data for Join Request (you need to adjust based on actual protocol)
    frame_data[0] = 0x01;  // Frame ID
    // Add other relevant data to frame_data based on protocol specification
    // ...

    // Call the api_send_frame function to send the Join Request API frame
    api_send_frame(0x14, frame_data, sizeof(frame_data));
}

static void SendTxReqApiFrame(const uint8_t* payload, uint16_t payload_len, uint8_t port, uint8_t options) {
    uint8_t frame_data[128]; // Adjust size as needed based on the frame structure

    // Example frame data for Tx Request
    frame_data[0] = 0x50;  // API Frame Tx Request ID
    frame_data[1] = 0x01;  // Frame ID
    frame_data[3] = port;  // LoRaWAN Port
    frame_data[4] = options;  // Transmit Options
    // Add the payload to the frame data
    memcpy(&frame_data[5], payload, payload_len);

    // Call the api_send_frame function to send the Tx Request API frame
    api_send_frame(0x50, frame_data, 4 + payload_len);
}