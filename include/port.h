/**
 * @file port.h
 * @brief Port Abstraction Header
 * @version 1.0
 * @date 2024-08-08
 * 
 * @license MIT
 * 
 * @author Felix Galindo, Digi International
 * @contact felix.galindo@digi.com
 */

#ifndef PORT_H
#define PORT_H

#include <stdint.h>

// Constants
#define UART_READ_TIMEOUT_MS 1000  // Timeout of 2500 milliseconds (2.5 second)

// Enum for UART read status
typedef enum {
    UART_SUCCESS = 0,
    UART_INIT_FAILED,
    UART_ERROR_TIMEOUT,
    UART_ERROR_OVERRUN,
    UART_ERROR_UNKNOWN
} uart_status_t;

uart_status_t port_uart_read(uint8_t *buf, int len, int *bytes_read);
int port_uart_write(uint8_t *data, uint16_t len);
uint32_t port_millis(void);
void port_flush_rx(void);
int port_uart_init(uint32_t baudrate, const char *device);
void port_delay(uint32_t ms);
void port_debug_printf(const char *format, ...);

#endif // UART_H
