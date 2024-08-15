/**
 * @file port_rp2350.c
 * @brief Platform-specific implementation of hardware abstraction functions for RP2350 (Raspberry Pi Pico).
 * 
 * This file provides the necessary functions to interface with the hardware on the RP2350 platform,
 * including UART initialization, data transmission, data reception, and timekeeping.
 * 
 * @version 1.0
 * @date 2024-08-13
 * 
 * @license MIT
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "port.h"
#include <stdio.h>

#define COMM_UART uart0        ///< UART used for communication with XBee
#define COMM_TX_PIN 18         ///< TX pin for UART0 (adjust as needed)
#define COMM_RX_PIN 19         ///< RX pin for UART0 (adjust as needed)

/**
 * @brief Initializes the UART for communication on the RP2350 platform.
 * 
 * This function sets up the communication UART (e.g., uart1) with the specified baud rate.
 * 
 * @param baudrate The baud rate for UART communication.
 * @param device Unused parameter on the RP2350 platform; pass NULL.
 * 
 * @return int Returns 0 on success, -1 on failure.
 */
int port_uart_init(uint32_t baudrate, const char *device) {
    gpio_set_function(COMM_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(COMM_RX_PIN, GPIO_FUNC_UART);

    uart_init(COMM_UART, baudrate);

   // Set UART parameters (optional, depending on your needs)
    uart_set_format(uart0, 8, 1, UART_PARITY_NONE); // 8 data bits, 1 stop bit, no parity

    // Enable UART
    uart_set_hw_flow(uart0, false, false); // Disable flow control, if not needed
    uart_set_fifo_enabled(uart0, true);    // Enable FIFO
    return 0;  // Initialization successful
}

/**
 * @brief Writes data to the communication UART.
 * 
 * This function sends the specified number of bytes from the provided buffer over the communication UART.
 * 
 * @param data Pointer to the data to be written.
 * @param len Number of bytes to write.
 * 
 * @return int Returns the number of bytes successfully written.
 */
int port_uart_write(const uint8_t *buf, uint16_t len) {
    int bytes_written = 0;
    for (int i = 0; i < len; i++) {
        if(uart_is_writable(COMM_UART)){
            uart_putc_raw(COMM_UART, buf[i]);
            bytes_written++;
        }else{
            break;
        }
    }
    return bytes_written;  // Return the number of bytes written
}

/**
 * @brief Reads data from the communication UART.
 * 
 * This function reads up to the specified number of bytes from the communication UART and stores them in the provided buffer.
 * It blocks until the requested number of bytes have been read or a timeout/error occurs.
 * 
 * @param buffer Pointer to the buffer where the data will be stored.
 * @param length Maximum number of bytes to read.
 * 
 * @return int Returns the number of bytes actually read.
 */
int port_uart_read(uint8_t *buffer, int length) {
    int bytes_read = 0;
    while (bytes_read < length) {
        if (uart_is_readable(COMM_UART)) {
            buffer[bytes_read++] = uart_getc(COMM_UART);
        } else {
            break;  // No more data to read, exit loop
        }
    }
    return bytes_read;  // Return the number of bytes read
}

/**
 * @brief Flushes the communication UART receive buffer.
 * 
 * This function clears any data that may be present in the UART's receive buffer.
 */
void port_flush_rx() {
    uart_set_hw_flow(COMM_UART, false, false);
}

/**
 * @brief Returns the number of milliseconds since the program started.
 * 
 * This function uses the SDK to return the time elapsed since the device was powered on.
 * 
 * @return uint32_t The number of milliseconds since startup.
 */
uint32_t port_millis() {
    return to_ms_since_boot(get_absolute_time());
}

/**
 * @brief Delays execution for a specified number of milliseconds.
 * 
 * This function pauses execution for the specified duration using the SDK.
 * 
 * @param ms The number of milliseconds to delay.
 */
void port_delay(uint32_t ms) {
    sleep_ms(ms);
}

/**
 * @brief Prints debug information to the debug UART output.
 * 
 * This function provides a formatted print capability for debugging purposes, similar to printf.
 * 
 * @param format The format string (same as printf).
 * @param ... The values to print.
 */
void port_debug_printf(const char *format, ...) {
    char buffer[128];
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}
