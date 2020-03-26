       

SECTIONS
{
    . = 0x80000;
    img_start = .;
    init : {
        CMakeFiles/kernel.img.dir/boot/start.S.o
     CMakeFiles/kernel.img.dir/boot/mmu.c.o
     CMakeFiles/kernel.img.dir/boot/tools.S.o
     CMakeFiles/kernel.img.dir/boot/init_c.c.o
     CMakeFiles/kernel.img.dir/boot/print/macro.S.o
     CMakeFiles/kernel.img.dir/boot/print/uart.c.o
     CMakeFiles/kernel.img.dir/boot/print/printf.c.o
    }

    . = ALIGN(0x00004000);

    init_end = ABSOLUTE(.);

    .text 0xffffff0000000000 + init_end : AT(init_end) {
        *(.text*)
        . = ALIGN(0x00000010);
    }

    . = ALIGN(0x00010000);
    .data : {
        *(.data*)
        . = ALIGN(0x00000010);
    }
    . = ALIGN(0x00010000);

    .rodata : {
        *(.rodata*)
        . = ALIGN(0x00000010);
    }
    _edata = . - 0xffffff0000000000;

    _bss_start = . - 0xffffff0000000000;
    .bss : {
        *(.bss*)
        . = ALIGN(0x00000010);
    }
    _bss_end = . - 0xffffff0000000000;
    . = ALIGN(0x00010000);
    img_end = . - 0xffffff0000000000;
}
