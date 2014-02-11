#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "Parser.h"

typedef struct {                                                                          // 程式碼產生器物件
	HashTable *symTable;                                                                    //   符號表
	Tree *tree;                                                                             //   剖析樹
} Interpreter;

Interpreter *InterNew();
void InterFree(Interpreter *o);
void InterRun(Interpreter *o, Tree *node, char *rzVar);
void typing(Tree *node);

#endif
