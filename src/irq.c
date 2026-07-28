#include "idt.h"
#include "irq.h"

irq_t interrupt_handlers[256];

void register_interrupt_handler(uint8_t n, irq_t handler) {
    interrupt_handlers[n] = handler;
}

void irq_handler(registers_t regs) {
    /* Send EOI (End of Interrupt) signal to PICs */
    if (regs.int_no >= 40) {
        /* Se a IRQ veio do PIC escravo (IRQ 8 a 15), envia EOI para o escravo */
        outb(0xA0, 0x20);
    }
    /* Envia EOI para o PIC mestre */
    outb(0x20, 0x20);

    /* Executa o handler customizado, se existir */
    if (interrupt_handlers[regs.int_no] != 0) {
        irq_t handler = interrupt_handlers[regs.int_no];
        handler(regs);
    }
}