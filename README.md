
# XBee C Library

### Overview
This library provides a framework for interfacing with XBee modules using an XBee API that abstracts AT commands and API frames. It supports multiple platforms including Unix, STM32, RP2350, and EFM32.

### Library Structure
- **src**: Contains the core source files implementing XBee classes and APIs.
- **include**: Contains header files exposing the library's API.
- **examples**: Contains example implementations for various platforms.
- **test**: Contains unit tests for validating the functionality of the library.

### How to Run Examples
1. Choose the example that matches your platform (e.g., Unix, STM32).
2. Compile the example using your platform's toolchain.
3. Run the compiled binary to see the XBee communication in action.

### How to Run Unit Tests
1. Navigate to the `test` directory.
2. Compile the test files using your platform's toolchain.
3. Run the compiled binary to execute the unit tests.

### Currently Supported XBees
- XBee LR (LoRaWAN)

### How to Add Support for Other XBee Subclasses
1. Extend the XBee API, API Frames, and AT commands files as needed for the new subclass.
2. Add the necessary support functions in the source files.
3. Update the examples to demonstrate the new functionality.

### Detailed File Breakdown
- **xbee.c**: Implements the XBee class
- **xbee_at_cmds.c**: Implements functions for sending and receiving AT commands.
- **xbee_lr.c**: Implements XBee LR module subclass.
- **xbee_api_frames.c**: Implements parsing and handling of API frames.

### Library Architecture
The library is designed to be modular, allowing easy expansion and support for different XBee modules and platforms. The main components include:
- **XBee Core**: Handles core XBee Class and API layer.
- **AT Commands**: Manages AT command interface.
- **API Frames**: Handles API frame communication.

### License 
See MIT-LICENSE.txt file

## Overview of the XBee Class Methods

The `xbee.c` file implements a set of methods that provide an interface for interacting with XBee modules through the use of a virtual table (vtable). These methods allow for initialization, connection, data transmission, and reset operations. Below is a detailed overview of the key methods:

### `XBee_Init`

**Purpose**:  
Initializes the XBee module by setting up the initial frame ID counter and calling the subclass-specific initialization routine.

**Parameters**:
- `self`: Pointer to the XBee instance.
- `baudrate`: The baud rate for serial communication.
- `device`: The path to the serial device (e.g., `/dev/ttyUSB0`).

**Returns**:  
`True` if the initialization is successful; otherwise, `False`.

### `XBee_Connect`

**Purpose**:  
Connects the XBee module to the network by invoking the connection implementation defined in the subclass.

**Parameters**:
- `self`: Pointer to the XBee instance.

**Returns**:  
`True` if the connection is successful; otherwise, `False`.

### `XBee_Disconnect`

**Purpose**:  
Disconnects the XBee module from the network using the subclass-specific disconnection routine.

**Parameters**:
- `self`: Pointer to the XBee instance.

**Returns**:  
`True` if the disconnection is successful; otherwise, `False`.

### `XBee_SendData`

**Purpose**:  
Sends data over the network by calling the subclass-specific data transmission method. This is a blocking function.

**Parameters**:
- `self`: Pointer to the XBee instance.
- `data`: The data to be sent.

**Returns**:  
`True` if the data was sent successfully; otherwise, `False`.

### `XBee_SoftReset`

**Purpose**:  
Performs a soft reset of the XBee module by invoking the `soft_reset` method in the subclass's virtual table. A soft reset typically resets the module's state without a full power cycle.

**Parameters**:
- `self`: Pointer to the XBee instance.

**Returns**:  
`True` if the soft reset is successful; otherwise, `False`.

### `XBee_HardReset`

**Purpose**:  
Performs a hard reset of the XBee module by invoking the `hard_reset` method in the subclass's virtual table. This typically involves a full power cycle or reset through the reset pin.

**Parameters**:
- `self`: Pointer to the XBee instance.

**Returns**:  
`Void` - This function does not return a value.

### `XBee_Process`

**Purpose**:  
Calls the subclass's process implementation, which is responsible for handling ongoing tasks or events related to the XBee module. This method should be called continuously in the application's main loop to ensure proper operation.

**Parameters**:
- `self`: Pointer to the XBee instance.

**Returns**:  
`Void` - This function does not return a value.

---

These methods provide a foundational interface for working with XBee modules, supporting various operations like initialization, connection management, data transmission, and both soft and hard resets.



## Usage Example: Communicating with XBee Modules

This section provides an example of how to use the XBee class to communicate with XBee modules. The example covers creating an instance of the XBee class, initializing the XBee module, connecting to a network, sending data, and handling received data. The code is written to be adaptable to various platforms, with platform-specific details abstracted through the library's API.

### Creating the XBee Instance

Before performing any operations, create an instance of the XBee class using the `XBee_Create` function:

