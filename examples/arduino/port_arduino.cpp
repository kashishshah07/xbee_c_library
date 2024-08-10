/**
 * @file port_arduino.cpp
 * @brief XBee Library Port for Arduino
 * @version 1.0
 * @date 2024-08-08
 * 
 * @license MIT
 * 
 * @author Felix Galindo, Digi International
 * @contact felix.galindo@digi.com
 */

#include "port.h"
#include <Arduino.h>

void port_uart_init(uint32_t baudrate) {
    Serial.begin(baudrate);
}

int port_uart_write(uint8_t *data, uint16_t len) {
    size_t written = Serial.write(data, len);

    if (written == len) {
        return 0;  // Success
    } else {
        return -1;  // UART failure
    }
}

uart_status_t port_uart_read(uint8_t *buf, int len, int *bytes_read) {
    *bytes_read = 0;
    unsigned long start_time = millis();

    while (len > 0 && xbeeSerial.available()) {
        buf[*bytes_read] = xbeeSerial.read();
        (*bytes_read)++;
        len--;

        if ((millis() - start_time) > UART_READ_TIMEOUT_MS) {
            return UART_ERROR_TIMEOUT;
        }
    }

    if (*bytes_read == 0) {
        return UART_ERROR_TIMEOUT; // No data received within the timeout
    }

    return UART_SUCCESS;
}

// Function to get the current time in milliseconds
uint32_t port_millis(void) {
    return millis();
}

void port_flush_rx(void) {
    while (xbeeSerial.available()) {
        xbeeSerial.read(); // Discard received byte
    }
}

void port_debug_printf(const char *format, ...) {
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Serial.print(buffer);
}