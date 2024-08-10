/**
 * @file port_unix.c
 * @brief XBee Library Port for Unix
 * @version 1.0
 * @date 2024-08-08
 * 
 * @license MIT
 * 
 * @author Felix Galindo, Digi International
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

void port_uart_init(const char *device, uint32_t baudrate) {
    struct termios options;

    // Open the UART device file
    uart_fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart_fd == -1) {
        perror("Unable to open UART");
        return;
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
}

int port_uart_write(uint8_t *data, uint16_t len) {
    ssize_t written = write(uart_fd, data, len);

    if (written == len) {
        return 0;  // Success
    } else {
        return -1;  // UART failure
    }
}
uart_status_t port_uart_read(uint8_t *buf, int len, int *bytes_read) {
    *bytes_read = 0;
    int result;
    fd_set readfds;
    struct timeval timeout;
    int total_bytes_read = 0;
    int bytes_left = len;
    time_t start_time, current_time;

    // Get the current time as the start time
    time(&start_time);

    while (bytes_left > 0) {
        // Set the timeout value using UART_READ_TIMEOUT_MS
        timeout.tv_sec = UART_READ_TIMEOUT_MS / 1000;
        timeout.tv_usec = (UART_READ_TIMEOUT_MS % 1000) * 1000;

        FD_ZERO(&readfds);
        FD_SET(uart_fd, &readfds);

        // Wait for data to become available or for the timeout to expire
        result = select(uart_fd + 1, &readfds, NULL, NULL, &timeout);

        if (result > 0) {
            // Check if data is available to read
            int available_bytes;
            if (ioctl(uart_fd, FIONREAD, &available_bytes) < 0) {
                perror("Error checking available bytes");
                return UART_ERROR_UNKNOWN;
            }

            if (available_bytes > 0) {
                // Read the available data, but no more than the remaining requested bytes
                int bytes_to_read = (available_bytes < bytes_left) ? available_bytes : bytes_left;
                int current_bytes_read = read(uart_fd, buf + total_bytes_read, bytes_to_read);
                if (current_bytes_read < 0) {
                    perror("Error reading from UART");
                    return UART_ERROR_UNKNOWN;
                }

                total_bytes_read += current_bytes_read;
                bytes_left -= current_bytes_read;

                // Reset the start time after successfully reading some data
                time(&start_time);
            }
        } else if (result == 0) {
            // Timeout occurred for this select call, check if the overall timeout has elapsed
            time(&current_time);
            if (difftime(current_time, start_time) >= (UART_READ_TIMEOUT_MS / 1000)) {
                return UART_ERROR_TIMEOUT;
            }
        } else {
            // An error occurred
            perror("Error with select()");
            return UART_ERROR_UNKNOWN;
        }
    }

    *bytes_read = total_bytes_read;
    return UART_SUCCESS;
}

uint32_t port_millis(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void port_flush_rx(void) {
    tcflush(uart_fd, TCIFLUSH);
}

void port_delay(uint32_t ms) {
    usleep(ms * 1000);  // POSIX or bare metal delay (microseconds)
}

void port_debug_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);  // Standard POSIX vprintf function
    va_end(args);
}