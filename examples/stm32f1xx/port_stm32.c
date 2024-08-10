/**
 * @file port_stm32.c
 * @brief XBee Library Port for STM32
 * @version 1.0
 * @date 2024-08-08
 * 
 * @license MIT
 * 
 * @author Felix Galindo, Digi International
 * @contact felix.galindo@digi.com
 */

#include "port.h"
#include "stm32f1xx_hal.h"

extern UART_HandleTypeDef huart1;

void port_uart_init(uint32_t baudrate) {
    huart1.Init.BaudRate = baudrate;
    HAL_UART_Init(&huart1);
}

int port_uart_write(uint8_t *data, uint16_t len) {
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart1, data, len, HAL_MAX_DELAY);

    if (status == HAL_OK) {
        return 0;  // Success
    } else {
        return -1;  // UART failure
    }
}
uart_status_t port_uart_read(uint8_t *buf, int len, int *bytes_read) {
    HAL_StatusTypeDef status = HAL_UART_Receive(&huart1, buf, len, UART_READ_TIMEOUT_MS);

    if (status == HAL_OK) {
        *bytes_read = len;
        return UART_SUCCESS;
    } else if (status == HAL_TIMEOUT) {
        *bytes_read = 0;
        return UART_ERROR_TIMEOUT;
    } else if (status == HAL_ERROR) {
        *bytes_read = 0;
        return UART_ERROR_OVERFLOW; // Consider as overrun or other error
    }

    *bytes_read = 0;
    return UART_ERROR_UNKNOWN;
}

// Function to get the current time in milliseconds
uint32_t port_millis(void) {
    return HAL_GetTick();
}

void port_flush_rx(void) {
    uint8_t dummy;
    while (HAL_UART_Receive(&huart1, &dummy, 1, 10) == HAL_OK) {
        // Continue reading and discarding data
    }
}