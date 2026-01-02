ENTRY(_start)

SECTIONS
{
    /* We want to be loaded at 1MB physical */
    . = 0x100000;

    .text : {
        *(.text)
    }

    . = ALIGN(0x1000);

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }
}