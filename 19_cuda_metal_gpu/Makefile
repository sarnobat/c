# Makefile for C/Objective-C + Metal GPU example

# Executable and source
TARGET = multiply_metal
SRC    = multiply_metal.m   # Objective-C source

# Default factor (can be overridden by:  make FACTOR=5)
FACTOR ?= 4

# Architecture-specific intermediates
ARM64_BIN  = $(TARGET).arm64
X86_BIN    = $(TARGET).x86_64

# Default target: build and run
all: $(TARGET)
	@echo "Running $(TARGET) with factor $(FACTOR)..."
	@./$(TARGET) $(FACTOR)

# Build universal binary (arm64 + x86_64)
$(TARGET): $(SRC)
	@echo "Compiling for arm64..."
	@clang -arch arm64 -framework Metal -framework Foundation $(SRC) -o $(ARM64_BIN)

	@echo "Compiling for x86_64..."
	@clang -arch x86_64 -framework Metal -framework Foundation $(SRC) -o $(X86_BIN)

	@echo "Creating universal binary..."
	@lipo -create -output $(TARGET) $(ARM64_BIN) $(X86_BIN)
	@echo "Built universal binary: $(TARGET)"

# Clean build artifacts
clean:
	@rm -f $(TARGET) $(ARM64_BIN) $(X86_BIN)

# File diff target
diff:
	# not as useful as I was hoping
	# /Volumes/apps/Apps/DiffMerge.app/Contents/MacOS/DiffMerge \
	#     /Volumes/git/github/depot_parent/python/20_pytorch_or_mac_metal/multiply.py \
	#     multiply_metal.m
	# Doesn't work: cd /Volumes/git/github/depot_parent/python/20_pytorch && make diff
