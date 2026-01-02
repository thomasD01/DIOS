#!/bin/bash
# scripts/build_disk.sh

DIRNAME=$(realpath "$(dirname "$0")/..")
BUILD_DIR="${DIRNAME}/build"
ISO_DIR="${BUILD_DIR}/iso"
DISK_IMG="${ISO_DIR}/os.img"
FAT_IMG="${BUILD_DIR}/fat.img"

BOOTLOADER="${BUILD_DIR}/boot/BOOTX64.EFI"
KERNEL="${BUILD_DIR}/kernel/kernel.elf"

mkdir -p "${ISO_DIR}/efi/EFI/BOOT"
cp "${BOOTLOADER}" "${ISO_DIR}/efi/EFI/BOOT/BOOTX64.EFI"
cp "${KERNEL}" "${ISO_DIR}/efi/kernel.elf"

# mkdir -p $ISO_DIR
# rm -f $DISK_IMG

# dd if=/dev/zero of=$FAT_IMG bs=1M count=40 status=none

# mformat -i $FAT_IMG -F ::

# mmd -i $FAT_IMG ::/EFI
# mmd -i $FAT_IMG ::/EFI/BOOT

# mcopy -i $FAT_IMG $BOOTLOADER ::/EFI/BOOT/BOOTX64.EFI
# mcopy -i $FAT_IMG $KERNEL ::/kernel.elf

# dd if=/dev/zero of=$DISK_IMG bs=1M count=48 status=none

# sgdisk -n 1:2048: -t 1:ef00 $DISK_IMG > /dev/null

# dd if=$FAT_IMG of=$DISK_IMG bs=1M seek=1 conv=notrunc status=none

# rm $FAT_IMG

# echo ">> Success! Created GPT Disk Image."
