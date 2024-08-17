#include "unity.h"
#include "xbee.h"

void setUp(void) {}

void tearDown(void) {}

void testXbeeInit(void) {
    xbee_t xbee;
    xbeeInit(&xbee);

    TESTAssertEqual(0, xbee.status); // Replace with actual status checks
}

void testXbeeSendFrame(void) {
    xbee_t xbee;
    xbeeInit(&xbee);

    uint8_t data[] = { 0x01, 0x02 };
    int result = xbeeSendFrame(&xbee, data, sizeof(data));

    TESTAssertEqual(0, result); // Expecting a successful send
}

void testXbeeToString(void) {
    xbee_t xbee;
    xbeeInit(&xbee);

    const char* result = xbeeToString(&xbee);
    TESTAssertNotNull(result); // Expecting a valid string
}

void testAtCommandToString(void) {
    const char* result = atCommandToString(AT_DE);
    TESTAssertEqualString("DE", result); // Replace with expected command string
}
