# Makefile for compiling ipv4 program for ARM64

# Compiler
CC = aarch64-linux-gnu-gcc

# Compiler flags
CFLAGS = -Wall -ansi -std=gnu99 -pedantic -g -O2 -pthread -static

# Include and library paths
INCLUDES = -I../gem5_dda/include
LIBS = -L../gem5_dda/util/m5/build/arm64/out -lm5 -lpthread -lm

# Source files
SRC = ipv4.c main.c

# Output executable
TARGET = ipv4_arm64

# Default rule
all: $(TARGET)

# Linking
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(INCLUDES) $(LIBS)

# Clean rule
clean:
	rm -f $(TARGET)

.PHONY: all clean
