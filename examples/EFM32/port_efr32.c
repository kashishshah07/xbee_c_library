/**
 * @file port_efm32.c
 * @brief XBee Library Port for EFM32
 * @version 1.0
 * @date 2024-08-08
 * 
 * @license MIT
 * 
 * This file provides a basic XBee Library port using the EFM32 HAL.
 */

#include "port.h"
#include "em_usart.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_timer.h"

#define UART_PORT gpioPortA  // Define the port as needed
#define UART_TX_PIN 0        // TX pin number
#define UART_RX_PIN 1        // RX pin number

void port_uart_init(uint32_t baudrate) {
    // Enable clocks for GPIO and USART0
    CMU_ClockEnable(cmuClock_GPIO, true);
    CMU_ClockEnable(cmuClock_USART0, true);

    // Configure GPIO pins for UART
    GPIO_PinModeSet(UART_PORT, UART_TX_PIN, gpioModePushPull, 1);  // TX
    GPIO_PinModeSet(UART_PORT, UART_RX_PIN, gpioModeInput, 0);     // RX

    // Initialize USART0 for asynchronous operation
    USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;
    init.baudrate = baudrate;

    USART_InitAsync(USART0, &init);

    // Route USART0 TX and RX to the specified pins
    USART0->ROUTELOC0 = (USART_ROUTELOC0_TXLOC_LOC0 << _USART_ROUTELOC0_TXLOC_SHIFT) |
                        (USART_ROUTELOC0_RXLOC_LOC0 << _USART_ROUTELOC0_RXLOC_SHIFT);
    USART0->ROUTEPEN = USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN;

    // Enable USART0
    USART_Enable(USART0, usartEnable);
}

int port_uart_write(uint8_t *data, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) {
        USART_Tx(USART0, data[i]);
    }

    // Assuming there's a way to check if the transmission was successful
    // For simplicity, we'll assume success here, but error checks can be added
    return 0;  // Success
}
uart_status_t port_uart_read(uint8_t *buf, int len, int *bytes_read) {
    *bytes_read = 0;
    uint32_t start_time = TIMER_CounterGet(TIMER0);

    for (int i = 0; i < len; i++) {
        while (!(USART_StatusGet(USART0) & USART_STATUS_RXDATAV)) {
            // Check if the timeout has occurred
            if ((TIMER_CounterGet(TIMER0) - start_time) > UART_READ_TIMEOUT_MS) {
                return UART_ERROR_TIMEOUT;
            }
        }
        buf[i] = USART_Rx(USART0);
        (*bytes_read)++;
    }

    return UART_SUCCESS;
}

uint32_t port_millis(void) {
    return TIMER_CounterGet(TIMER0);  // Use a TIMER or similar to get time in milliseconds
}

void port_flush_rx(void) {
    while (USART_StatusGet(USART0) & USART_STATUS_RXDATAV) {
        volatile uint8_t dummy = USART_Rx(USART0);
        (void)dummy;  // Discard the data
    }
}
