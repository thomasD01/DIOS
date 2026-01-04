
BUILD_DIR=$(BUILD_ROOT)/data

.PHONY: all

all: clean sysroot

sysroot: $(BUILD_DIR)/rootfs.tar
	@echo "Sysroot image created successfully!"

$(BUILD_DIR)/rootfs.tar:
	@mkdir -p $(BUILD_DIR)
	@tar -cvf $(BUILD_DIR)/rootfs.tar -C sysroot .

clean:
	@rm -rf $(BUILD_DIR)