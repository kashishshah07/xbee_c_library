
/**
 * @file uart_arduino.cpp
 * @brief UART Abstraction for Arduino
 * @version 1.0
 * @date 2024-08-08
 * 
 * @license MIT
 * 
 * @author Felix Galindo, Digi International
 * @contact felix.galindo@digi.com
 */

#include "uart.h"
#include <Arduino.h>

void uart_init(uint32_t baudrate) {
    Serial.begin(baudrate);
}

void uart_write(uint8_t *data, uint16_t len) {
    Serial.write(data, len);
}

uart_status_t uart_read(uint8_t *buf, int len, int *bytes_read) {
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
uint32_t get_current_time_ms(void) {
    return millis();
}

void uart_clear_receive_buffer(void) {
    while (xbeeSerial.available()) {
        xbeeSerial.read(); // Discard received byte
    }
}