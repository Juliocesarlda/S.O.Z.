#include "screen.h"

#define VIDEO_MEMORY ((char*)0xB8000)
#define WHITE_ON_BLACK 0x07

/* Portas de controle do hardware do cursor VGA */
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

#define COLS 80
#define ROWS 25

/* Função interna para escrita em portas de I/O */
static void outb(unsigned short port, unsigned char data) {
    __asm__ __volatile__("outb %1, %0" : : "dN"(port), "a"(data));
}

// Função para rolar a tela uma linha para cima
void scroll() {
    // Copia a linha 1 para a linha 0, a linha 2 para a linha 1, e assim por diante.
    // Cada caractere ocupa 2 bytes (Caractere ASCII + Byte de Atributo/Cor).
    for (int i = 0; i < (ROWS - 1) * COLS * 2; i++) {
        VIDEO_MEMORY[i] = VIDEO_MEMORY[i + COLS * 2];
    }

    // Limpa a última linha (preenche com espaços e atributo padrão)
    int last_line_start = (ROWS - 1) * COLS * 2;
    for (int i = 0; i < COLS; i++) {
        VIDEO_MEMORY[last_line_start + i * 2] = ' ';
        VIDEO_MEMORY[last_line_start + i * 2 + 1] = WHITE_ON_BLACK;
    }
}

/* Limpa a tela e reseta o cursor */
void clear_screen() {
    for (int i = 0; i < 80 * 25; i++) {
        VIDEO_MEMORY[i * 2] = ' ';
        VIDEO_MEMORY[i * 2 + 1] = WHITE_ON_BLACK;
    }
    mover_cursor(0);
}

/* Imprime a string na posição do cursor especificada */
void print_string_at(const char *str, int pos) {
    int i = 0;
    while (str[i] != '\0') {
        VIDEO_MEMORY[(pos + i) * 2] = str[i];
        VIDEO_MEMORY[(pos + i) * 2 + 1] = WHITE_ON_BLACK;
        i++;
    }
}

/* Atualiza a posição piscante do cursor do hardware VGA */
void mover_cursor(int pos) {
    outb(REG_SCREEN_CTRL, 14);
    outb(REG_SCREEN_DATA, (unsigned char)(pos >> 8));
    outb(REG_SCREEN_CTRL, 15);
    outb(REG_SCREEN_DATA, (unsigned char)(pos & 0xFF));
}
