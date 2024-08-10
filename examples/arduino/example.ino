#include "xbee_api_frames.h"
#include "xbee_at_cmds.h"
#include "uart_arduino.h"

unsigned long previousMillis = 0;  // Stores the last time the command was sent
const long interval = 5000;        // Interval at which to send the command (milliseconds)

void setup() {
    port_uart_init(9600);  // Initialize UART for Arduino
}

void loop() {
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
                Serial.println("Received unknown frame type.");
                break;
        }
    }

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        api_send_at_command(AT_DE, NULL, 0);  // Send the DE AT Command through API Frame
    }
}
