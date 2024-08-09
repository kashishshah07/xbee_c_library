
/**
 * @file uart.h
 * @brief UART Abstraction Header
 * @version 1.0
 * @date 2024-08-08
 * 
 * @license MIT
 * 
 * @author Felix Galindo, Digi International
 * @contact felix.galindo@digi.com
 */

#ifndef UART_H
#define UART_H

#include <stdint.h>

// Constants
#define UART_READ_TIMEOUT_MS 1000  // Timeout of 2500 milliseconds (2.5 second)

// Enum for UART read status
typedef enum {
    UART_SUCCESS = 0,
    UART_ERROR_TIMEOUT,
    UART_ERROR_OVERRUN,
    UART_ERROR_UNKNOWN
} uart_status_t;

uart_status_t uart_read(uint8_t *buf, int len, int *bytes_read);
void uart_write(uint8_t *data, uint16_t len);
uint32_t get_current_time_ms(void);
void uart_clear_receive_buffer(void);
void uart_init(const char *device, uint32_t baudrate);

#endif // UART_H
