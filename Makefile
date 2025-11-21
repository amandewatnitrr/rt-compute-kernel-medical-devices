# Compiler and flags
CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -std=c99
LDFLAGS =

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin
TEST_DIR = tests

# Source files
SRCS = $(SRC_DIR)/kernel.c $(SRC_DIR)/task.c $(SRC_DIR)/scheduler.c $(SRC_DIR)/ipc.c $(SRC_DIR)/timer.c
MAIN_SRC = $(SRC_DIR)/main.c
TEST_SRCS = $(TEST_DIR)/test_task.c $(TEST_DIR)/test_scheduler.c $(TEST_DIR)/test_main.c

# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BIN_DIR)/%.o)
MAIN_OBJ = $(MAIN_SRC:$(SRC_DIR)/%.c=$(BIN_DIR)/%.o)
TEST_OBJS = $(filter-out $(BIN_DIR)/test_main.o, $(TEST_SRCS:$(TEST_DIR)/%.c=$(BIN_DIR)/%.o))
TEST_MAIN_OBJ = $(BIN_DIR)/test_main.o

# Executables
TARGET = $(BIN_DIR)/medical_kernel
TEST_TARGET = $(BIN_DIR)/run_tests

# Default target
all: $(TARGET) $(TEST_TARGET)

# Create bin directory
$(shell mkdir -p $(BIN_DIR))

# Link the main application
$(TARGET): $(OBJS) $(MAIN_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Link the test runner
$(TEST_TARGET): $(OBJS) $(TEST_OBJS) $(TEST_MAIN_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Compile source files into object files
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile test files into object files
$(BIN_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the main application
run: all
	./$(TARGET)

# Run tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Clean up build artifacts
clean:
	rm -rf $(BIN_DIR)

.PHONY: all run test clean
