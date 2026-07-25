ENTRY(_start)
SECTIONS
{
    . = 0x9000;
    .text : {
        boot.o(.text)   /* Garante que _start de boot.s vem primeiro */ 
        *(.text)
    }
    .rodata : { *(.rodata) }
    .data   : { *(.data)   }
    .bss    : { *(.bss)    }
}
