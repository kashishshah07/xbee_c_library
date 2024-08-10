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

    XBeeLR_GetDevEUI(my_xbee_lr);

    // Implement XBeeLR specific data sending logic
    uint8_t example_payload[] = {0x01, 0x02, 0x03, 0x04, 0x05};

    // Length of the payload
    uint16_t payload_len = sizeof(example_payload) / sizeof(example_payload[0]);

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
            // if(XBee_Connected(my_xbee_lr)){
                XBee_SendData((XBee*)my_xbee_lr, example_payload, payload_len);
            // }
            // else{
            //     XBee_Connect((XBee*)my_xbee_lr);
            // }
            // Reset the start time
            time(&start_time);

        }

    }

    return 0;
}