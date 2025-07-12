# === Compiler and Flags ===
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude -Isrc -Ilib/unity

# === Directories ===
SRC_DIR = src
TEST_DIR = test
UNITY_DIR = lib/unity
BUILD_DIR = build

# === File Discovery ===
SRC_FILES   := $(wildcard $(SRC_DIR)/*.c)
TEST_FILES  := $(wildcard $(TEST_DIR)/*.c)
UNITY_FILES := $(wildcard $(UNITY_DIR)/*.c)

# === Output Target ===
TARGET = $(BUILD_DIR)/test_runner

# === Default Rule ===
all: test

# === Build Directory Rule ===
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# === Link and Build ===
$(TARGET): $(SRC_FILES) $(TEST_FILES) $(UNITY_FILES) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# === Run Tests ===
test: $(TARGET)
	./$(TARGET)

# === Cleanup ===
clean:
	rm -rf $(BUILD_DIR)

