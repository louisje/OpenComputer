#ifndef COMPILER_H

#include "Parser.h"
#include "Semantic.h"
#include "Generator.h"
/*
typedef struct {            // ��R�������󵲺c      
	SymTable  *symTable;    // �Ÿ��� 
	Scanner   *scanner;     // ���J���y�� (Lexical Analysis)
	Parser    *parser;      // �y�k��R�� (Parser) 
	SA        *sa;          // �y�N���R�� (Semantic Analyzer)
	Generator *gen;         // �{���X���;� (Code Generator) 
} Compiler;

Compiler *CompilerNew();
void CompilerCompile(char *cFile, char *asmFile);
void CompilerFree(Compiler *c);
*/
void compile(char *cFile, char *asmFile);

#endif
