#ifndef SHELL_H
#define SHELL_H

// Apenas protótipos (sem o corpo { ... } das funções)
void shell_run(void);
void execute_command(char *buffer);
int kstrcmp(const char *s1, const char *s2);

#endif
