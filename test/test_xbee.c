#include "unity.h"
#include "xbee.h"

void setUp(void) {}

void tearDown(void) {}

void test_xbee_init(void) {
    xbee_t xbee;
    xbee_init(&xbee);

    TEST_ASSERT_EQUAL(0, xbee.status); // Replace with actual status checks
}

void test_xbee_send_frame(void) {
    xbee_t xbee;
    xbee_init(&xbee);

    uint8_t data[] = { 0x01, 0x02 };
    int result = xbee_send_frame(&xbee, data, sizeof(data));

    TEST_ASSERT_EQUAL(0, result); // Expecting a successful send
}

void test_xbee_to_string(void) {
    xbee_t xbee;
    xbee_init(&xbee);

    const char* result = xbee_to_string(&xbee);
    TEST_ASSERT_NOT_NULL(result); // Expecting a valid string
}

void test_at_command_to_string(void) {
    const char* result = at_command_to_string(AT_DE);
    TEST_ASSERT_EQUAL_STRING("DE", result); // Replace with expected command string
}
