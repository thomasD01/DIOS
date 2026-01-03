
BUILD_DIR=$(BUILD_ROOT)/kernel

TARGET_CC=$(TOOLCHAIN_ROOT)/bin/x86_64-elf-gcc
TARGET_CXX=$(TOOLCHAIN_ROOT)/bin/x86_64-elf-g++
TARGET_LD=$(TOOLCHAIN_ROOT)/bin/x86_64-elf-ld

SOURCES_C=$(shell find c -name "*.c")
SOURCES_CPP=$(shell find c -name "*.cpp")
SOURCES_ASM=$(shell find asm -name "*.S")

OBJECTS_C=$(patsubst %.c, $(BUILD_DIR)/%.c.o, $(SOURCES_C))
OBJECTS_CPP=$(patsubst %.cpp, $(BUILD_DIR)/%.cpp.o, $(SOURCES_CPP))
OBJECTS_ASM=$(patsubst %.S, $(BUILD_DIR)/%.asm.o, $(SOURCES_ASM))

.PHONY: all

all: clean kernel.elf
	@echo "Kernel build completed!"

kernel.elf: $(BUILD_DIR)/kernel.elf

$(BUILD_DIR)/kernel.elf: $(OBJECTS_ASM) $(OBJECTS_C) $(OBJECTS_CPP)
	@$(TARGET_LD) $^ -n -T linker.ld -o $@ 
	@echo "Linking kernel completed successfully!"

clean:
	@rm -rf $(BUILD_DIR)
	@echo "Cleaned kernel build directory."

always:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(dir $(OBJECTS_C))
	@mkdir -p $(dir $(OBJECTS_ASM))

$(BUILD_DIR)/asm/%.asm.o: asm/%.S
	@mkdir -p $(dir $@)
	@$(TARGET_CC) $< -c -o $@
	@echo "Compiled $< successfully!"

$(BUILD_DIR)/c/%.c.o: c/%.c
	@mkdir -p $(dir $@)
	@$(TARGET_CC) $< -ffreestanding -mno-red-zone -fno-stack-protector -c -I $(LIB_ROOT)/gnu-efi/inc -I $(LIB_ROOT)/gnu-efi/inc/x86_64 -I ./include -o $@
	@echo "Compiled $< successfully!"

$(BUILD_DIR)/c/%.cpp.o: c/%.cpp
	@mkdir -p $(dir $@)
	@$(TARGET_CXX) $< -ffreestanding -mno-red-zone -fno-stack-protector -fno-exceptions -fno-rtti -c -I $(LIB_ROOT)/gnu-efi/inc -I $(LIB_ROOT)/gnu-efi/inc/x86_64 -I ./include -o $@
	@echo "Compiled $< successfully!"