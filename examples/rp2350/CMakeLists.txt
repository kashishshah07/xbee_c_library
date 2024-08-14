# Set minimum CMake version
cmake_minimum_required(VERSION 3.13)

# Set project name
project(xbee_example_project C CXX ASM)

# Include the Pico SDK
include(/Users/felix/Projects/Code/pico-sdk/external/pico_sdk_import.cmake)

# Set the project to use C
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Pico SDK
pico_sdk_init()

# Define the executable for your project
add_executable(xbee_example
    example.c
    port_rp2350.c
)

# Add necessary libraries for the Pico SDK
target_link_libraries(xbee_example pico_stdlib hardware_uart)

# Enable USB serial output, optional but useful for debugging
pico_enable_stdio_usb(xbee_example 1)
pico_enable_stdio_uart(xbee_example 0)  # Disable stdio on the UART (we're using uart0 for debugging)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(xbee_example)
