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

int main() {
    port_uart_init("/dev/cu.usbserial-1120", B9600);
    time_t start_time, current_time;
    
   // Create an instance of the XBeeLR class
    XBeeLR * my_xbee_lr = XBeeLR_Create();

    //Example configuration (can be NULL if not needed)
    const void* config = NULL;

    uint8_t dev_eui[17];
    XBeeLR_GetDevEUI(my_xbee_lr, dev_eui, sizeof(dev_eui));
    port_debug_printf("DEVEUI: %s\n", dev_eui);

    // XBeeLR payload to send
    uint8_t example_payload[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    uint16_t payload_len = sizeof(example_payload) / sizeof(example_payload[0]);

    port_debug_printf("Connecting...\n");
    XBee_Connect((XBee*)my_xbee_lr);
    time(&start_time);

   while (1) {
        //Let XBee process
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
                XBee_SendData((XBee*)my_xbee_lr, example_payload, payload_len);
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