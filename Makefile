# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

# Directories
SRC_DIR = src
BUILD_DIR = .

# Target executable
TARGET = airomsShell

# Source files
SOURCES = $(SRC_DIR)/main.c

# Default target
all: $(TARGET)

# Build the shell
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)
	@echo "Build successful! Run with: ./$(TARGET)"

# Clean build artifacts
clean:
	rm -f $(TARGET)
	@echo "Clean complete"

# Run the shell
run: $(TARGET)
	./$(TARGET)

# Help
help:
	@echo "MyShell Makefile"
	@echo "Usage:"
	@echo "  make          - Build the shell"
	@echo "  make clean    - Remove built files"
	@echo "  make run      - Build and run the shell"
	@echo "  make help     - Show this help message"

.PHONY: all clean run help
