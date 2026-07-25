.intel_syntax noprefix

.global _start
.extern kernel_main

.section .text
.align 4

_start:
    cli                     # Desativa interrupções
    mov ax, 0x10            # 0x10 é o seletor de dados da GDT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    # CORREÇÃO: Usa 0x90000 (576 KB) que é memória baixa segura
    mov esp, 0x90000
    and esp, 0xFFFFFFF0     # Alinha a pilha em 16 bytes (exigência da ABI x86)

    call kernel_main        # Entra no Kernel em C!

hang:
    hlt                     # Trava a CPU caso o kernel_main retorne
    jmp hang
