ASM=nasm
CC=gcc
CC16=wcc
LD16=wlink

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
	dd if=$(BUILD_DIR)/boot.bin of=$(BIN_DIR)/DIOS.img conv=notrunc
	mcopy -i $(BIN_DIR)/DIOS.img $(BUILD_DIR)/setup.bin "::setup.bin"
	mcopy -i $(BIN_DIR)/DIOS.img $(BUILD_DIR)/kernel.bin "::kernel.bin"
	mcopy -i $(BIN_DIR)/DIOS.img data/test.txt "::test.txt"

#
# Bootloader
#
bootloader: always
	$(MAKE) -C $(SRC_DIR)/bootloader/boot BUILD_DIR=$(abspath $(BUILD_DIR)) ASM=$(ASM)
	$(MAKE) -C $(SRC_DIR)/bootloader/setup BUILD_DIR=$(abspath $(BUILD_DIR)) ASM=$(ASM) CC16=$(CC16) LD16=$(LD16)

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
	rm -rf $(BUILD_DIR)/*
	rm -rf $(BIN_DIR)/*

#
# Tools
#

tools:
	$(CC) tools/fat/fat.c -o $(BIN_DIR)/fat