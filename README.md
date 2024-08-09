
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

## Running Unit Tests

1. Navigate to the project root directory.
2. Run the tests using Ceedling:
   ```bash
   ceedling test:all
   ```

## License

This project is licensed under the MIT License. See the `LICENSE` file for more details.
