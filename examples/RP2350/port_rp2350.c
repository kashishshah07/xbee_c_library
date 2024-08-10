/**
 * @file xbee_port_rp2350.c
 * @brief XBee Library POrt for Raspberry Pi using BCM2835
 * @version 1.0
 * @date 2024-08-08
 * 
 * @license MIT
 * 
 * This file provides a basic XBee library port using the BCM2835 library.
 */

#include "uart.h"
#include <bcm2835.h>
#include <stdio.h>
#include <stdint.h>

#define UART_TX_PIN RPI_GPIO_P1_08 // GPIO14 TXD
#define UART_RX_PIN RPI_GPIO_P1_10 // GPIO15 RXD

void uart_init(uint32_t baudrate) {
    // Initialize the BCM2835 library
    if (!bcm2835_init()) {
        printf("BCM2835 initialization failed!\n");
        return;
    }

    // Set GPIO pins for UART
    bcm2835_gpio_fsel(UART_TX_PIN, BCM2835_GPIO_FSEL_ALT0); // Set TXD pin to alternate function
    bcm2835_gpio_fsel(UART_RX_PIN, BCM2835_GPIO_FSEL_ALT0); // Set RXD pin to alternate function

    // Configure UART settings (example: 8n1, no parity)
    bcm2835_uart_begin();
    bcm2835_uart_set_baudrate(baudrate);
    bcm2835_uart_set_data_bits(BCM2835_UART_DATA_8_BITS);
    bcm2835_uart_set_stop_bits(BCM2835_UART_STOP_1);
    bcm2835_uart_set_parity(BCM2835_UART_PARITY_OFF);
}

int uart_write(uint8_t *data, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) {
        uart_putc_raw(uart0, data[i]);
    }

    // Assuming there's a way to check if the transmission was successful
    // For simplicity, we'll assume success here, but error checks can be added
    return 0;  // Success
}

uart_status_t uart_read(uint8_t *buf, int len, int *bytes_read) {
    *bytes_read = 0;
    uint32_t start_time = bcm2835_st_read();  // Start time in microseconds

    for (int i = 0; i < len; i++) {
        while (!bcm2835_uart_data_available()) {
            // Check if the timeout has occurred
            if ((bcm2835_st_read() - start_time) > (UART_READ_TIMEOUT_MS * 1000)) { // Convert to microseconds
                return UART_ERROR_TIMEOUT;
            }
        }
        buf[i] = bcm2835_uart_receive();
        (*bytes_read)++;
    }

    return UART_SUCCESS;
}

uint32_t get_current_time_ms(void) {
    return bcm2835_st_read() / 1000;  // Convert microseconds to milliseconds
}

void uart_clear_receive_buffer(void) {
    while (bcm2835_uart_data_available()) {
        volatile uint8_t dummy = bcm2835_uart_receive();
        (void)dummy;  // Discard the data
    }
}
