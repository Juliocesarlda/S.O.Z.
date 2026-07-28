/* kernel.c - Sistema Operacional do Zero (S.O.Z.) */
#include "shell.h"
#include "screen.h"
#include "idt.h"
#include "irq.h"

extern void set_cursor_pos(int pos);

static volatile uint32_t timer_ticks = 0;

void timer_callback(registers_t regs) {
    (void)regs;
    timer_ticks++;
}

uint32_t get_timer_ticks(void) {
    return timer_ticks;
}

void get_uptime_formatted(char *buffer) {
    uint32_t ticks = get_timer_ticks();
    uint32_t total_seconds = ticks / 18;

    uint32_t hours = total_seconds / 3600;
    uint32_t minutes = (total_seconds % 3600) / 60;
    uint32_t seconds = total_seconds % 60;

    buffer[0] = '0' + (hours / 10);
    buffer[1] = '0' + (hours % 10);
    buffer[2] = ':';
    buffer[3] = '0' + (minutes / 10);
    buffer[4] = '0' + (minutes % 10);
    buffer[5] = ':';
    buffer[6] = '0' + (seconds / 10);
    buffer[7] = '0' + (seconds % 10);
    buffer[8] = '\0';
}

static uint8_t cmos_read(uint8_t reg) {
    outb(0x70, reg);
    return inb(0x71);
}

static uint8_t bcd_to_bin(uint8_t val) {
    return ((val / 16) * 10) + (val % 16);
}

void get_rtc_datetime_formatted(char *buffer) {
    while (cmos_read(0x0A) & 0x80);

    uint8_t second = bcd_to_bin(cmos_read(0x00));
    uint8_t minute = bcd_to_bin(cmos_read(0x02));
    uint8_t hour   = bcd_to_bin(cmos_read(0x04));
    uint8_t day    = bcd_to_bin(cmos_read(0x07));
    uint8_t month  = bcd_to_bin(cmos_read(0x08));
    uint8_t year   = bcd_to_bin(cmos_read(0x09));

    buffer[0]  = '0' + (day / 10);
    buffer[1]  = '0' + (day % 10);
    buffer[2]  = '/';
    buffer[3]  = '0' + (month / 10);
    buffer[4]  = '0' + (month % 10);
    buffer[5]  = '/';
    buffer[6]  = '2';
    buffer[7]  = '0';
    buffer[8]  = '0' + (year / 10);
    buffer[9]  = '0' + (year % 10);

    buffer[10] = ' ';
    buffer[11] = '-';
    buffer[12] = ' ';

    buffer[13] = '0' + (hour / 10);
    buffer[14] = '0' + (hour % 10);
    buffer[15] = ':';
    buffer[16] = '0' + (minute / 10);
    buffer[17] = '0' + (minute % 10);
    buffer[18] = ':';
    buffer[19] = '0' + (second / 10);
    buffer[20] = '0' + (second % 10);
    buffer[21] = '\0';
}

static void delay_ticks(uint32_t count) {
    uint32_t start = timer_ticks;
    while ((timer_ticks - start) < count);
}

static void show_boot_banner(void) {
    clear_screen();

    // Arte ASCII do S.O.Z.
    print_string_at("   _____  ____  ______ ", 0);
    print_string_at("  / ____|/ __ \\|___  / ", 80);
    print_string_at(" | (___ | |  | |  / /  ", 160);
    print_string_at("  \\___ \\| |  | | / /   ", 240);
    print_string_at("  ____) | |__| |/ /__  ", 320);
    print_string_at(" |_____/ \\____//_____| ", 400);

    print_string_at("================================================================================", 560);
    print_string_at("S.O.Z. Kernel v1.5 (i386-baremetal)", 640);
    print_string_at("Direitos Reservados (C) 2026 Julio Cesar.", 720);
    print_string_at("================================================================================", 800);

    // Mensagens de inicialização de hardware em Português
    delay_ticks(5);
    print_string_at("[   0.000000] Relogio de Tempo Real (CMOS/RTC) inicializado.", 960);
    
    delay_ticks(5);
    print_string_at("[   0.018220] Temporizador de Intervalo (PIT) ativo a 18.22Hz.", 1040);
    
    delay_ticks(5);
    print_string_at("[   0.036440] PIC Duplo 8259 remapeado para os vetores 0x20-0x2F.", 1120);
    
    delay_ticks(5);
    print_string_at("[   0.054660] Tabela IDT carregada com 256 portas de interrupcao.", 1200);

    delay_ticks(5);
    print_string_at("[   0.072880] Console: Modo texto VGA 80x25 pronto.", 1280);

    delay_ticks(8);
    print_string_at("Iniciando o Shell Interativo do Sistema (sh)...", 1440);
    print_string_at("--------------------------------------------------------------------------------", 1520);

    // Reposiciona o cursor do Shell para a linha 20 (1600 offset)
    set_cursor_pos(1600);
}

void kernel_main() {
    __asm__ volatile("cli");

    idt_init();
    register_interrupt_handler(32, timer_callback);

    __asm__ volatile("sti");

    show_boot_banner();

    shell_run();

    while(1) {
        __asm__ volatile("hlt");
    }
}