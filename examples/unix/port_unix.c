/**
 * @file port_unix.c
 * @brief Unix-specific platform abstraction layer for hardware interfaces.
 * 
 * This file implements the Unix-specific functions required by the XBee library 
 * to interface with hardware. It provides implementations for serial communication, 
 * timing, and other low-level operations that are specific to Unix-based systems.
 * This file serves as the platform abstraction layer, ensuring that the XBee library 
 * can operate on Unix systems.
 * 
 * @version 1.0
 * @date 2024-08-08
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
 * 
 * @author Felix Galindo
 * @contact felix.galindo@digi.com
 */

#include "port.h"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdarg.h>

static int uart_fd = -1;

/**
 * @brief Initializes the serial port for communication with the XBee module.
 * 
 * This function configures and opens the serial port on a Unix system for communication 
 * with the XBee module. It sets parameters such as baud rate, parity, and stop bits.
 * 
 * @param[in] device Path to the serial device (e.g., "/dev/ttyUSB0").
 * @param[in] baudrate Baud rate for the serial communication.
 * 
 * @return int Returns 0 on success, or a negative error code on failure.
 */
int port_uart_init(uint32_t baudrate, const char *device) {
    struct termios options;

    // Open the UART device file
    uart_fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart_fd == -1) {
        perror("Unable to open UART");
        return UART_INIT_FAILED;
    }

    // Get the current options for the port
    tcgetattr(uart_fd, &options);

    // Set the baud rate
    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);

    // Set 8N1 (8 data bits, no parity, 1 stop bit)
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // Set non-blocking mode
    fcntl(uart_fd, F_SETFL, FNDELAY);

    // Apply the options
    tcsetattr(uart_fd, TCSANOW, &options);

    return UART_SUCCESS;
}

/**
 * @brief Writes data to the UART, ensuring all bytes are sent.
 * 
 * This function attempts to write the entire buffer to the UART. It will
 * continue writing until all bytes are sent or an error occurs. It does not
 * handle timeouts; this is expected to be managed by the calling function.
 * 
 * @param buf Pointer to the buffer containing the data to be sent.
 * @param len Length of the data in bytes.
 * @return int The number of bytes successfully written, or a negative error code.
 */
int port_uart_write(const uint8_t *buf, uint16_t len) {
    uint16_t total_bytes_written = 0;

    while (total_bytes_written < len) {
        int bytes_written = write(uart_fd, buf + total_bytes_written, len - total_bytes_written);
        if (bytes_written < 0) {
            perror("Error writing to UART");
            return UART_ERROR_UNKNOWN;  // Return error code if write fails
        }

        total_bytes_written += bytes_written;  // Accumulate the total bytes written
    }

    return total_bytes_written;  // Return the total number of bytes successfully written
}


/**
 * @brief Reads up to `length` bytes from the UART.
 * 
 * This function reads up to `length` bytes from the UART and stores them in the provided `buffer`.
 * It returns the number of bytes actually read. 
 * 
 * @param buffer Pointer to the buffer where the data will be stored.
 * @param length Maximum number of bytes to read.
 * @return int Number of bytes actually read, or -1 if an error occurs.
 */
int port_uart_read(uint8_t *buffer, int length) {
    int bytes_read = read(uart_fd, buffer, length);
    
    if (bytes_read < 0) {
        //perror("UART read error");
        return -1;
    }

    return bytes_read; // Return the actual number of bytes read
}
/**
 * @brief Gets the current system time in milliseconds.
 * 
 * This function retrieves the current system time, expressed in milliseconds 
 * since the Unix epoch. It can be used for timing and measuring elapsed time.
 * 
 * @return uint32_t The current system time in milliseconds.
 */
uint32_t port_millis(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

/**
 * @brief Flushes the receive buffer of the serial port.
 * 
 * This function clears the receive buffer of the serial port, discarding any 
 * unread data. It is typically used to ensure that no old or unwanted data 
 * remains in the buffer before starting a new communication session.
 * 
 * @param[in] fd File descriptor for the serial port.
 * 
 * @return void
 */
void port_flush_rx(void) {
    tcflush(uart_fd, TCIFLUSH);
}

/**
 * @brief Delays execution for a specified amount of time.
 * 
 * This function causes the program to pause execution for a given number of milliseconds.
 * 
 * @param[in] milliseconds The number of milliseconds to delay.
 * 
 * @return void This function does not return a value.
 */
void port_delay(uint32_t ms) {
    usleep(ms * 1000);  // POSIX or bare metal delay (microseconds)
}

/**
 * @brief Outputs a formatted debug message to the console or log.
 * 
 * This function prints a formatted debug message to the console or a log file. 
 * It acts as a wrapper around the standard printf function, potentially adding 
 * additional debug information such as timestamps or log levels.
 * 
 * @param[in] format The format string (similar to printf) that specifies how 
 * the subsequent arguments are converted for output.
 * @param[in] ... Variable arguments that are formatted and printed according 
 * to the format string.
 * 
 * @return void This function does not return a value.
 */
void port_debug_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);  // Standard POSIX vprintf function
    va_end(args);
}