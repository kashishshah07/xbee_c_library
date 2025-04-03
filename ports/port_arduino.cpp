/**
 * @file port_arduino.cpp
 * @brief Platform-specific implementation of hardware abstraction functions for Arduino.
 * 
 * This file provides the necessary functions to interface with the hardware on the Arduino platform,
 * including UART initialization, data transmission, data reception, and timekeeping.
 * 
 * @version 1.0
 * @date 2024-08-17
 * author Felix Galindo
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

#include <Arduino.h>
#include <stdarg.h>
#include "port.h"

// Global or static variable to hold the serial instance (HardwareSerial or SoftwareSerial)
static Stream* serialPort = NULL;

/**
 * @brief Initializes the UART for communication on the Arduino platform.
 * 
 * This function sets up the UART peripheral with the specified baud rate.
 * 
 * @param baudrate The baud rate for UART communication.
 * @param device Pointer to the `HardwareSerial` or `SoftwareSerial` instance representing the UART to use.
 * 
 * @return int Returns 0 on success, or -1 if the device is not specified.
 */
int portUartInit(uint32_t baudrate, void *device) {
    if (device == NULL) {
        return -1; // Error: No device specified
    }

    serialPort = static_cast<Stream*>(device); // Store the device globally as a Stream*

    // Since RTTI is not available, we will assume the correct type is passed
    // if (device == &Serial1 || device == &Serial || device == &Serial2 || device == &Serial3) {
        ((HardwareSerial*)serialPort)->begin(baudrate);
    // }

    // KDB - The arduino core doesn't set our pins to the correct function for the UART 
    // see the helpful table here: https://github.com/raspberrypi/pico-sdk/blob/master/src/rp2_common/hardware_gpio/include/hardware/gpio.h
#if defined(PICO_RP2350)
    if (PIN_SERIAL1_TX == 18 && PIN_SERIAL1_RX == 19 && device == &Serial1) {
        gpio_set_function(PIN_SERIAL1_TX, (gpio_function_t)11);
        gpio_set_function(PIN_SERIAL1_RX, (gpio_function_t)11);
        uart_init(uart0, baudrate);
    }
#endif
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
 * @return int Returns the number of bytes successfully written, or -1 if the serial port is not initialized.
 */
int portUartWrite(const uint8_t *data, uint16_t length) {
    if (serialPort == NULL) {
        return -1; // Error: Serial port not initialized
    }
    return serialPort->write(data, length);
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
 * @return int Returns the number of bytes actually read, or -1 if the serial port is not initialized.
 */
int portUartRead(uint8_t *buffer, int length) {
    if (serialPort == NULL) {
        return -1; // Error: Serial port not initialized
    }

    int bytesRead = 0;


    if (serialPort->available()) {
        int c = serialPort->read();
        if (c == -1) {
            return -1; // Return error if Serial.read() fails
        }
        buffer[bytesRead++] = (uint8_t)c;
    }


    return bytesRead;
}

/**
 * @brief Flushes the UART receive buffer.
 * 
 * This function clears any data that may be present in the UART's receive buffer.
 */
void portFlushRx() {
    if (serialPort == NULL) {
        return; // Error: Serial port not initialized
    }

    while (serialPort->available()) {
        serialPort->read();
    }
}

/**
 * @brief Returns the number of milliseconds since the program started.
 * 
 * This function uses Arduino's `millis()` function to return the time elapsed since the device was powered on.
 * 
 * @return uint32_t The number of milliseconds since startup.
 */
uint32_t portMillis() {
    return millis();
}

/**
 * @brief Delays execution for a specified number of milliseconds.
 * 
 * This function pauses execution for the specified duration using Arduino's `delay()` function.
 * 
 * @param ms The number of milliseconds to delay.
 */
void portDelay(uint32_t ms) {
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
void portDebugPrintf(const char *format, ...) {
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Serial.print(buffer);
    Serial.println();
}