; interrupts.s (S.O.Z.)
[bits 32]

global idt_flush
idt_flush:
    mov eax, [esp + 4]
    lidt [eax]
    ret

; Declaração do manipulador C
extern irq_handler

; Macro para criar stubs de IRQs rapidamente
%macro IRQ 2
global irq%1
irq%1:
    cli
    push byte 0    ; Código de erro dummy para alinhar a pilha
    push byte %2   ; Número da IRQ (32..47)
    jmp irq_common_stub
%endmacro

; Define IRQ0 a IRQ15 (Mapeadas para 32..47)
IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

; Stub comum para todas as IRQs
irq_common_stub:
    pusha           ; Salva EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI

    mov ax, ds
    push eax        ; Salva o segmento de dados atual

    mov ax, 0x10    ; Carrega o segmento de dados do kernel
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call irq_handler ; Chama a função C

    pop eax         ; Restaura o segmento de dados original
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa            ; Restaura registradores gerais
    add esp, 8      ; Limpa o número da interrupção e o código de erro da pilha
    sti             ; Reabilita interrupções
    iretd           ; Retorna da interrupção
