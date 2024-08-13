# Makefile for the XBee LR library Windows example

CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = 

# Files
SRCS = example.c port_windows.c
OBJS = $(SRCS:.c=.o)
TARGET = xbee_example.exe

# Default rule
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compile
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(OBJS) $(TARGET)
