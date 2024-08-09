#include "xbee_api_frames.h"
#include "xbee_at_cmds.h"
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

   while (1) {

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