// ���n��ĥΤ��@�����v (Public Domain License)�A�z�i�H���N�ϥΥ��n��Ψ��l�X�C 
// ���@�̹�����]���n��Ҳ��ͪ��l�`�]�]�t�L�H�ק��ҳy�����l�`�^�A���t����d���C
// �@�̡G�����

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
// ETYPE = (byte | char | int | float | ID) // �̫�@�� ID �����O TYPE [STRUCT]
// ID = [A-Za-z_][0-9A-Za-z_]*
// CINT = [0-9]+
// CFLOAT = [0-9]+.[0-9]+
// CSTR = ".*"
// OP2 = +|-|/|*|%|&|&&|^|<<|>>|<|>|<=|>=|==|!=|  �P | , ||
// OP1 = ++ | --

#ifndef PARSER_H
#define PARSER_H

#include "Scanner.h"
#include "Tree.h"
#include "Lib.h"
#include "SymTable.h"

typedef struct {                           // ��R�������󵲺c      
	Array *nodeStack;      // ��R�L�{�Ϊ��`�I node ���| (�q��ڨ�ثe�`�I�����Ҧ��`�I�Φ������|)�C 
	Array *blockStack;     // �Ÿ��϶����|�A�ܼ� id ���϶��d��A���O PROG, STRUCT, METHOD, BLOCK ���C
	PType  ptype;            // �b parseType �ɥΨӰO���A���ܼơC 
	Scanner *scanner;      // ���J���y�� (Lexical Analysis)
	SymTable *symTable;    // �Ÿ��� 
	Tree *atomLast, *op;
	char  spaces[MAX_LEN]; // �ΨӼȦs�ťզr�ꪺ�ܼơC 
} Parser;                                                     
                                                              
Tree *parse(char *text, SymTable *symTable);// ��R�����D�{��
Parser *ParserNew(Scanner *scanner, SymTable *symTable); // ��R�����غc���
Tree *ParserParse(Parser *p, char *text);   // ��R������R���
void ParserFree(Parser *parser);           // ����O����         

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

BOOL isMethod(Parser *p); // �P�_���U�ӬO�_�� METHOD �{���϶��C 
BOOL isDecl(Parser *p);  // �P�_���U�ӬO�_�� DECL �ŧi�y�y 
#define push(p, tag, SemType) sem=ObjNew(SemType, 1);Tree *node=push1(p, tag);node->sem=sem; 
Tree *push1(Parser *p, char* tag);  // �إ߼аO�� tag ���s�l��C 
Tree *pop(Parser *p, char* tag);    // �q���|�����X��R�������l��A���ˬd�аO�O�_�� tag�C 
BOOL isNext(Parser *p, char *tags); // �ˬd�U�@�� token �� tag �O�_�ݩ� tags �аO���@�C 
Tree *next(Parser *p, char *tags);  // ���o�U�@�� token�A�ýT�{�� tag �� tags �аO���@�C 
char *token(Tree *node);            // ���o�𸭸`�I node �� token�C 

void pushBlock(Parser *p, Symbol *sym); // �N�϶��Ÿ����J���| 
#define popBlock(p) ArrayPop(p->blockStack) // �q���|���X�϶��Ÿ� 
#define peekBlock(p) ArrayPeek(p->blockStack) // ���o�̤W�����϶��Ÿ� 

// Token �����X�A�Ψ��ˬd�O����r�A�B�⤸�A�򥻫��A�A�Ϊ̥u�O�ܼ� ID�C 
#define SET_KEYWORDS "|if|else|for|while|return|def|int|byte|char|float|struct|"
#define SET_OP1 "|++|--|"
#define SET_OP2 "|+|-|*|/|%|^|&|<<|>>|==|!=|<=|>=|<|>|&&||||"
#define SET_BTYPE "|int|byte|char|float|"

// ========================= �y�N���R Semantic Analysis ==============================

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
