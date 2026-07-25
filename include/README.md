#  Arquivos de Cabeçalho (Headers)

Esta pasta reúne as definições de interfaces, protótipos de funções e constantes do **S.O.Z.**. Os arquivos `.h` garantem a modularidade do kernel e a comunicação entre os drivers e o sistema.

---

##  Módulos e Interfaces

* **`keyboard.h`**: Declarações para captura e tratamento de interrupções do teclado, mapeamento de *scancodes* e manipulação de buffers de entrada.
* **`screen.h`**: Definições de constantes do buffer de vídeo VGA (`0xB8000`), cores, cursores e protótipos de funções para renderização na tela.
* **`shell.h`**: Interfaces do interpretador de comandos, declaração das rotinas de parsing e assinaturas dos comandos internos do sistema.
