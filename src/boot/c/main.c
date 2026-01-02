#include <efi.h>
#include <efilib.h>

#include "elf.h"

#define kernel_load_address 0x100000

typedef struct __attribute__((packed)) {
    // Memory Map Info
    EFI_MEMORY_DESCRIPTOR* mMap;
    UINTN mMapSize;
    UINTN mMapDescriptorSize;

    // Graphics Info (GOP)
    void* FrameBufferBase;
    UINT64 FrameBufferSize;
    UINT32 ScreenWidth;
    UINT32 ScreenHeight;
    UINT32 PixelsPerScanLine;
    UINT8 PixelFormat;
    
    EFI_RUNTIME_SERVICES* RuntimeServices;
} BootInfo;

typedef void (*KernelEntryFunc)(void* BootInfo);

void JumpToKernel(Elf64_Addr EntryPoint, BootInfo* BootInfo) {
    KernelEntryFunc k_entry = (KernelEntryFunc)EntryPoint;

    __asm__ __volatile__ (
        "cli \n\t"
        "movq %0, %%rcx \n\t"
        "movq %1, %%rdi \n\t"
        "jmp *%%rcx \n\t"
        : 
        : "r"(EntryPoint), "r"(BootInfo)
        : "rcx", "rdi", "memory"
    );
}

EFI_STATUS EFIAPI efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);
    
    Print(L"Hello World from GNU-EFI!\n");
    EFI_STATUS status;

    // open root directory
    EFI_LOADED_IMAGE *loadedImage;
    status = uefi_call_wrapper(SystemTable->BootServices->HandleProtocol, 3, ImageHandle, &gEfiLoadedImageProtocolGuid, (void**)&loadedImage);
    if (EFI_ERROR(status)) {
        Print(L"Failed to get Loaded Image Protocol\n");
        return status;
    }
    EFI_FILE_HANDLE root_dir = LibOpenRoot(loadedImage->DeviceHandle);

    // find kernel.elf
    EFI_FILE_HANDLE kernel_file;
    status = uefi_call_wrapper(root_dir->Open, 5, root_dir, &kernel_file, L"kernel.elf", EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(status)) {
        Print(L"Failed to open kernel.elf\n");
        return status;
    }

    // parse elf file header
    Elf64_Ehdr* elf_header;
    status = uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3, EfiLoaderData, sizeof(Elf64_Ehdr), (void**)&elf_header);
    if (EFI_ERROR(status)) {
        Print(L"Failed to allocate pool for ELF header\n");
        return status;
    }
    status = uefi_call_wrapper(kernel_file->Read, 3, kernel_file, (UINTN[]){sizeof(Elf64_Ehdr)}, elf_header);
    if (EFI_ERROR(status)) {
        Print(L"Failed to read ELF header\n");
        return status;
    }

    // calculate kernel size and allocate memory
    UINT64 kernel_size = elf_header->e_shoff + (elf_header->e_shnum * elf_header->e_shentsize);
    Print(L"Loading kernel of size %d bytes, entry=%d\n", kernel_size, elf_header->e_entry);

    // read entire kernel into memory
    Elf64_Phdr* PHdrs = AllocatePool(elf_header->e_phnum * elf_header->e_phentsize);
    uefi_call_wrapper(kernel_file->SetPosition, 2, kernel_file, elf_header->e_phoff);
    UINTN PhdrSize = elf_header->e_phnum * elf_header->e_phentsize;
    uefi_call_wrapper(kernel_file->Read, 3, kernel_file, &PhdrSize, PHdrs);

    for (int i = 0; i < elf_header->e_phnum; i++) {
        Elf64_Phdr* ph = &PHdrs[i];

        if (ph->p_type == PT_LOAD) {
            Print(L"Segment %d: FileOffset=0x%lx VAddr=0x%lx FileSize=0x%lx\n", 
                i, ph->p_offset, ph->p_vaddr, ph->p_filesz);

            EFI_PHYSICAL_ADDRESS AllocAddr = ph->p_paddr;
            UINTN Pages = (ph->p_memsz + 0xFFF) / 0x1000;

            if (Pages > 0) {
                status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAddress, EfiLoaderData, Pages, &AllocAddr);
                if (EFI_ERROR(status)) {
                    Print(L"ERROR: Failed to allocate page at 0x%lx (Status: %r)\n", AllocAddr, status);
                    return status;
                }

                status = uefi_call_wrapper(kernel_file->SetPosition, 2, kernel_file, ph->p_offset);
                
                UINTN Size = ph->p_filesz;
                status = uefi_call_wrapper(kernel_file->Read, 3, kernel_file, &Size, (void*)AllocAddr);
                
                if (ph->p_memsz > ph->p_filesz) {
                SetMem((void*)(AllocAddr + ph->p_filesz), ph->p_memsz - ph->p_filesz, 0);
                }
            }
        }
    }

    // create bootinfo structure
    BootInfo boot_info;
    status = uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3, EfiLoaderData, sizeof(BootInfo), (void**)&boot_info);
    if (EFI_ERROR(status)) {
        Print(L"Failed to allocate pool for BootInfo\n");
        return status;
    }
    boot_info.RuntimeServices = SystemTable->RuntimeServices;

    // get graphics info (GOP)
    EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
    EFI_STATUS gop_status = uefi_call_wrapper(SystemTable->BootServices->LocateProtocol, 3, &gop_guid, NULL, (void**)&gop);
    if (EFI_ERROR(gop_status)) {
        Print(L"Failed to locate GOP protocol\n");
        return gop_status;
    }

    boot_info.FrameBufferBase = (void*) gop->Mode->FrameBufferBase;
    boot_info.FrameBufferSize = gop->Mode->FrameBufferSize;
    boot_info.ScreenWidth = gop->Mode->Info->HorizontalResolution;
    boot_info.ScreenHeight = gop->Mode->Info->VerticalResolution;
    boot_info.PixelsPerScanLine = gop->Mode->Info->PixelsPerScanLine;
    boot_info.PixelFormat = gop->Mode->Info->PixelFormat;

    // exit boot services
    UINTN map_key;
    UINT32 descriptor_version;
    status = uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5, &boot_info.mMapSize, boot_info.mMap, &map_key, &boot_info.mMapDescriptorSize, &descriptor_version);
    if (status == EFI_BUFFER_TOO_SMALL) {
        status = uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3, EfiLoaderData,  boot_info.mMapSize, (void**)&boot_info.mMap);
        if (EFI_ERROR(status)) {
            Print(L"Failed to allocate pool for memory map\n");
            return status;
        }
        status = uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5, &boot_info.mMapSize, boot_info.mMap, &map_key, &boot_info.mMapDescriptorSize, &descriptor_version);
        if (EFI_ERROR(status)) {
            Print(L"Failed to get memory map\n");
            return status;
        }
    }

    status = uefi_call_wrapper(SystemTable->BootServices->ExitBootServices, 2, ImageHandle, map_key);
    if (EFI_ERROR(status)) {
        Print(L"Failed to exit boot services\n");
        return status;
    }

    // jump to kernel entry point
    JumpToKernel(elf_header->e_entry, &boot_info);
    
    return EFI_SUCCESS;
}