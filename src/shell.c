#include "shell.h"
#include "screen.h"
#include "keyboard.h"

#define MAX_ARGS 8
#define BUFFER_SIZE 256

static int cursor_pos = 0;

// Imprime a string e faz o scroll automático quando necessário
static void print_str(const char *str) {
    while (*str) {
        if (*str == '\n') {
            // Pula para o início da próxima linha
            cursor_pos = (cursor_pos / 80 + 1) * 80;
        } else {
            // Desenha o caractere na tela
            char temp[2] = {*str, '\0'};
            print_string_at(temp, cursor_pos);
            cursor_pos++;
        }

        // --- LÓGICA DE SCROLL ---
        // Se o cursor ultrapassou o fim da tela (80 * 25 = 2000)
        if (cursor_pos >= 80 * 25) {
            scroll();
            // Move o cursor para o início da última linha (linha 24 -> índice 1920)
            cursor_pos = (25 - 1) * 80;
        }

        str++;
    }
    mover_cursor(cursor_pos);
}

// Comparação de Strings Bare-Metal
int kstrcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

// Interpretador de Comandos
void execute_command(char *buffer) {
    char *argv[MAX_ARGS];
    int argc = 0;
    char *ptr = buffer;

    while (*ptr != '\0') {
        while (*ptr == ' ') {
            *ptr = '\0';
            ptr++;
        }

        if (*ptr == '\0') break;
        if (argc >= MAX_ARGS) break;

        argv[argc++] = ptr;

        while (*ptr != ' ' && *ptr != '\0') {
            ptr++;
        }
    }

    if (argc == 0) return;

    if (kstrcmp(argv[0], "ajuda") == 0) {
        print_str("Comandos:\n\nAjuda: Mostra quais os comandos disponiveis.\nLimpa: Limpa a tela do shel.\nEcho: exibe mensagem do usuario na tela.\nVer: Mostra a versao do Sistema Operacional.\n\n");
    } 
    else if (kstrcmp(argv[0], "limpa") == 0) {
        clear_screen();
        cursor_pos = 0;
        mover_cursor(cursor_pos);
    } 
    else if (kstrcmp(argv[0], "echo") == 0) {
        for (int i = 1; i < argc; i++) {
            print_str(argv[i]);
            if (i < argc - 1) print_str(" ");
        }
        print_str("\n");
    }
    else if (kstrcmp(argv[0], "ver") == 0) {
        print_str("\n");
		print_str("S. O. Z. (Sistema Operacional do Zero) v1.5\n");
        print_str("By Julio Cesar\n");
        print_str("\n");
    }
    else {
        print_str("Comando invalido: '");
        print_str(argv[0]);
        print_str("'. Digite 'ajuda'.\n");
    }
}

// Loop Principal do Shell
void shell_run(void) {
    char input_buffer[BUFFER_SIZE];
    int buffer_idx = 0;

    print_str("\nSOZ> ");

    while (1) {
        char c = keyboard_getchar();

        if (c == '\r' || c == '\n') {
            print_str("\n");
            input_buffer[buffer_idx] = '\0';

            execute_command(input_buffer);

            buffer_idx = 0;
            print_str("SOZ> ");
        } 
        else if (c == '\b' || c == 0x7F) {
            if (buffer_idx > 0) {
                buffer_idx--;
                if (cursor_pos > 0) cursor_pos--;
                print_string_at(" ", cursor_pos);
                mover_cursor(cursor_pos);
            }
        } 
        else {
            if (buffer_idx < BUFFER_SIZE - 1) {
                input_buffer[buffer_idx++] = c;
                
                char str[2] = {c, '\0'};
                print_str(str);
            }
        }
    }
}
