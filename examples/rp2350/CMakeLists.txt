# Set minimum CMake version
cmake_minimum_required(VERSION 3.13)

# Set the project to use C
set(CMAKE_C_STANDARD 11)

# Include the Pico SDK
include(/Users/felix/Projects/Code/pico-sdk/external/pico_sdk_import.cmake)

#Add include directory
include_directories(../../include)

# Set project name
project(xbee_example_project C CXX ASM)

# Initialize the Pico SDK
pico_sdk_init()

# Define the source directory
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/../../src")
file(GLOB SOURCES "${SOURCE_DIR}/*.c")

# Define the executable for your project
add_executable(xbee_example
    example.c
    port_rp2350.c
    ${SOURCES}
)

# Add necessary libraries for the Pico SDK
target_link_libraries(xbee_example pico_stdlib hardware_uart hardware_gpio)

# enable usb output, disable uart output
pico_enable_stdio_usb(xbee_example 1)
pico_enable_stdio_uart(xbee_example 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(xbee_example)
