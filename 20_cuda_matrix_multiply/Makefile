SDK    := $(shell xcrun --sdk macosx --show-sdk-path)
BUILD  := build
APP    := $(BUILD)/matmul
METAL  := xcrun -sdk macosx metal
METALLIB := xcrun -sdk macosx metallib
CLANG  := clang
CFLAGS := -fobjc-arc -framework Metal -framework Foundation -isysroot $(SDK)

run: $(APP)
	$(APP)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/matmul.air: matmul.metal | $(BUILD)
	$(METAL) -c $< -o $@

$(BUILD)/matmul.metallib: $(BUILD)/matmul.air
	$(METALLIB) $< -o $@

$(APP): main.c $(BUILD)/matmul.metallib
	$(CLANG) -x objective-c $(CFLAGS) main.c -o $(APP)

clean:
	rm -rf $(BUILD)
