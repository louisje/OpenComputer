#include "Compiler.h"
/*
void compile(char *cFile, char *asmFile) {     // 編譯器主程式                  
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
  char *cText = fileToStr(cFile);              //   讀取檔案到 cText 字串中。   
	debug("================ Scan =========================\n");
	ScannerScan(c->scanner, cText);
	debug("================ Parse ========================\n");
	Tree *tree = ParserParse(c->parser, c->scanner); // 剖析程式 (cText) 轉為語法樹
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

void compile(char *cFile, char *asmFile) {             // 編譯器主程式                  
	printf("compile file:%s\n", cFile, asmFile);                               
	char *cText = fileToStr(cFile);                    //   讀取檔案到 cText 字串中。   
	SymTable *symTable = SymTableNew();
	Tree *tree = parse(cText, symTable);           //   剖析程式 (cText) 轉為語法樹 	
	ERROR();
	debug("================ Semantic Tree ================\n");
	semanticAnalysis(tree, symTable);
//	SymTableDebug(symTable);
//	ERROR();
	TreePrintLevel(tree, 0);
	ERROR();
	//  TreePrint(parser->tree);
//	generate(parser->tree, symTable, asmFile); //   程式碼產生
	TreeFree(tree);
	freeMemory(cText);
	ERROR();
}
   
