# Makefile for building the EFI bootloader

BUILD_DIR=$(BUILD_ROOT)/boot

TARGET_CC=$(TOOLCHAIN_ROOT)/bin/x86_64-elf-gcc
TARGET_LD=$(TOOLCHAIN_ROOT)/bin/x86_64-elf-ld
TARGET_OBJCOPY=$(TOOLCHAIN_ROOT)/bin/x86_64-elf-objcopy

SOURCES_C=$(wildcard c/*.c)
SOURCES_ASM=$(wildcard asm/*.S)
OBJECTS_C=$(patsubst %.c, $(BUILD_DIR)/%.o, $(SOURCES_C))
OBJECTS_ASM=$(patsubst %.S, $(BUILD_DIR)/%.o, $(SOURCES_ASM))

.PHONY: all

all: clean efi

efi: $(BUILD_DIR)/BOOTX64.EFI
	@echo "EFI Bootloader build completed!"

clean:
	@rm -rf $(BUILD_DIR)
	@echo "Cleaned build directory."

always:
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/BOOTX64.EFI: $(BUILD_DIR)/main.elf
	@$(TARGET_OBJCOPY) -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-x86_64 $< $@
	@echo "EFI Bootloader created successfully!"

$(BUILD_DIR)/main.elf: $(OBJECTS_ASM) $(OBJECTS_C)
	@$(TARGET_LD) $^ $(LIB_ROOT)/gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o -nostdlib -znocombreloc -T $(LIB_ROOT)/gnu-efi/gnuefi/elf_x86_64_efi.lds -shared -Bsymbolic -L $(LIB_ROOT)/gnu-efi/x86_64/gnuefi -L $(LIB_ROOT)/gnu-efi/x86_64/lib -l:libgnuefi.a -l:libefi.a -Map $(BUILD_DIR)/bootloader.map -o $@
	@echo "Linking completed successfully!"

$(BUILD_DIR)/asm/%.o: asm/%.S always
	@mkdir -p $(dir $@)
	@$(TARGET_ASM) $< -c -o $@
	@echo "Compiled $< successfully!"

$(BUILD_DIR)/c/%.o: c/%.c always
	@mkdir -p $(dir $@)
	@$(TARGET_CC) $< -c -fno-stack-protector -ffreestanding -fpic -fshort-wchar -mno-red-zone -I $(LIB_ROOT)/gnu-efi/inc -I $(LIB_ROOT)/gnu-efi/inc/x86_64 -DEFI_FUNCTION_WRAPPER -o $@
	@echo "Compiled $< successfully!"