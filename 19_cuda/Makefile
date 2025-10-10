SDK    := $(shell xcrun --sdk macosx --show-sdk-path)
BUILD  := build
APP    := $(BUILD)/hello_metal

METAL     := xcrun -sdk macosx metal
METALLIB  := xcrun -sdk macosx metallib
CLANG     := clang
CFLAGS    := -framework Metal -framework Foundation -fobjc-arc -isysroot $(SDK)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/hello.air: hello.metal | $(BUILD)
	$(METAL) -c $< -o $@

$(BUILD)/hello.metallib: $(BUILD)/hello.air
	$(METALLIB) $< -o $@

$(APP): main.c $(BUILD)/hello.metallib | $(BUILD)
	$(CLANG) -x objective-c $(CFLAGS) $< -o $@

run: $(APP)
	$(APP)

clean:
	rm -rf $(BUILD)

.PHONY: all run clean
