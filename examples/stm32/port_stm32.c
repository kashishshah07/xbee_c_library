/**
 * @file port_stm32.c
 * @brief Platform-specific implementation of hardware abstraction functions for STM32.
 * 
 * This file provides the necessary functions to interface with the hardware on the STM32 platform,
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
#include "stm32f4xx_hal.h"  // Include STM32 HAL header for the target device

UART_HandleTypeDef huart1;      // Define the UART handle for USART1
UART_HandleTypeDef huart_debug; // Define the UART handle for the debug UART (e.g., USART2)

/**
 * @brief Initializes the UART for communication on the STM32 platform.
 * 
 * This function sets up the UART peripheral with the specified baud rate.
 * 
 * @param baudrate The baud rate for UART communication.
 * @param device Unused parameter on the STM32 platform; pass NULL.
 * 
 * @return int Returns 0 on success, -1 on failure.
 */
int port_uart_init(uint32_t baudrate, const char *device) {
    huart1.Instance = USART1;  // Use USART1
    huart1.Init.BaudRate = baudrate;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        return -1;  // Initialization failed
    }
    return 0;  // Initialization successful
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
    return HAL_UART_Transmit(&huart1, (uint8_t *)data, length, HAL_MAX_DELAY);
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
    return HAL_UART_Receive(&huart1, buffer, length, HAL_MAX_DELAY);
}

/**
 * @brief Flushes the UART receive buffer.
 * 
 * This function clears any data that may be present in the UART's receive buffer.
 */
void port_flush_rx() {
    __HAL_UART_FLUSH_DRREGISTER(&huart1);
}

/**
 * @brief Returns the number of milliseconds since the program started.
 * 
 * This function uses the HAL library to return the time elapsed since the device was powered on.
 * 
 * @return uint32_t The number of milliseconds since startup.
 */
uint32_t port_millis() {
    return HAL_GetTick();
}

/**
 * @brief Delays execution for a specified number of milliseconds.
 * 
 * This function pauses execution for the specified duration using the HAL library.
 * 
 * @param ms The number of milliseconds to delay.
 */
void port_delay(uint32_t ms) {
    HAL_Delay(ms);
}

/**
 * @brief Prints debug information to the debug UART output.
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
    HAL_UART_Transmit(&huart_debug, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
}
