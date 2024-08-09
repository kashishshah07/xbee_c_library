#include "xbee_api_frames.h"
#include "xbee_at_cmds.h"
#include "uart_stm32.h"
#include "stm32f1xx_hal.h"

int main(void) {
    HAL_Init();
    SystemClock_Config();  // Assuming you have this function set up for your STM32 project

    uart_init(9600);  // Initialize UART for STM32

    uint32_t previousMillis = 0;
    const uint32_t interval = 5000;  // Interval at which to send the command (milliseconds)

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
        }

        uint32_t currentMillis = HAL_GetTick();
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;
            api_send_at_command(AT_DE, NULL, 0);  // Send the DE AT Command through API Frame
        }
    }
}
