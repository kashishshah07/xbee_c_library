/**
 * @file port_arduino.cpp
 * @brief Platform-specific implementation of hardware abstraction functions for Arduino.
 * 
 * This file provides the necessary functions to interface with the hardware on the Arduino platform,
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

#include "port.h"
#include <Arduino.h>

// Define which serial port to use for XBee communication
#define XBEE_SERIAL_PORT Serial1

/**
 * @brief Initializes the UART for communication on the Arduino platform.
 * 
 * This function sets up the UART peripheral with the specified baud rate.
 * 
 * @param baudrate The baud rate for UART communication.
 * @param device Unused parameter on the Arduino platform; pass NULL.
 * 
 * @return int Returns 0 on success.
 */
int port_uart_init(uint32_t baudrate, const char *device) {
    XBEE_SERIAL_PORT.begin(baudrate);
    while (!XBEE_SERIAL_PORT) {
        ; // Wait for the serial port to connect (for Leonardo or similar boards)
    }
    return 0; // Indicate success
}

/**
 * @brief Writes data to the UART.
 * 
 * This function sends the specified number of bytes from the provided buffer over the UART.
 * 
 * @param data Pointer to the data to be written.
 * @param length Number of bytes to write.
 * 
 * @return int Returns the number of bytes successfully written.
 */
int port_uart_write(const uint8_t *data, int length) {
    return XBEE_SERIAL_PORT.write(data, length);
}

/**
 * @brief Reads data from the UART.
 * 
 * This function reads up to the specified number of bytes from the UART and stores them in the provided buffer.
 * It blocks until the requested number of bytes has been read or an error occurs.
 * 
 * @param buffer Pointer to the buffer where the data will be stored.
 * @param length Maximum number of bytes to read.
 * 
 * @return int Returns the number of bytes actually read.
 */
int port_uart_read(uint8_t *buffer, int length) {
    int bytes_read = 0;

    while (bytes_read < length) {
        if (XBEE_SERIAL_PORT.available()) {
            int c = XBEE_SERIAL_PORT.read();
            if (c == -1) {
                return -1; // Return error if Serial.read() fails
            }
            buffer[bytes_read++] = (uint8_t)c;
        }
    }

    return bytes_read;
}

/**
 * @brief Flushes the UART receive buffer.
 * 
 * This function clears any data that may be present in the UART's receive buffer.
 */
void port_flush_rx() {
    while (XBEE_SERIAL_PORT.available()) {
        XBEE_SERIAL_PORT.read();
    }
}

/**
 * @brief Returns the number of milliseconds since the program started.
 * 
 * This function uses Arduino's `millis()` function to return the time elapsed since the device was powered on.
 * 
 * @return uint32_t The number of milliseconds since startup.
 */
uint32_t port_millis() {
    return millis();
}

/**
 * @brief Delays execution for a specified number of milliseconds.
 * 
 * This function pauses execution for the specified duration using Arduino's `delay()` function.
 * 
 * @param ms The number of milliseconds to delay.
 */
void port_delay(uint32_t ms) {
    delay(ms);
}

/**
 * @brief Prints debug information to the Serial output.
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
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Serial.print(buffer);
}
