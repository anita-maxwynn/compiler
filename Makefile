# Compiler and flags
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Iinclude

# Source files (automatically includes all .cpp files in src/)
SRC = $(wildcard src/*.cpp)

# Build directory and target
BUILD_DIR = build
TARGET = $(BUILD_DIR)/a.out

# Default target
all: $(TARGET)

# Compile and link
$(TARGET): $(SRC)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Run with optional file
run: $(TARGET)
	./$(TARGET) $(FILE)

# Clean up build output
clean:
	rm -f $(TARGET)
