#include "Interpreter.h"

void interpret(char *cFile) { // 解譯器主程式                  
	printf("interpret file:%s\n", cFile);
	char *cText = fileToStr(cFile);               //   讀取檔案到 cText 字串中。   
	SymTable *symTable = SymTableNew();
	Scanner *scanner = ScannerNew(cText);
	Tree *tree = parse(cText);           //   剖析程式 (cText) 轉為語法樹 
/*	Interpreter *inter = InterNew(); 
	inter->tree = parser->tree;
	char rzVar[100];
	InterRun(inter, inter->tree, rzVar);
	InterFree(inter); */
	TreeFree(tree);
	memFree(cText);
	ERROR();
}

Interpreter *InterNew() {
	Interpreter *o = ObjNew(Interpreter, 1);
	return o;
}

void InterFree(Interpreter *o) {
	ObjFree(o);
}

void InterRun(Interpreter *o, Tree *node, char *rzVar) {
}
