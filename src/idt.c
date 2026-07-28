/* idt.c - Sistema Operacional do Zero (S.O.Z.) */
#include "idt.h"

struct idt_entry idt[256];
struct idt_ptr   idtp;

/* Função em Assembly para carregar a IDT no registrador IDTR da CPU */
extern void idt_flush(uint32_t idt_ptr_addr);

/* Stubs de interrupção definidos no interrupts.s */
extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

/* -------------------------------------------------------------
   REMAPEAMENTO DO PIC 8259
   ------------------------------------------------------------- */
void pic_remap(void) {
    /* Envia o comando de inicialização (ICW1) para os dois PICs */
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    /* ICW2: Mapeia IRQ 0..7 para os vetores 32..39 da IDT */
    outb(0x21, 0x20);
    /* ICW2: Mapeia IRQ 8..15 para os vetores 40..47 da IDT */
    outb(0xA1, 0x28);

    /* ICW3: Configura o PIC mestre e escravo em cascata */
    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    /* ICW4: Define o modo 8086 */
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    /* MÁSCARA (OCW1): Restringe/Libera as interrupções.
       0x00 habilita todas as IRQs de hardware. */
    outb(0x21, 0x00);
    outb(0xA1, 0x00);
}

/* -------------------------------------------------------------
   CONFIGURAÇÃO DA IDT
   ------------------------------------------------------------- */
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low  = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel       = sel;
    idt[num].always0   = 0;
    idt[num].flags     = flags;
}

void idt_init(void) {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base  = (uint32_t)&idt;

    /* 1. Limpa todas as 256 entradas da IDT */
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    /* 2. Remapeia as IRQs do PIC para os vetores 32-47 */
    pic_remap();

    /* 3. Mapeia os vetores 32..47 para os stubs de IRQ do Assembly */
    /* 0x08 = Seletor de Código GDT | 0x8E = Interrupt Gate (Ring 0, 32-bit) */
    idt_set_gate(32, (uint32_t)irq0,  0x08, 0x8E);
    idt_set_gate(33, (uint32_t)irq1,  0x08, 0x8E);
    idt_set_gate(34, (uint32_t)irq2,  0x08, 0x8E);
    idt_set_gate(35, (uint32_t)irq3,  0x08, 0x8E);
    idt_set_gate(36, (uint32_t)irq4,  0x08, 0x8E);
    idt_set_gate(37, (uint32_t)irq5,  0x08, 0x8E);
    idt_set_gate(38, (uint32_t)irq6,  0x08, 0x8E);
    idt_set_gate(39, (uint32_t)irq7,  0x08, 0x8E);
    idt_set_gate(40, (uint32_t)irq8,  0x08, 0x8E);
    idt_set_gate(41, (uint32_t)irq9,  0x08, 0x8E);
    idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E);
    idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
    idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);
    idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
    idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E);
    idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);

    /* 4. Carrega o registrador IDTR da CPU */
    idt_flush((uint32_t)&idtp);
}