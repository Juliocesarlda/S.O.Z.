[org 0x7c00]
KERNEL_OFFSET equ 0x9000

    ; 1. Salva a unidade de boot passada pela BIOS no registrador DL
    mov [BOOT_DRIVE], dl

    ; 2. Zera os registradores de segmento (16-bits)
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; 3. Carrega o kernel do disco para a RAM
    call carregar_kernel

    ; 4. Alterna para Modo Protegido de 32-bits
    call alternar_modo_protegido

    jmp $

BOOT_DRIVE db 0

carregar_kernel:
    mov bx, KERNEL_OFFSET    ; Endereço de destino na RAM (0x9000)
    mov dh, 0                ; Cabeça 0
    mov ch, 0                ; Cilindro 0
    mov cl, 2                ; Começa no Setor 2 (Setor 1 é o bootloader)
    mov al, 20               ; Lê 20 setores (suficiente para todo o kernel)
    mov dl, [BOOT_DRIVE]     ; Unidade de disco correta

    mov ah, 0x02             ; Função BIOS de leitura
    int 0x13                 ; Chama a BIOS
    jc erro_disco            ; Se o sinal de erro (Carry Flag) for ativado, vai para o erro
    ret

erro_disco:
    ; Se falhar a leitura, imprime 'E' em amarelo na tela para nos avisar
    mov ah, 0x0e
    mov al, 'E'
    int 0x10
    cli
    hlt

[bits 16]
alternar_modo_protegido:
    cli                       ; Desativa interrupções
    lgdt [gdt_descriptor]     ; Carrega a tabela GDT
    mov eax, cr0
    or eax, 0x1               ; Liga o bit de Modo Protegido no CR0
    mov cr0, eax
    jmp 0x08:iniciar_32bit    ; Salto distante (Far Jump) para limpar o pipeline da CPU

[bits 32]
iniciar_32bit:
    ; Configura os registradores de segmento para 32-bits
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Configura o topo da Pilha na memória
    mov ebp, 0x90000
    mov esp, ebp

    ; Salta para o endereço do Kernel (0x9000) onde está o boot.s!
    jmp KERNEL_OFFSET

; --- Tabela GDT (Global Descriptor Table) ---
gdt_start:
gdt_null:
    dd 0x0
    dd 0x0

gdt_code:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0

gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

times 510-($-$$) db 0
dw 0xaa55
