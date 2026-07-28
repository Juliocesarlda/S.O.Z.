# Inicialização do Sistema (boot)  

Esta pasta contém o código em **Assembly x86** responsável pela sequência de boot e pela transição do hardware para a execução do kernel.

---

## Componentes de Boot

* **`bootloader.s`**: O carregador de inicialização principal de 512 bytes (com a assinatura de boot `0xAA55`). Responsável por ser lido pela BIOS e carregar o kernel na memória RAM.
* **`boot.s`**: Código de inicialização e transição que configura os registradores do processador, o ponteiro de pilha (*stack pointer*) e passa o controle para o ponto de entrada (*entrypoint*) do kernel em C.
