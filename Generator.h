#ifndef GENERATOR_H
#define GENERATOR_H

#include "Tree.h"
#include "HashTable.h"
#include "Scanner.h"
#include "Lib.h"
#include "Symbol.h"
#include "Asm0.h"

typedef enum { GLOBAL, LOCAL, PARAM, CODE } SCOPE;

typedef struct {                                                                          // 程式碼產生器物件
  HashTable *symTable;                                                                    //   符號表
  Tree *tree;                                                                             //   剖析樹
  FILE *asmFile;                                                                          //   輸出的CPU0組合語言檔
	int forCount, ifCount, whileCount, varCount, labelCount;                                            //   標記與臨時變數的數量
	SCOPE scope;
} Generator;

void generate(Tree *tree, char *asmFile);                                                 // 程式碼產生器的主函數

Generator *GenNew();                                                                      // Generator 的建構函數
void GenFree(Generator *g);                                                               // Generator 的解構函數
void GenCode(Generator *g, Tree *node, char *rzVar);                                     // 產生組合語言程式碼
void GenDecl(Generator *g, Tree *node, SCOPE scope);
void GenPcode(Generator *g, char* label, char* op, char* p1, char* p2, char* pTo);
void GenTempVar(Generator *g, char *tempVar);
void negateOp(char *condOp, char *negOp);
void pcode(FILE *file, char* label, char* op, char* p1, char* p2, char* pTo);
void pcodeNoLabel(FILE *file, char* op, char* p1, char* p2, char* pTo);

#endif
