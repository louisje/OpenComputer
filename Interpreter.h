#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "Parser.h"

typedef struct {                                                                          // �{���X���;�����
	HashTable *symTable;                                                                    //   �Ÿ���
	Tree *tree;                                                                             //   ��R��
} Interpreter;

Interpreter *InterNew();
void InterFree(Interpreter *o);
void InterRun(Interpreter *o, Tree *node, char *rzVar);
void typing(Tree *node);

#endif
