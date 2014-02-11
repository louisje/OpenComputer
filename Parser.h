// 本軟體採用公共領域授權 (Public Domain License)，您可以任意使用本軟體及其原始碼。 
// 但作者對於任何因本軟體所產生的損害（包含他人修改後所造成的損害），不負任何責任。
// 作者：陳鍾誠

// PROG = (STRUCT | METHOD | DECL ; )*
// METHOD = ETYPE ** ID(PARAM_LIST?) BLOCK
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
// DECL = ETYPE VAR_LIST
// PARAM = ETYPE VAR
// VAR_LIST = VAR (, VAR)*
// EXP_LIST = EXP (, EXP)*
// DECL_LIST = DECL (; DECL)*
// PARAM_LIST = PARAM (, PARAM)*
// ETYPE = (byte | char | int | float | ID) // 最後一個 ID 必須是 TYPE [STRUCT]
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
#include "SymTable.h"

typedef struct {                           // 剖析器的物件結構      
	Array *nodeStack;      // 剖析過程用的節點 node 堆疊 (從樹根到目前節點間的所有節點形成的堆疊)。 
	Array *blockStack;     // 符號區塊堆疊，變數 id 的區塊範圍，像是 PROG, STRUCT, METHOD, BLOCK 等。
	PType  ptype;            // 在 parseType 時用來記住型態的變數。 
	Scanner *scanner;      // 詞彙掃描器 (Lexical Analysis)
	SymTable *symTable;    // 符號表 
	Tree *atomLast, *op;
	char  spaces[MAX_LEN]; // 用來暫存空白字串的變數。 
} Parser;                                                     
                                                              
Tree *parse(char *text, SymTable *symTable);// 剖析器的主程式
Parser *ParserNew(Scanner *scanner, SymTable *symTable); // 剖析器的建構函數
Tree *ParserParse(Parser *p, char *text);   // 剖析器的剖析函數
void ParserFree(Parser *parser);           // 釋放記憶體         

Tree* parseProg(Parser *p);     // PROG = (STRUCT | METHOD | DECL ; )*
Tree* parseBase(Parser *p);     // BASE = IF | FOR | WHILE | BLOCK | STMT ;
Tree* parseStruct(Parser *p);   // STRUCT = struct ID { DECL_LIST ; }
Tree* parseMethod(Parser *p);   // METHOD = ETYPE ** ID(PARAM_LIST?) BLOCK
Tree* parseDecl(Parser *p);     // DECL = ETYPE VAR_LIST
Tree* parseIf(Parser *p);       // IF = if (EXP) BASE (else BASE)?
Tree* parseFor(Parser *p);      // FOR = for (STMT ; EXP ; STMT) BASE
Tree* parseWhile(Parser *p);    // WHILE = while (EXP) BASE
Tree* parseStmt(Parser *p);     // STMT = return EXP | DECL | PATH (EXP_LIST) | PATH = EXP | PATH OP1
Tree* parseBlock(Parser *p);    // BLOCK = { BASE* }
Tree* parseVar(Parser *p);      // VAR = ** ID ([ integer ])* (= EXP)?
Tree* parseExp(Parser *p);      // EXP = TERM (OP2 TERM)?
Tree* parseTerm(Parser *p);     // TERM = ( EXP (OP2 EXP)? ) | CINT | CFLOAT | CSTR | PATH
Tree* parsePath(Parser *p);     // PATH = ATOM ((.|->) ATOM)*
Tree* parseAtom(Parser *p);     // ATOM = ID (([ EXP ])* |( EXP_LIST? ))
Tree* parseDecl(Parser *p);     // DECL = ETYPE VAR_LIST
Tree* parseParam(Parser *p);    // PARAM = ETYPE VAR
Tree* parseVarList(Parser *p);  // VAR_LIST = VAR (, VAR)*
Tree* parseExpList(Parser *p);  // EXP_LIST = EXP (, EXP)*
Tree* parseDeclList(Parser *p); // DECL_LIST = DECL (; DECL)*
Tree* parseParamList(Parser *p);// PARAM_LIST = PARAM (, PARAM)*
Tree* parseEType(Parser *p);    // ETYPE = (byte | char | int | float | ID)
Tree* parseId(Parser *p);       // ID = [A-Za-z_][0-9A-Za-z_]*

BOOL isMethod(Parser *p); // 判斷接下來是否為 METHOD 程式區塊。 
BOOL isDecl(Parser *p);  // 判斷接下來是否為 DECL 宣告語句 
#define push(p, tag, SemType) sem=ObjNew(SemType, 1);Tree *node=push1(p, tag);node->sem=sem; 
Tree *push1(Parser *p, char* tag);  // 建立標記為 tag 的新子樹。 
Tree *pop(Parser *p, char* tag);    // 從堆疊中取出剖析完成的子樹，並檢查標記是否為 tag。 
BOOL isNext(Parser *p, char *tags); // 檢查下一個 token 的 tag 是否屬於 tags 標記之一。 
Tree *next(Parser *p, char *tags);  // 取得下一個 token，並確認其 tag 為 tags 標記之一。 
char *token(Tree *node);            // 取得樹葉節點 node 的 token。 

void pushBlock(Parser *p, Symbol *sym); // 將區塊符號推入堆疊 
#define popBlock(p) ArrayPop(p->blockStack) // 從堆疊取出區塊符號 
#define peekBlock(p) ArrayPeek(p->blockStack) // 取得最上面的區塊符號 

// Token 的集合，用來檢查是關鍵字，運算元，基本型態，或者只是變數 ID。 
#define SET_KEYWORDS "|if|else|for|while|return|def|int|byte|char|float|struct|"
#define SET_OP1 "|++|--|"
#define SET_OP2 "|+|-|*|/|%|^|&|<<|>>|==|!=|<=|>=|<|>|&&||||"
#define SET_BTYPE "|int|byte|char|float|"

// ========================= 語意分析 Semantic Analysis ==============================

typedef struct { } SemProg;
typedef struct {  } SemBase;
typedef struct { Tree *exp, *base1, *base2; } SemIf;
typedef struct { Tree *stmt1, *exp, *stmt2; } SemFor;
typedef struct { Tree *exp, *base; } SemWhile;
typedef struct { Tree *type, *id, *paramList, *block; Symbol *symMethod; } SemMethod; // /*int starCount; char *returnType;
typedef struct { Tree *id; Symbol *symStruct; } SemStruct;
typedef struct { Tree *id, *exp; PType *ptype; } SemVar; // int starCount, dimList[10], dimCount;
typedef struct { Tree *exp, *decl, *path, *op1, *expList; } SemStmt;
typedef struct { Tree *type, *varList; } SemDecl;
typedef struct { Tree *type, *var; } SemParam;
typedef struct { Tree *baseList; Symbol *symBlock; } SemBlock;
typedef struct { Tree *term1, *op, *term2;  PType ptype; } SemExp;
typedef struct { Tree *exp1, *op, *exp2, *path;  char *subTag; PType ptype; } SemTerm; // int int1; float float1; char *str1;
typedef struct { PType ptype; } SemPath;
typedef struct { Tree *id, *expList; char *subTag; PType ptype; } SemAtom;
typedef struct { Array *varList; } SemVarList;
typedef struct { Array *expList; } SemExpList;
typedef struct { Array *declList; } SemDeclList;
typedef struct { Array *paramList; } SemParamList;
typedef struct { } SemEType;

Symbol* SymbolLookup(Parser *p, char *name);
#define PTypeCopy(to, from) ObjCopy(to, from, PType)

#endif
