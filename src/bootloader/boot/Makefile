BUILD_DIR?=build
ASM?=nasm

.PHONY: all

all: boot

boot: $(BUILD_DIR)/boot.bin

$(BUILD_DIR)/boot.bin: boot.asm
	@$(ASM) $< -f bin -o $(BUILD_DIR)/boot.bin
	@echo "Bootloader compiled successfully!"
