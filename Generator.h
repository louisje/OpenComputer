#ifndef GENERATOR_H
#define GENERATOR_H

#include "Tree.h"
#include "HashTable.h"
#include "Scanner.h"
#include "Lib.h"
#include "Symbol.h"
#include "Asm0.h"

typedef enum { GLOBAL, LOCAL, PARAM, CODE } SCOPE;

typedef struct {                                                                          // �{���X���;�����
  HashTable *symTable;                                                                    //   �Ÿ���
  Tree *tree;                                                                             //   ��R��
  FILE *asmFile;                                                                          //   ��X��CPU0�զX�y����
	int forCount, ifCount, whileCount, varCount, labelCount;                                            //   �аO�P�{���ܼƪ��ƶq
	SCOPE scope;
} Generator;

void generate(Tree *tree, char *asmFile);                                                 // �{���X���;����D���

Generator *GenNew();                                                                      // Generator ���غc���
void GenFree(Generator *g);                                                               // Generator ���Ѻc���
void GenCode(Generator *g, Tree *node, char *rzVar);                                     // ���ͲզX�y���{���X
void GenDecl(Generator *g, Tree *node, SCOPE scope);
void GenPcode(Generator *g, char* label, char* op, char* p1, char* p2, char* pTo);
void GenTempVar(Generator *g, char *tempVar);
void negateOp(char *condOp, char *negOp);
void pcode(FILE *file, char* label, char* op, char* p1, char* p2, char* pTo);
void pcodeNoLabel(FILE *file, char* op, char* p1, char* p2, char* pTo);

#endif
