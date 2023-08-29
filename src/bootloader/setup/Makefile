TARGET_ASMFLAGS += -f elf
TARGET_CFLAGS += -ffreestanding -nostdlib
TARGET_LIBS += -lgcc
TARGET_LINKFLAGS += -T linker.ld -nostdlib

SOURCES_C=$(wildcard c/*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS_C=$(patsubst %.c, $(BUILD_DIR)/bootloader/%.obj, $(SOURCES_C))
OBJECTS_ASM=$(patsubst %.asm, $(BUILD_DIR)/bootloader/%.obj, $(SOURCES_ASM))

.PHONY: all

all: $(BUILD_DIR)/setup.bin

$(BUILD_DIR)/setup.bin: $(OBJECTS_ASM) $(OBJECTS_C)
	@$(TARGET_LD) $(TARGET_LINKFLAGS) -Wl,-Map=$(BUILD_DIR)/setup.map -o $@ $^ $(TARGET_LIBS)
	@echo "Setup compiled successfully!"

$(BUILD_DIR)/bootloader/asm/%.obj: asm/%.asm always
	@mkdir -p $(@D)
	@$(TARGET_ASM) $(TARGET_ASMFLAGS) -o $@ $<
	@echo "Compiled $< successfully!"

$(BUILD_DIR)/bootloader/c/%.obj: c/%.c always
	@mkdir -p $(@D)
	@$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $<
	@echo "Compiled $< successfully!"

always:
	@mkdir -p $(BUILD_DIR)/bootloader
	@mkdir -p $(BUILD_DIR)/bootloader/asm
	@mkdir -p $(BUILD_DIR)/bootloader/c
