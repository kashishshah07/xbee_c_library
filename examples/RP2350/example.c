#include "xbee_api_frames.h"
#include "xbee_at_cmds.h"
#include "uart.h"
#include <bcm2835.h>
#include <stdio.h>
#include <stdint.h>

int main(void) {
    // Initialize UART with a baud rate of 9600
    uart_init(9600);

    // Timing variables
    uint32_t previousMillis = get_current_time_ms();
    const uint32_t interval = 5000;  // Interval at which to send the command (milliseconds)

    while (1) {
        // Create a frame to receive data
        xbee_api_frame_t frame;
        int status = api_receive_api_frame(&frame);

        // Handle the received frame based on its type
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

        // Check if 5 seconds have passed
        uint32_t currentMillis = get_current_time_ms();
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;

            // Send the BD AT Command through API Frame with an empty parameter
            api_send_at_command(AT_DE, NULL, 0);
        }
    }

    // Cleanup before exiting
    bcm2835_close();
    return 0;
}
