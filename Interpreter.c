#include "Interpreter.h"

void interpret(char *cFile) { // ��Ķ���D�{��                  
	printf("interpret file:%s\n", cFile);
	char *cText = fileToStr(cFile);               //   Ū���ɮר� cText �r�ꤤ�C   
	SymTable *symTable = SymTableNew();
	Scanner *scanner = ScannerNew(cText);
	Tree *tree = parse(cText);           //   ��R�{�� (cText) �ର�y�k�� 
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
