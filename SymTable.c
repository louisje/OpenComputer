#include "SymTable.h"

void SymTableTest() {
	debug("=======SymTableTest()==========\n");    
    SymTable *st = SymTableNew();
    SymTableDebug(st);
    SymTableFree(st);
    memCheck();
}

Block Global = { .name = GLOBAL, .symType=SymBlock /*, .parent=NULL */ };

SymTable *SymTableNew() {
    SymTable *st = HashTableNew(127);
    SymTablePutType(st, kINT, 4);
    SymTablePutType(st, kFLOAT, 4);
    SymTablePutType(st, kCHAR, 1);
    return st;
}

void SymTableFree(SymTable *st) {
	int i,j;
	for (i=0; i<st->count; i++) {
		Array *hits = st->item[i];
		for (j=0; j<hits->count; j++) {
			Entry *e = hits->item[j];
			Symbol *s = (Symbol*) e->data;
			strFree(e->key);
			SymFree(s);
		}
	}
	HashTableFree(st);
}

Symbol *SymTablePut(SymTable *st, Symbol *sym) {
    char key[100];
    sprintf(key, "%p:%s", sym->scope, sym->name);
    HashTablePut(st, strNew(key), sym);
    return sym;
}

Symbol* SymTableGet(SymTable *st, void *scope, char *name) {
    char key[100];
    sprintf(key, "%p:%s", scope, name);
    Symbol *sym = HashTableGet(st, key);
    return sym;
}

void SymTablePutType(SymTable *st, char *name, int size) {
    Symbol *sym = SymNew(&Global, name, SymBType);
    sym->size = size;
    SymTablePut(st, sym);
}

void SymTableDebug(SymTable *st) {
	debug("%-8s %4s %-8s\n", "name", "size", "scope");
    HashTableEach(st, SymDebug);
}

Symbol *SymNew(Block *scope, char *name, SymType symType) {
	Symbol *s = ObjNew(Symbol, 1);
	s->scope = scope;
	s->name = strNew(name);
	s->symType = symType;
	return s;
}

void SymFree(Symbol *s) {
    
	ObjFree(s->name);
	ObjFree(s);
}

char *typeToStr(char *str, Var *var) {
    sprintf(str, "%s:*%d:[%d]", var->typeName, var->starCount, var->dimCount);
    return str;
}

void SymDebug(void *data) {
    Symbol *s = (Symbol*) data;
	debug("%-8s %-4d %p", s->name, s->size, s->scope);
    if (s->symType == SymVar) {
        char typeStr[100];
        debug("%-16s %p", typeToStr(typeStr, s->u.var), s->scope);
    }
    debug("\n");
}

Block *BlockNew(void *parent, char *name, SymType symType) {
    Block *block = ObjNew(Block, 1);
    block->parent = parent;
    block->name = strNew(name);
    block->symType  = symType;
}

void BlockFree(Block *block) {
    strFree(block->name);
    ObjFree(block);
}

Method *MethodNew() {
    Method *m = ObjNew(Method, 1);
    m->params = ArrayNew(1);
    return m;    
}

void MethodFree(Method *m) {
    ArrayFree(m->params, (FuncPtr1) VarFree);
    ObjFree(m);
}

Struct *StructNew() {
    Struct *s = ObjNew(Struct, 1);
    s->fields = ArrayNew(1);
    return s;
}

void StructFree(Struct *s) {
    ArrayFree(s->fields, (FuncPtr1) VarFree);
    ObjFree(s);
}

Var *VarNew() {
    Var *v = ObjNew(Var, 1);
    return v;
}

void VarFree(Var *v) {
    ObjFree(v);
}
