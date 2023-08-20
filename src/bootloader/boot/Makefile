BUILD_DIR?=build
ASM?=nasm

.PHONY: all

all: boot

boot: $(BUILD_DIR)/boot.bin

$(BUILD_DIR)/boot.bin:
	$(ASM) boot.asm -f bin -o $(BUILD_DIR)/boot.bin