```c
#include "xbee.h"
#include "port.h"

// Define a pointer for the XBee instance
XBee* my_xbee;

int main() {
    // Create the XBee instance
    my_xbee = XBee_Create();
    if (my_xbee == NULL) {
        printf("Failed to create XBee instance.
");
        return -1;
    }

    // Proceed with initialization and connection...
}
```

### Initialization and Connection

After creating the XBee instance, initialize the XBee module and connect to the network:

```c
    // Initialize the XBee module
    if (!XBee_Init(my_xbee, 9600, "COM3")) {  // Replace "COM3" with the appropriate serial port for your platform
        printf("Failed to initialize XBee module.
");
        return -1;
    }

    // Connect to the network
    if (!XBee_Connect(my_xbee)) {
        printf("Failed to connect to the network.
");
        return -1;
    }

    printf("XBee module initialized and connected.
");

    // Proceed with other operations...
}
```

### Sending Data

To send data over the XBee network, use the `XBee_SendData` method:

```c
// Data to send
const char* data = "Hello, XBee Network!";

if (!XBee_SendData(my_xbee, data)) {
    printf("Failed to send data.
");
} else {
    printf("Data sent successfully.
");
}
```

### Receiving Data

Implement a callback function to handle data received from the XBee module:

```c
void data_received_callback(const char* data) {
    printf("Data received: %s
", data);
}

// Register the callback (assuming XBee_Process is handling incoming data in a loop)
XBee_RegisterReceiveCallback(my_xbee, data_received_callback);

// In your main loop, process incoming data:
while (1) {
    XBee_Process(my_xbee);
    // Sleep or delay mechanism, platform-specific, e.g., usleep(10000) on Unix, Sleep(10) on Windows, etc.
}
```

### Closing the Connection

After completing your operations, disconnect the XBee module, clean up resources, and free the XBee instance:

```c
XBee_Disconnect(my_xbee);
XBee_Close(my_xbee);
XBee_Destroy(my_xbee);  // Free the XBee instance
printf("XBee module disconnected and resources cleaned up.
");
```

### Full Example

Here is the full example code combining the steps mentioned above:

```c
#include "xbee.h"
#include "port.h"

XBee* my_xbee;

void data_received_callback(const char* data) {
    printf("Data received: %s
", data);
}

int main() {
    // Create the XBee instance
    my_xbee = XBee_Create();
    if (my_xbee == NULL) {
        printf("Failed to create XBee instance.
");
        return -1;
    }

    // Initialize the XBee module
    if (!XBee_Init(my_xbee, 9600, "COM3")) {  // Replace "COM3" with the appropriate serial port
        printf("Failed to initialize XBee module.
");
        return -1;
    }

    // Connect to the network
    if (!XBee_Connect(my_xbee)) {
        printf("Failed to connect to the network.
");
        return -1;
    }

    printf("XBee module initialized and connected.
");

    XBee_RegisterReceiveCallback(my_xbee, data_received_callback);

    const char* data = "Hello, XBee Network!";
    if (!XBee_SendData(my_xbee, data)) {
        printf("Failed to send data.
");
    } else {
        printf("Data sent successfully.
");
    }

    // Main loop to process incoming data
    while (1) {
        XBee_Process(my_xbee);
        // Sleep or delay mechanism, platform-specific
    }

    XBee_Disconnect(my_xbee);
    XBee_Close(my_xbee);
    XBee_Destroy(my_xbee);  // Free the XBee instance
    printf("XBee module disconnected and resources cleaned up.
");

    return 0;
}
```

This example demonstrates the full process of creating an XBee instance, initializing the module, connecting to a network, sending data, and handling incoming data. The code is designed to be adaptable to different platforms, with platform-specific details abstracted by the library.


## How to Port to Other Platforms

Porting the XBee C library to other platforms involves adapting the code to the specific hardware and software environment of the new platform. The following steps outline the general process for porting the library:

### 1. Understand the Target Platform
Before starting the porting process, gather detailed information about the target platform, including:
- CPU architecture
- Operating system (if any)
- Available compilers and development tools
- Peripheral interfaces (UART, SPI, I2C, etc.)

### 2. Configure the Build Environment
Set up a development environment compatible with the target platform. This includes:
- Installing the appropriate compiler and toolchain
- Configuring makefiles or build scripts for the target platform

### 3. Modify Hardware Abstraction Layer (HAL)
The XBee library uses a HAL to interact with hardware peripherals. Modify the HAL implementation to match the target platform's peripherals:
- Update UART initialization and configuration
- Adjust GPIO settings if needed
- Implement any additional platform-specific peripheral control functions

### 4. Adjust Compiler and Linker Settings
Modify the compiler and linker settings to match the target platform:
- Set the correct optimization flags for the target CPU
- Adjust memory settings and linker scripts if required

