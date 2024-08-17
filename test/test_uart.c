#include "unity.h"
#include "port.h"

void setUp(void) {};

void tearDown(void) {};

void testUartInit(void) {
    portUartInit("/dev/ttyUSB0", B9600);
    // Check UART initialization behavior (e.g., file descriptor value)
}

void testUartWrite(void) {
    uint8_t data[] = { 0x01, 0x02, 0x03 };
    int result = portUartWrite(data, sizeof(data));

    TESTAssertEqual(3, result); // Expecting 3 bytes written
}

void testUartRead(void) {
    uint8_t buffer[10];
    int bytesRead = portUartRead(buffer, sizeof(buffer), 1000);

    TESTAssert(bytesRead > 0); // Assuming some data was read
}

void testUartClearReceiveBuffer(void) {
    portFlushRx();
    // Verify that the buffer was cleared
}
