#ifndef SYMBOL_H
#define SYMBOL_H
#include "Lib.h"

typedef struct {                                                                          // �Ÿ��O��
	char *name, *type;
} Symbol;

Symbol *SymNew(char *name, char *type);
void SymFree(Symbol *s);

#endif
