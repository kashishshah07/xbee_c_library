/**
 * @file port_windows.c
 * @brief Platform-specific implementation of hardware abstraction functions for Windows.
 * 
 * This file provides the necessary functions to interface with the hardware on the Windows platform,
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
#include <windows.h>
#include <stdio.h>

HANDLE hSerial;  // Handle for the serial port

/**
 * @brief Initializes the UART for communication on the Windows platform.
 * 
 * This function sets up the UART (COM port) with the specified baud rate.
 * 
 * @param baudrate The baud rate for UART communication.
 * @param device The name of the COM port (e.g., "COM3").
 * 
 * @return int Returns 0 on success, -1 on failure.
 */
int portUartInit(uint32_t baudrate, const char *device) {
    hSerial = CreateFile(device, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("Error opening serial port %s\n", device);
        return -1;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        printf("Error getting device state\n");
        return -1;
    }
    dcbSerialParams.BaudRate = baudrate;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(hSerial, &dcbSerialParams)) {
        printf("Error setting device parameters\n");
        return -1;
    }

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (!SetCommTimeouts(hSerial, &timeouts)) {
        printf("Error setting timeouts\n");
        return -1;
    }

    return 0;
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
int portUartWrite(const uint8_t *data, int length) {
    DWORD bytes_written;
    if (!WriteFile(hSerial, data, length, &bytes_written, NULL)) {
        printf("Error writing data to serial port\n");
        return -1;
    }
    return bytes_written;
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
int portUartRead(uint8_t *buffer, int length) {
    DWORD bytesRead;
    if (!ReadFile(hSerial, buffer, length, &bytesRead, NULL)) {
        printf("Error reading data from serial port\n");
        return -1;
    }
    return bytesRead;
}

/**
 * @brief Flushes the UART receive buffer.
 * 
 * This function clears any data that may be present in the UART's receive buffer.
 */
void portFlushRx() {
    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_RXABORT);
}

/**
 * @brief Returns the number of milliseconds since the program started.
 * 
 * This function uses Windows API to return the time elapsed since the device was powered on.
 * 
 * @return uint32_t The number of milliseconds since startup.
 */
uint32_t portMillis() {
    return GetTickCount();
}

/**
 * @brief Delays execution for a specified number of milliseconds.
 * 
 * This function pauses execution for the specified duration using the Windows Sleep function.
 * 
 * @param ms The number of milliseconds to delay.
 */
void portDelay(uint32_t ms) {
    Sleep(ms);
}

/**
 * @brief Prints debug information to the console.
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
    printf("%s", buffer);
}
