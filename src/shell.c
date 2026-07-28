/* shell.c - Sistema Operacional do Zero (S.O.Z.) */
#include "shell.h"
#include "screen.h"
#include "keyboard.h"
#include <stdint.h>

#define MAX_ARGS 8
#define BUFFER_SIZE 256

extern uint32_t get_timer_ticks(void);
extern void get_uptime_formatted(char *buffer);
extern void get_rtc_datetime_formatted(char *buffer);

static int cursor_pos = 0;

/* shell.c */

// Permite que o kernel defina onde o cursor/Shell deve começar
void set_cursor_pos(int pos) {
    cursor_pos = pos;
    mover_cursor(cursor_pos);
}

static void itoa(uint32_t n, char *str) {
    int i = 0;
    if (n == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    while (n > 0) {
        str[i++] = (n % 10) + '0';
        n /= 10;
    }
    str[i] = '\0';

    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

static void print_str(const char *str) {
    while (*str) {
        if (*str == '\n') {
            cursor_pos = (cursor_pos / 80 + 1) * 80;
        } else {
            char temp[2] = {*str, '\0'};
            print_string_at(temp, cursor_pos);
            cursor_pos++;
        }

        if (cursor_pos >= 80 * 25) {
            scroll();
            cursor_pos = (25 - 1) * 80;
        }
        str++;
    }
    mover_cursor(cursor_pos);
}

int kstrcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

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
        print_str("Comandos:\n\nAjuda: Mostra comandos disponiveis.\nLimpa: Limpa a tela.\nEcho: Exibe mensagem.\nVer: Mostra a versao.\nData / Relogio: Mostra a data e hora do sistema.\nHora / Uptime: Tempo de atividade em execucao.\nIrqtest: Testa o funcionamento das IRQs.\n\n");
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
        print_str("\nS. O. Z. (Sistema Operacional do Zero) v1.5\nBy Julio Cesar\n\n");
    }
    else if (kstrcmp(argv[0], "data") == 0 || kstrcmp(argv[0], "relogio") == 0 || kstrcmp(argv[0], "date") == 0) {
        char datetime_str[32];
        get_rtc_datetime_formatted(datetime_str);

        print_str("Data e Hora do Sistema: ");
        print_str(datetime_str);
        print_str("\n\n");
    }
    else if (kstrcmp(argv[0], "hora") == 0 || kstrcmp(argv[0], "uptime") == 0) {
        char time_str[16];
        get_uptime_formatted(time_str);

        print_str("Tempo de atividade (Uptime): ");
        print_str(time_str);
        print_str("\n\n");
    }
    else if (kstrcmp(argv[0], "irqtest") == 0) {
        uint32_t t1 = get_timer_ticks();
        
        for (volatile int i = 0; i < 5000000; i++);
        
        uint32_t t2 = get_timer_ticks();

        char buf[16];
        print_str("[STATUS IRQ0 - TIMER]\n");
        print_str("Ticks no momento: ");
        itoa(t2, buf);
        print_str(buf);
        print_str("\n");

        if (t2 > t1) {
            print_str("-> SUCESSO: O PIC e a IRQ0 estao gerando interrupcoes ativas!\n\n");
        } else {
            print_str("-> ERRO: Nenhuma interrupcao detectada. Verifique o sti ou o PIC.\n\n");
        }
    }
    else {
        print_str("Comando invalido: '");
        print_str(argv[0]);
        print_str("'. Digite 'ajuda'.\n");
    }
}

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