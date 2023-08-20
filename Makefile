ASM=nasm
GCC=gcc

SRC_DIR=src
BIN_DIR=bin
BUILD_DIR=build

.PHONY: all image bootloader kernel tools clean always

all: image tools

#
# Image
#
image: $(BIN_DIR)/DIOS.img
$(BIN_DIR)/DIOS.img: bootloader kernel
	dd if=/dev/zero of=$(BIN_DIR)/DIOS.img bs=512 count=2880
	mkfs.fat -F 12 -n "DIOS" $(BIN_DIR)/DIOS.img
	dd if=$(BUILD_DIR)/bootloader.bin of=$(BIN_DIR)/DIOS.img conv=notrunc
	mcopy -i $(BIN_DIR)/DIOS.img $(BUILD_DIR)/kernel.bin "::kernel.bin"
	mcopy -i $(BIN_DIR)/DIOS.img data/test.txt "::test.txt"

#
# Bootloader
#
bootloader: $(BUILD_DIR)/bootloader.bin

$(BUILD_DIR)/bootloader.bin: always
	$(ASM) $(SRC_DIR)/bootloader/boot.asm -f bin -o $(BUILD_DIR)/bootloader.bin

#
# Kernel
#
kernel: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: always
	$(ASM) $(SRC_DIR)/kernel/main.asm -f bin -o $(BUILD_DIR)/kernel.bin

#
# Always
#
always:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)

#
# Clean
#
clean:
	rm -rf $(BUILD_DIR)

#
# Tools
#

tools:
	$(GCC) tools/fat/fat.c -o $(BIN_DIR)/fat