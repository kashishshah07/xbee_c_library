
# Makefile for the XBee LR library Unix example

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -I../../include

# Directories
SRC_DIR = ../../src
INC_DIR = ../../include
BUILD_DIR = build
EXAMPLE_DIR = .

# Source files
SRCS = $(SRC_DIR)/xbee.c $(SRC_DIR)/xbee_api_frames.c $(SRC_DIR)/xbee_at_cmds.c
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Example files
EXAMPLE_SRCS = $(EXAMPLE_DIR)/uart_unix.c $(EXAMPLE_DIR)/example.c
EXAMPLE_OBJS = $(patsubst $(EXAMPLE_DIR)/%.c,$(BUILD_DIR)/%.o,$(EXAMPLE_SRCS))

# Output binary
TARGET = xbee_example

# Create build directory if not exists
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Build rules
all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(EXAMPLE_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS) $(EXAMPLE_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Clean up build directory and binary
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
