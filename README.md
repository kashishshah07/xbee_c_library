
# XBee C Library

## Overview
This library provides a framework for interfacing with XBee modules using an XBee API that abstracts AT commands and API frames. It supports multiple platforms including Unix, STM32, RP2350, and EFM32.

## Library Structure
- **src**: Contains the core source files implementing XBee classes and APIs.
- **include**: Contains header files exposing the library's API.
- **examples**: Contains example implementations for various platforms.
- **test**: Contains unit tests for validating the functionality of the library.

## How to Run Examples
1. Choose the example that matches your platform (e.g., Unix, STM32).
2. Compile the example using your platform's toolchain.
3. Run the compiled binary to see the XBee communication in action.

## How to Port to Other Platforms
1. Implement the `port_*` files for your target platform.
2. Ensure you provide the necessary low-level functions (e.g., UART initialization, send/receive).
3. Use the examples as a guide to develop your platform-specific example.

## How to Run Unit Tests
1. Navigate to the `test` directory.
2. Compile the test files using your platform's toolchain.
3. Run the compiled binary to execute the unit tests.

## How to Add Support for Other XBee Subclasses
1. Extend the XBee API, API Frames, and AT commands files as needed for the new subclass.
2. Add the necessary support functions in the source files.
3. Update the examples to demonstrate the new functionality.

## Detailed File Breakdown
- **xbee.c**: Implements the XBee class
- **xbee_at_cmds.c**: Implements functions for sending and receiving AT commands.
- **xbee_lr.c**: Implements XBee LR module subclass.
- **xbee_api_frames.c**: Implements parsing and handling of API frames.

## Library Architecture
The library is designed to be modular, allowing easy expansion and support for different XBee modules and platforms. The main components include:
- **XBee Core**: Handles core XBee Class and API layer.
- **AT Commands**: Manages AT command interface.
- **API Frames**: Handles API frame communication.

## License 
See MIT-LICENSE.txt file