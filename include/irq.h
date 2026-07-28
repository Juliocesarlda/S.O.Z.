#ifndef ISR_H
#define ISR_H

#include <stdint.h>

typedef struct {
    uint32_t ds;                                     /* Segmento de dados */
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; /* Empilhados por pusha */
    uint32_t int_no, err_code;                       /* Número da IRQ e código de erro */
    uint32_t eip, cs, eflags, useresp, ss;           /* Empilhados automaticamente pela CPU */
} registers_t;

typedef void (*irq_t)(registers_t);
void register_interrupt_handler(uint8_t n, irq_t handler);

#endif