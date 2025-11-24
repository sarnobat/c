# Makefile for C/Objective-C + Metal GPU example

# Executable and source
TARGET = multiply_metal
SRC = multiply_metal.m   # Objective-C source

# Default factor (can be overridden by make FACTOR=5)
FACTOR ?= 4

# Default target: build and run
all: $(TARGET)
	@echo "Running $(TARGET) with factor $(FACTOR)..."
	@./$(TARGET) $(FACTOR)

# Build the Metal executable for Apple Silicon
$(TARGET): $(SRC)
	clang -arch arm64 -framework Metal -framework Foundation $(SRC) -o $(TARGET)

# Clean build artifacts
clean:
	@rm -f $(TARGET)

diff:
	/Volumes/apps/Apps/DiffMerge.app/Contents/MacOS/DiffMerge /Volumes/git/github/depot_parent/python/20_pytorch_or_mac_metal/multiply.py multiply_metal.m