#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "Tree.h"
#include "SymTable.h"

typedef struct { // �y�N���R�����󵲺c
    Tree *tree;
    SymTable *symTable;
	char type[100]; // , pathType[100];
	Array *blockStack; // �ܼ� id ���϶��d��A���O PROG, STRUCT, METHOD, BLOCK ���C 
//	BOOL pathFirstAtom;
//	Tree *pathOp;
//    , *pathBlock;
//	char structName[100];
//	char methodName[100];
//	SCOPE scope;    
} SA;

void semanticAnalysis(Tree *tree, SymTable *symTable);

// ========================= Semantic Analysis ==============================

typedef struct { } SemProg;
typedef struct {  } SemBase;
typedef struct { Tree *exp, *base1, *base2; } SemIf;
typedef struct { Tree *stmt1, *exp, *stmt2; } SemFor;
typedef struct { Tree *exp, *base; } SemWhile;
typedef struct { Tree *type, *id, *paramList, *block; Method *method; } SemMethod; // /*int starCount; char *returnType;
typedef struct { Tree *id; Struct *stru; } SemStruct;
typedef struct { Tree *id, *exp; Var *var; } SemVar; // int starCount, dimList[10], dimCount;
typedef struct { Tree *exp, *decl, *path, *op1, *expList; } SemStmt;
typedef struct { Tree *type, *varList; } SemDecl;
typedef struct { Tree *type, *var; } SemParam;
typedef struct { Tree *baseList; } SemBlock;
typedef struct { Tree *term1, *op, *term2; } SemExp;
typedef struct { Tree *exp1, *op, *exp2, *path; } SemTerm; // int int1; float float1; char *str1;
typedef struct { } SemPath;
typedef struct { Tree *id, *expList; Array *exps; char *subTag; } SemAtom;
typedef struct { Array *varList; } SemVarList;
typedef struct { Array *expList; } SemExpList;
typedef struct { Array *declList; } SemDeclList;
typedef struct { Array *paramList; } SemParamList;
typedef struct { } SemType;
typedef struct { char *token; } SemToken; // 
// typedef struct { } SemId;

// SemStruct* StructNew();
// void StructFree();
// Var* VarListFind(Array *vars, char *name);

// ============= sa:semantic analysis ================
SA *SaNew();
void SaFree(SA *sa);
void SaTree(SA *sa, Tree *node);
char *token(Tree *node);
SemToken *SemTokenNew(char *token);
char *SaAtomType(SA *sa, Tree *cAtom, char *type);

#endif
