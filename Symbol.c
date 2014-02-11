#include "Symbol.h"

Symbol *SymNew(char *name, char *type) {
	Symbol *s = ObjNew(Symbol, 1);
	s->name = newStr(name);
	s->type = newStr(type);
	return s;
}

void SymFree(Symbol *s) {
	ObjFree(s->name);
	ObjFree(s->type);
	ObjFree(s);
}

