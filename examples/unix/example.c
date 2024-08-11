#include "xbee_api_frames.h"
#include "xbee_at_cmds.h"
#include "xbee_lr.h"
#include "port.h"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>


void OnReceiveCallback(XBee* self, void* data){
    XBeeLRPacket_t* packet = (XBeeLRPacket_t*) data;
    port_debug_printf("Received Packet: ");
    for (int i = 1; i < packet->payloadSize; i++) {
        port_debug_printf("%02X ", packet->payload[i]);
    }
    port_debug_printf("\n");
    port_debug_printf("Ack %u\n", packet->ack);
    port_debug_printf("Port %u\n", packet->port);
    port_debug_printf("RSSI %d\n", packet->rssi);
    port_debug_printf("SNR %d\n", packet->snr);
    port_debug_printf("Downlink Counter %lu\n", packet->counter);
}

void OnSendCallback(XBee* self, void* data){
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
    // port_uart_init("/dev/cu.usbserial-1120", B9600);
    time_t start_time, current_time;
    
    // Harware Abstraction Function Pointer Table for XBeeLR
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
        .OnReceiveCallback = OnReceiveCallback,
        .OnSendCallback = OnSendCallback,
    };

   // Create an instance of the XBeeLR class
    XBeeLR * my_xbee_lr = XBeeLR_Create(&XBeeLR_CTable,&XBeeLR_HTable);
    XBee_Init((XBee*)my_xbee_lr,B9600, "/dev/cu.usbserial-1120");
    port_delay(1);

    uint8_t dev_eui[17];
    XBeeLR_GetDevEUI((XBee*)my_xbee_lr, dev_eui, sizeof(dev_eui));
    port_debug_printf("DEVEUI: %s\n", dev_eui);

    port_debug_printf("Configuring...\n");
    XBeeLR_SetAppEUI((XBee*)my_xbee_lr, "37D56A3F6CDCF0A5");
    XBeeLR_SetAppKey((XBee*)my_xbee_lr, "CD32AAB41C54175E9060D86F3A8B7F48");
    XBeeLR_SetNwkKey((XBee*)my_xbee_lr, "CD32AAB41C54175E9060D86F3A8B7F48");
    XBee_WriteConfig((XBee*)my_xbee_lr);
    XBee_ApplyChanges((XBee*)my_xbee_lr);

    // XBeeLR payload to send
    uint8_t example_payload[] = {0xC0, 0xC0, 0xC0, 0xFF, 0xEE};
    uint16_t payload_len = sizeof(example_payload) / sizeof(example_payload[0]);
    XBeeLRPacket_t packet = {
        .payload = example_payload,
        .payloadSize = payload_len,
        .port = 2,
        .ack = 0,
    };

    port_debug_printf("Connecting...\n");
    XBee_Connect((XBee*)my_xbee_lr);
    time(&start_time);

   while (1) {
        //Let XBee class process any serial data
        XBee_Process((XBee*)my_xbee_lr);

        // Get the current time
        time(&current_time);

        // Check if 10 seconds have passed
        if (difftime(current_time, start_time) >= 10) {
            //Send data if connected, else connect
            if(XBee_Connected((XBee*)my_xbee_lr)){
                port_debug_printf("Sending 0x");
                for (int i = 0; i < payload_len; i++) {
                    port_debug_printf("%02X", example_payload[i]);
                }
                port_debug_printf("\n");
                XBee_SendData((XBee*)my_xbee_lr, &packet);
                packet.payload[0] = packet.payload[0] + 1; //change payload
                packet.port++; //change port
                port_debug_printf("Send Frame Id 0x%02X \n", packet.frameId); //XBee_SendData will update packet.frameId to allow tracking 
            }
            else{
                port_debug_printf("Not connected. Connecting...\n");
                XBee_Connect((XBee*)my_xbee_lr);
            }
            // Reset the start time
            time(&start_time);
        }
    }
    return 0;
}