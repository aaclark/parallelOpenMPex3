# Makefile for seives program

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -lm -fopenmp

# Target executable
TARGET = seives

# Source files
SRC = seives.c

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS)

# Clean target
clean:
	rm -f $(TARGET)

# Run target with arguments
run: $(TARGET)
	@echo "Running $(TARGET) with arguments:"
	@read -p "Enter max: " max; \
	read -p "Enter threads: " threads; \
	./$(TARGET) $$max $$threads

.PHONY: all clean run
