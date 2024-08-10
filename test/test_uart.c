#include "unity.h"
#include "port.h"

void setUp(void) {}

void tearDown(void) {}

void test_uart_init(void) {
    port_uart_init(("/dev/ttyUSB0", B9600);
    // Check UART initialization behavior (e.g., file descriptor value)
}

void test_uart_write(void) {
    uint8_t data[] = { 0x01, 0x02, 0x03 };
    int result = port_uart_write(data, sizeof(data));

    TEST_ASSERT_EQUAL(3, result); // Expecting 3 bytes written
}

void test_uart_read(void) {
    uint8_t buffer[10];
    int bytes_read = port_uart_read(buffer, sizeof(buffer), 1000);

    TEST_ASSERT(bytes_read > 0); // Assuming some data was read
}

void test_uart_clear_receive_buffer(void) {
    port_flush_rx(();
    // Verify that the buffer was cleared
}
