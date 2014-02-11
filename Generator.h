#ifndef GENERATOR_H
#define GENERATOR_H

#include "Tree.h"
#include "HashTable.h"
#include "Scanner.h"
#include "Lib.h"
#include "SymTable.h"
#include "Parser.h"

// #include "Asm0.h"

// typedef enum { GLOBAL, LOCAL, PARAM, CODE, STRUCT } SCOPE;
typedef enum { CStyle, AsmStyle } PCodeStyle;

typedef struct {                                                                          // 程式碼產生器物件
	SymTable *symTable;                                                                    //   符號表
  Tree *tree;                                                                             //   剖析樹
  FILE *asmFile;                                                                          //   輸出的CPU0組合語言檔
	int forCount, ifCount, whileCount, varCount, labelCount, stringCount;                                            //   標記與臨時變數的數量
	SCOPE scope;
	PCodeStyle pCodeStyle;
	char type[100];
} Generator;

void generate(Tree *tree, SymTable *symTable, char *asmFile); // 程式碼產生器的主函數
Generator *GenNew();                                                                      // Generator 的建構函數
void GenFree(Generator *g);                                                               // Generator 的解構函數
void GenCode(Generator *g, Tree *node, char *rzVar);                                     // 產生組合語言程式碼
void GenData(Generator *g);
void GenDecl(Generator *g, Tree *node, SCOPE scope);
void GenPcode(Generator *g, char* label, char* op, char* p1, char* p2, char* pTo);
void GenTempVar(Generator *g, char *tempVar);
void negateOp(char *condOp, char *negOp);
void pcode(FILE *file, char* label, char* op, char* p1, char* p2, char* pTo, int style);
void pcodeNoLabel(FILE *file, char* op, char* p1, char* p2, char* pTo, int style);
void pcodeCStyle(FILE *file, char* op, char* p1, char* p2, char* pTo);
void pcodeAsmStyle(FILE *file, char* op, char* p1, char* p2, char* pTo);
int typeSize(char *type);

#endif
