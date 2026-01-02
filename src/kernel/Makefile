
BUILD_DIR=$(BUILD_ROOT)/kernel

TARGET_CC=$(TOOLCHAIN_ROOT)/bin/x86_64-elf-gcc
TARGET_LD=$(TOOLCHAIN_ROOT)/bin/x86_64-elf-ld

SOURCES_C=$(wildcard c/*.c)
SOURCES_ASM=$(wildcard asm/*.S)
OBJECTS_C=$(patsubst %.c, $(BUILD_DIR)/%.o, $(SOURCES_C))
OBJECTS_ASM=$(patsubst %.S, $(BUILD_DIR)/%.o, $(SOURCES_ASM))

.PHONY: all

all: clean kernel.elf
	@echo "Kernel build completed!"

kernel.elf: $(BUILD_DIR)/kernel.elf

$(BUILD_DIR)/kernel.elf: $(OBJECTS_ASM) $(OBJECTS_C)
	@$(TARGET_LD) $^ -n -T linker.ld -o $@ 
	@echo "Linking kernel completed successfully!"

clean:
	@rm -rf $(BUILD_DIR)
	@echo "Cleaned kernel build directory."

always:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(dir $(OBJECTS_C))
	@mkdir -p $(dir $(OBJECTS_ASM))

$(BUILD_DIR)/asm/%.o: asm/%.S
	@mkdir -p $(dir $@)
	@$(TARGET_CC) $< -c -o $@
	@echo "Compiled $< successfully!"

$(BUILD_DIR)/c/%.o: c/%.c
	@mkdir -p $(dir $@)
	@$(TARGET_CC) $< -ffreestanding -mno-red-zone -fno-stack-protector -c -I $(LIB_ROOT)/gnu-efi/inc -I $(LIB_ROOT)/gnu-efi/inc/x86_64 -I ./include -o $@
	@echo "Compiled $< successfully!"