// PROG = (STRUCT | METHOD | DECL ; )*
// METHOD = TYPE ** ID(PARAM_LIST?) BLOCK
// STRUCT = struct ID { DECL_LIST ; }
// BLOCK = { BASE* }
// BASE = IF | FOR | WHILE | BLOCK | STMT ;
// IF = if (EXP) BASE (else BASE)?
// FOR = for (STMT ; EXP ; STMT) BASE
// WHILE = while (EXP) BASE
// STMT = return EXP | DECL | PATH (EXP_LIST) | PATH = EXP | PATH OP1
// VAR = ** ID ([ integer ])* (= EXP)?
// EXP = TERM (OP2 TERM)?
// TERM = ( EXP (OP2 EXP)? ) | CINT | CFLOAT | CSTR | PATH
// PATH = ATOM ((.|->) ATOM)*
// ATOM = ID (([ EXP ])* |( EXP_LIST? ))
// DECL = TYPE VAR_LIST
// PARAM = TYPE VAR
// VAR_LIST = VAR (, VAR)*
// EXP_LIST = EXP (, EXP)*
// DECL_LIST = DECL (; DECL)*
// PARAM_LIST = PARAM (, PARAM)*
// TYPE = (byte | char | int | float | ID) // 最後一個 ID 必須是 TYPE [STRUCT]
// ID = [A-Za-z_][0-9A-Za-z_]*
// CINT = [0-9]+
// CFLOAT = [0-9]+.[0-9]+
// CSTR = ".*"
// OP2 = +|-|/|*|%|&|&&|^|<<|>>|<|>|<=|>=|==|!=|  與 | , ||
// OP1 = ++ | --

#ifndef PARSER_H
#define PARSER_H

#include "Scanner.h"
#include "Tree.h"
#include "Lib.h"
#include "Semantic.h"

typedef enum { SCOPE_GLOBAL, SCOPE_METHOD_NAME, SCOPE_PARAM, 
SCOPE_LOCAL, SCOPE_CODE, SCOPE_STRUCT_NAME, SCOPE_STRUCT, SCOPE_DECL 
} SCOPE;

typedef struct {                           // 剖析器的物件結構      
  Tree *tree;                              //   剖析樹 (樹根)    
	Array *stack;          // 剖析過程用的 node 堆疊 
	Array *blockStack;     // 區塊堆疊，變數 id 的區塊範圍，像是 PROG, STRUCT, METHOD, BLOCK 等。
	Block *block;
	char  spaces[MAX_LEN];
	Var   decl;
	Scanner *scanner;      // 詞彙掃描器 (Lexical Analysis)
	SymTable *symTable;
} Parser;                                                     
                                                              
Tree *parse(char *text);// 剖析器的主程式     
Parser *ParserNew(Scanner *scanner); // 剖析器的建構函數   
void ParserParse(Parser *p, char *text);   // 剖析器的剖析函數   
void ParserFree(Parser *parser);           // 釋放記憶體         

Tree* parseProg(Parser *p);
Tree* parseBase(Parser *p);
Tree* parseIf(Parser *p);
Tree* parseFor(Parser *p);
Tree* parseWhile(Parser *p);
Tree* parseMethod(Parser *p);
Tree* parseStmt(Parser *p);
Tree* parseDecl(Parser *p);
Tree* parseBlock(Parser *p);
Tree* parseVar(Parser *p);
Tree* parseExp(Parser *p);
Tree* parseTerm(Parser *p);
Tree* parsePath(Parser *p);
Tree* parseAtom(Parser *p);
Tree* parseVarList(Parser *p);
Tree* parseExpList(Parser *p);
Tree* parseDeclList(Parser *p);
Tree* parseParamList(Parser *p);
Tree* parseStruct(Parser *p);
Tree* parseType(Parser *p);
Tree* parseId(Parser *p);

// char* tokenToType(char *token);
#define push(p, tag, SemType) sem=ObjNew(SemType, 1);Tree *node=push1(p, tag);node->sem=sem; 
Tree* push1(Parser *p, char* term);
Tree* pop(Parser *p, char* term);
BOOL isEnd(Parser *p);
// Token *nextN(Parser *p, int n);
// BOOL isNextN(Parser *p, int n, char *pTags);
BOOL isNext(Parser *p, char *pTags);
Tree *next(Parser *p, char *pTags);

// Token 的集合， 
#define SET_KEYWORDS "|if|else|for|while|return|def|int|byte|char|float|struct|"
#define SET_OP1 "|++|--|"
#define SET_OP2 "|+|-|*|/|%|^|&|<<|>>|==|!=|<=|>=|<|>|&&||||"
#define SET_BTYPE "|int|byte|char|float|"

#endif
