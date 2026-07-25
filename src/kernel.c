#include "shell.h"
#include "screen.h"

// Ponto de entrada chamado pelo boot.s
void kernel_main() {
    // Desativa interrupções
    __asm__ volatile("cli");

    // Prepara a tela inicial
    clear_screen();
    print_string_at("S. O. Z. (Sistema Operacional do Zero) v1.5 - Kernel Ativo", 0);
	
    // Passa o controle para o Shell
    shell_run();

    // Trava de segurança
    while(1) {
        __asm__ volatile("hlt");
    }
}
