#!/bin/bash 

echo "========================================="
echo "   LIMPANDO ARQUIVOS VELHOS DO SISTEMA   "
echo "========================================="
rm -f bootloader.bin boot.o kernel.o shell.o screen.o keyboard.o kernel.bin jcos.img julio_cesar_novo.img

echo "========================================="
echo "        COMPILANDO O JULIO CESAR OS      "
echo "========================================="

# 0. Verifica suporte a 32-bits (Corrigido com a flag -c)
if ! gcc -m32 -c -x c /dev/null -o /dev/null 2>/dev/null; then
    echo "ERRO: gcc-multilib nao instalado!"
    echo "Execute: sudo apt install gcc-multilib"
    exit 1
fi

# 1. Bootloader 16-bits (NASM)
nasm -f bin bootloader.s -o bootloader.bin
if [ $? -ne 0 ]; then echo "Erro no Bootloader!"; exit 1; fi
echo "bootloader.bin: $(wc -c < bootloader.bin) bytes"

# 2. Ponto de entrada do kernel (Assembly 32-bits via GCC)
gcc -m32 -c boot.s -o boot.o
if [ $? -ne 0 ]; then echo "Erro no boot.s!"; exit 1; fi

# 3. Kernel e Módulos em C (32-bits)
FLAGS="-m32 -c -ffreestanding -O2 -fno-pie -fno-stack-protector -fno-builtin -fno-common -Wall -Wextra"

echo "Compilando kernel.c..."
gcc $FLAGS kernel.c -o kernel.o
if [ $? -ne 0 ]; then echo "Erro no kernel.c!"; exit 1; fi

echo "Compilando shell.c..."
gcc $FLAGS shell.c -o shell.o
if [ $? -ne 0 ]; then echo "Erro no shell.c!"; exit 1; fi

echo "Compilando screen.c..."
gcc $FLAGS screen.c -o screen.o
if [ $? -ne 0 ]; then echo "Erro no screen.c!"; exit 1; fi

echo "Compilando keyboard.c..."
gcc $FLAGS keyboard.c -o keyboard.o
if [ $? -ne 0 ]; then echo "Erro no keyboard.c!"; exit 1; fi

# 4. Linka em binário PURO (sem header ELF)
ld -m elf_i386 -T linker.ld boot.o kernel.o shell.o screen.o keyboard.o -o kernel.bin --oformat binary
if [ $? -ne 0 ]; then echo "Erro na Linkagem!"; exit 1; fi
echo "kernel.bin: $(wc -c < kernel.bin) bytes"

echo "========================================="
echo "        GRAVANDO NO NOVO DISQUETE        "
echo "========================================="

cat bootloader.bin kernel.bin > jcos.img
truncate -s 1440k jcos.img
echo "jcos.img: $(wc -c < jcos.img) bytes"

# Verifica assinatura MBR (55aa)
if command -v xxd >/dev/null 2>&1; then
    ASSINATURA=$(xxd -s 510 -l 2 bootloader.bin | awk '{print $2$3}')
    echo "Assinatura MBR: $ASSINATURA (deve ser 55aa)"
fi

echo "========================================="
echo "      SUCESSO ABSOLUTO! PRONTO PRO VB    "
echo "========================================="
