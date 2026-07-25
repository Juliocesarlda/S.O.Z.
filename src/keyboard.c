#include "keyboard.h"

/* Portas de I/O do teclado */
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

/* Tabela de scancodes (US Layout) */
static const char scancode_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,  'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,  '\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0,  ' '
};

/* Leitura da porta de I/O */
unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ __volatile__("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

/* Verifica se há dados na porta do teclado */
int keyboard_hit() {
    return inb(KEYBOARD_STATUS_PORT) & 1;
}

/* Lê uma tecla (bloqueia até obter um caractere ASCII válido) */
char keyboard_getchar() {
    char key = 0;

    while (key == 0) {
        while (!keyboard_hit());

        unsigned char scancode = inb(KEYBOARD_DATA_PORT);

        // Se for break code (tecla solta > 0x80), ignora e continua no loop
        if (scancode & 0x80) {
            continue;
        }

        if (scancode < 128) {
            key = scancode_ascii[scancode];
        }
    }

    return key;
}

/* Versão não-bloqueante */
char keyboard_read() {
    if (!keyboard_hit())
        return 0;

    unsigned char scancode = inb(KEYBOARD_DATA_PORT);

    if (scancode & 0x80)
        return 0;

    return scancode_ascii[scancode];
}
