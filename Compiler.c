#include "Compiler.h"
/*
void compile(char *cFile, char *asmFile) {     // �sĶ���D�{��                  
    Compiler *c = CompilerNew();
    CompilerCompile(cFile, asmFile);
    CompilerFree(c);
}

Compiler *CompilerNew() {
    Compiler *c    = ObjNew(Compiler, 1);
	c->symTable    = SymTableNew();
	c->scanner     = ScannerNew();
	c->parser      = ParserNew(c->scanner, c->symTable);
	c->sa          = SaNew(c->symTable, c->parser);
	c->gen         = GenNew(c->symTable, c->parser->tree);
}

void CompilerCompile(char *cFile, char *asmFile) {
  printf("compile file:%s\n", cFile, asmFile);                               
  char *cText = fileToStr(cFile);              //   Ū���ɮר� cText �r�ꤤ�C   
	debug("================ Scan =========================\n");
	ScannerScan(c->scanner, cText);
	debug("================ Parse ========================\n");
	Tree *tree = ParserParse(c->parser, c->scanner); // ��R�{�� (cText) �ର�y�k��
  freeMemory(cText);
	debug("================ Semantic Analysis ============\n");
	semanticAnalysis(tree, symTable);
	TreePrintLevel(tree, 0);
	debug("================ Code Generator ===============\n");
}

void CompilerFree(Compiler *c) {
	ParserFree(c->parser);
	SaFree(c->sa);
	GenFree(c->generator);
}
*/

void compile(char *cFile, char *asmFile) {             // �sĶ���D�{��                  
	printf("compile file:%s\n", cFile, asmFile);                               
	char *cText = fileToStr(cFile);                    //   Ū���ɮר� cText �r�ꤤ�C   
	SymTable *symTable = SymTableNew();
	Tree *tree = parse(cText, symTable);           //   ��R�{�� (cText) �ର�y�k�� 	
	ERROR();
	debug("================ Semantic Tree ================\n");
	semanticAnalysis(tree, symTable);
//	SymTableDebug(symTable);
//	ERROR();
	TreePrintLevel(tree, 0);
	ERROR();
	//  TreePrint(parser->tree);
//	generate(parser->tree, symTable, asmFile); //   �{���X����
	TreeFree(tree);
	freeMemory(cText);
	ERROR();
}
   
