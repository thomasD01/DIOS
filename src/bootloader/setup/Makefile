BUILD_DIR?=build
ASM?=nasm
ASMFLAGS?=-f obj
CC16?=wcc
C16FLAGS?=-4 -d3 -s -wx -ms -zl -zq  
LD16?=wlink

SOURCES_C=$(wildcard c/*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS_C=$(patsubst %.c, $(BUILD_DIR)/bootloader/%.obj, $(SOURCES_C))
OBJECTS_ASM=$(patsubst %.asm, $(BUILD_DIR)/bootloader/%.obj, $(SOURCES_ASM))

.PHONY: all

all: $(BUILD_DIR)/setup.bin

$(BUILD_DIR)/setup.bin: $(OBJECTS_ASM) $(OBJECTS_C)
	$(LD16) NAME $(BUILD_DIR)/setup.bin FILE \{ $(OBJECTS_ASM) $(OBJECTS_C) \} OPTION MAP=$(BUILD_DIR)/setup.map @linker.lnk

$(BUILD_DIR)/bootloader/asm/%.obj: asm/%.asm always
	$(ASM) $(ASMFLAGS) -o $@ $<

$(BUILD_DIR)/bootloader/c/%.obj: c/%.c always
	$(CC16) $(C16FLAGS) -fo=$@ $<

always:
	mkdir -p $(BUILD_DIR)/bootloader
	mkdir -p $(BUILD_DIR)/bootloader/asm
	mkdir -p $(BUILD_DIR)/bootloader/c
