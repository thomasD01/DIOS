include build_scripts/config.mk

.PHONY: all image bootloader kernel clean always

all: image

include build_scripts/toolchain.mk

#
# Image
#
image: $(BIN_DIR)/DIOS.img

$(BIN_DIR)/DIOS.img: bootloader kernel
	@dd if=/dev/zero of=$@ bs=512 count=2880 > /dev/null
	@mkfs.fat -F 12 -n "DIOS" $@ > /dev/null
	@dd if=$(BUILD_DIR)/boot.bin of=$@ conv=notrunc > /dev/null
	@mcopy -i $@ $(BUILD_DIR)/setup.bin "::setup.bin"
	@mcopy -i $@ $(BUILD_DIR)/kernel.bin "::kernel.bin"
	@mcopy -i $@ data/test.txt "::test.txt"
	@mmd -i $@ "::mydir"
	@mcopy -i $@ data/test.txt "::mydir/test.txt"

#
# Bootloader
#
bootloader: always
	@$(MAKE) -C src/bootloader/boot BUILD_DIR=$(abspath $(BUILD_DIR))
	@$(MAKE) -C src/bootloader/setup BUILD_DIR=$(abspath $(BUILD_DIR))

#
# Kernel
#
kernel: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: always
	@$(MAKE) -C src/kernel BUILD_DIR=$(abspath $(BUILD_DIR))

#
# Always
#
always:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)

#
# Clean
#
clean:
	@rm -rf $(BUILD_DIR)/*
	@rm -rf $(BIN_DIR)/*
