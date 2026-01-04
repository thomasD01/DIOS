# global makefile

export BUILD_ROOT = $(abspath build)
export BIN_DIR = $(abspath bin)
export TOOLCHAIN_ROOT = $(abspath toolchain/build_toolchain/install)
export LIB_ROOT = $(abspath lib)

export PATH := $(TOOLCHAIN_ROOT)/bin:$(PATH)

BINUTILS_VERSION = 2.45
BINUTILS_URL = https://ftp.gnu.org/gnu/binutils/binutils-$(BINUTILS_VERSION).tar.xz

GCC_VERSION = 15.2.0
GCC_URL = https://ftp.gnu.org/gnu/gcc/gcc-$(GCC_VERSION)/gcc-$(GCC_VERSION).tar.xz

.PHONY: all

all: clean bootloader kernel sysroot

toolchain: toolchain/build_toolchain

toolchain/build_toolchain:
	@mkdir -p toolchain/build_toolchain
	@cd toolchain/build_toolchain && cmake .. && cmake --build .

lib:
	@$(MAKE) -C $(LIB_ROOT)/gnu-efi
	@echo "Libraries built successfully!"

bootloader: lib
	$(MAKE) -C src/boot efi

kernel: lib
	$(MAKE) -C src/kernel kernel.elf

sysroot:
	$(MAKE) -C data sysroot

clean:
	rm -rf $(BUILD_ROOT)
