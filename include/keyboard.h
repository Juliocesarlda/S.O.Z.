#ifndef KEYBOARD_H
#define KEYBOARD_H

/* Lê uma tecla (bloqueia até pressionar) */
char keyboard_getchar();

/* Lê uma tecla (não bloqueante, retorna 0 se nada) */
char keyboard_read();

/* Verifica se há tecla disponível */
int keyboard_hit();

/* Função interna de I/O (pode ser usada pelo kernel também) */
unsigned char inb(unsigned short port);

#endif
