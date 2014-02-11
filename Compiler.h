#ifndef COMPILER_H

#include "Parser.h"
#include "Semantic.h"
#include "Generator.h"
/*
typedef struct {            // 剖析器的物件結構      
	SymTable  *symTable;    // 符號表 
	Scanner   *scanner;     // 詞彙掃描器 (Lexical Analysis)
	Parser    *parser;      // 語法剖析器 (Parser) 
	SA        *sa;          // 語意分析器 (Semantic Analyzer)
	Generator *gen;         // 程式碼產生器 (Code Generator) 
} Compiler;

Compiler *CompilerNew();
void CompilerCompile(char *cFile, char *asmFile);
void CompilerFree(Compiler *c);
*/
void compile(char *cFile, char *asmFile);

#endif
