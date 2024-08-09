
# XBee C Library Project

## Overview

This project provides a C library for interacting with XBee modules across multiple platforms, including Unix, Arduino, and STM32. The library is designed to be portable, with platform-specific UART implementations for each target environment.

## Project Structure

- **include/**: Contains all the header files for the XBee library.
- **src/**: Contains the source files for the core XBee library.
- **platform/**: Contains platform-specific code and examples.
  - **arduino/**: Arduino-specific UART implementation and example.
  - **stm32/**: STM32-specific UART implementation and example, along with a Makefile.
  - **unix/**: Unix-specific UART implementation and example, along with a Makefile.
- **test/**: Contains unit tests for the library using the Ceedling framework.
- **project.yml**: Ceedling configuration file.

## Building and Running

### Unix

1. Navigate to the Unix platform directory:
   ```bash
   cd platform/unix
   ```
2. Build the project:
   ```bash
   make
   ```
3. Run the resulting binary:
   ```bash
   ./xbee_example
   ```

### STM32

1. Navigate to the STM32 platform directory:
   ```bash
   cd platform/stm32
   ```
2. Build the project using the ARM toolchain:
   ```bash
   make
   ```
3. Flash the resulting binary to your STM32 device.

### Arduino

1. Open the Arduino example (`example.ino`) in the Arduino IDE.
2. Ensure that the necessary libraries are included.
3. Upload the sketch to your Arduino board.

## Using the XBee Class

### Initialization

To use the XBee class, you first need to include the necessary headers and initialize an instance of the XBee class.

```c
#include "xbee.h"

// Create an instance of the XBee class
XBee my_xbee;

// Initialize the XBee instance with UART configuration
xbee_init(&my_xbee, &uart_instance, uart_baud_rate);
```

### Sending AT Commands

You can send AT commands to the XBee module using the `xbee_send_at_command` function.

```c
// Send an AT command (e.g., "ATMY" to get the module's MY address)
xbee_send_at_command(&my_xbee, "MY");

// Handle the response
if (xbee_wait_for_response(&my_xbee) == XBEE_OK) {
    printf("MY address: %s
", my_xbee.response_data);
} else {
    printf("Failed to get response
");
}
```

### Sending Data

To send data to another XBee module, use the `xbee_send_data` function.

```c
uint8_t data[] = "Hello, XBee!";
xbee_send_data(&my_xbee, destination_address, data, sizeof(data));
```

### Receiving Data

To receive data from another XBee module, use the `xbee_receive_data` function.

```c
uint8_t buffer[100];
int bytes_received = xbee_receive_data(&my_xbee, buffer, sizeof(buffer));

if (bytes_received > 0) {
    printf("Received data: %s
", buffer);
} else {
    printf("No data received
");
}
```

### Closing the XBee Instance

When you are done using the XBee instance, make sure to close it properly.

```c
xbee_close(&my_xbee);
```

## Running Unit Tests

1. Navigate to the project root directory.
2. Run the tests using Ceedling:
   ```bash
   ceedling test:all
   ```

## License

This project is licensed under the MIT License. See the `LICENSE` file for more details.
