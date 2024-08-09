#include "xbee_api_frames.h"
#include "xbee_at_cmds.h"
#include "xbee_lr.h"
#include "uart.h"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int main() {
    uart_init("/dev/cu.usbserial-1120", B9600);
    time_t start_time, current_time;
    time(&start_time);

   // Create an instance of the XBeeLR class
    XBeeLR * my_xbee_lr = XBeeLR_Create();

    //Example configuration (can be NULL if not needed)
    const void* config = NULL;

    //Use the XBee_Connect method, which will call the custom connect function for XBeeLR
    XBee_Connect((XBee*)my_xbee_lr, config);


   while (1) {
        XBee_Process((XBee*)my_xbee_lr);

        // Get the current time
        time(&current_time);

        // Check if 5 seconds have passed
        if (difftime(current_time, start_time) >= 5) {
            // Reset the start time
            time(&start_time);

            // Send the BD AT Command through API Frame with an empty parameter
            api_send_at_command(AT_DE, NULL, 0);
        }

    }

    return 0;
}