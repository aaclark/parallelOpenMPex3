# Makefile for seives program

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -fopenmp -O3 -Wextra

# Adding additional flags from the command line or the Makefile
CFLAGS += $(OUTPUT_FLAG)

# Target executable
TARGET = Game_Of_Life

# Source files
SRC = Game_Of_Life.c

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
	@read -p "Enter ArraySize: " arraysize; \
        read -p "Enter TimeSteps: " timesteps; \
	read -p "Enter Threads: " threads; \
	./$(TARGET) $$arraysize $$timesteps $$threads

.PHONY: all clean run