### 5. Test and Debug
Thoroughly test the ported library on the target platform:
- Verify basic communication with the XBee module
- Test all major library functions
- Debug any issues that arise during testing

### 6. Optimize for Performance
After successful porting and testing, consider optimizing the library for performance on the target platform:
- Optimize critical sections of code for speed
- Reduce memory usage if necessary
- Take advantage of platform-specific features (e.g., DMA for UART transfers)

### 7. Document the Port
Finally, document any changes made during the porting process and provide instructions for others who may need to port the library to similar platforms.


## Adding a New Subclass for a New XBee Module

If you want to add support for a new XBee module, you will need to create a new subclass that extends the functionality of the base XBee class. This guide will walk you through the steps to add a new subclass and integrate it into the existing XBee framework.

### 1. Define the Subclass Structure

Start by creating a new header and source file for your subclass. Let's assume your new XBee module is called `XBeeNew`. Create `xbee_new.h` and `xbee_new.c`.

In `xbee_new.h`, define the subclass structure:

```c
#ifndef XBEE_NEW_H
#define XBEE_NEW_H

#include "xbee.h"

typedef struct {
    XBee base;  /**< Base class structure. */
    // Add any additional members specific to XBeeNew
} XBeeNew;

// Function prototypes for the subclass
XBeeNew* XBeeNew_Create(const XBeeCTable* cTable, const XBeeHTable* hTable);
void XBeeNew_Destroy(XBeeNew* self);

#endif // XBEE_NEW_H
```

### 2. Implement the Subclass Methods

In `xbee_new.c`, implement the methods specific to your new XBee module. This will include the creation and destruction methods, as well as any specific behaviors your module requires.

```c
#include "xbee_new.h"
#include <stdlib.h>

static bool XBeeNew_Init(XBee* self, uint32_t baudrate, const char* device);
static void XBeeNew_Process(XBee* self);
static bool XBeeNew_Connect(XBee* self);
static bool XBeeNew_Disconnect(XBee* self);
static bool XBeeNew_SendData(XBee* self, const void* data);
static bool XBeeNew_Connected(XBee* self);
static void XBeeNew_Handle_Rx_Packet(XBee* self, void* param);
static void XBeeNew_Handle_Transmit_Status(XBee* self, void* param);

// Define the VTable for the new XBee module
static const XBeeVTable XBeeNew_VTable = {
    .init = XBeeNew_Init,
    .process = XBeeNew_Process,
    .connect = XBeeNew_Connect,
    .disconnect = XBeeNew_Disconnect,
    .send_data = XBeeNew_SendData,
    .soft_reset = XBee_SoftReset,
    .hard_reset = XBee_HardReset,
    .connected = XBeeNew_Connected,
    .handle_rx_packet_frame = XBeeNew_Handle_Rx_Packet,
    .handle_transmit_status_frame = XBeeNew_Handle_Transmit_Status,
};

// Implement the create and destroy methods
XBeeNew* XBeeNew_Create(const XBeeCTable* cTable, const XBeeHTable* hTable) {
    XBeeNew* instance = (XBeeNew*)malloc(sizeof(XBeeNew));
    instance->base.vtable = &XBeeNew_VTable;
    instance->base.htable = hTable;
    instance->base.ctable = cTable;
    return instance;
}

void XBeeNew_Destroy(XBeeNew* self) {
    free(self);
}

// Implement the specific methods for the new XBee module
static bool XBeeNew_Init(XBee* self, uint32_t baudrate, const char* device) {
    // Implement initialization logic specific to XBeeNew
    return true;
}

static void XBeeNew_Process(XBee* self) {
    // Implement process logic specific to XBeeNew
}

static bool XBeeNew_Connect(XBee* self) {
    // Implement connection logic specific to XBeeNew
    return true;
}

static bool XBeeNew_Disconnect(XBee* self) {
    // Implement disconnection logic specific to XBeeNew
    return true;
}

static bool XBeeNew_SendData(XBee* self, const void* data) {
    // Implement data sending logic specific to XBeeNew
    return true;
}

static bool XBeeNew_Connected(XBee* self) {
    // Implement logic to check if XBeeNew is connected
    return true;
}

static void XBeeNew_Handle_Rx_Packet(XBee* self, void* param) {
    // Implement logic to handle received packets specific to XBeeNew
}

static void XBeeNew_Handle_Transmit_Status(XBee* self, void* param) {
    // Implement logic to handle transmit status specific to XBeeNew
}
```

### 3. Integrate the Subclass into Your Application

After defining and implementing your subclass, you can use it in your application by creating an instance of `XBeeNew` and using it like any other XBee module. See various example code.

### 4. Customize the Subclass as Needed

Depending on the specific features and requirements of your new XBee module, you may need to add or modify methods and behaviors in the subclass. The provided structure and functions offer a flexible framework that can be easily extended to support additional features.
