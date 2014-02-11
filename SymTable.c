// 本軟體採用公共領域授權 (Public Domain License)，您可以任意使用本軟體及其原始碼。 
// 但作者對於任何因本軟體所產生的損害（包含他人修改後所造成的損害），不負任何責任。
// 作者：陳鍾誠

#include "SymTable.h"

Symbol *Global /*全域區塊符號 */, *Int /*整數型態符號*/ , *Float /*浮點數型態符號*/ , *Char /*字元型態符號*/; 

// 功能：符號表的測試程式。
// 範例：SymTableTest();
void SymTableTest() {
	debug("=======SymTableTest()==========\n");    
    SymTable *st = SymTableNew();
    SymTableDebug(st);
    SymTableFree(st);
    memCheck();
}

// ========================= 符號表 Symbol Table =========================
// 功能：建立符號表物件。 
// 範例：SymTable *st = SymTableNew();
SymTable *SymTableNew() {
    SymTable *st = HashTableNew(127);
    Global = SymTablePut(st, SymNew(NULL, GLOBAL, SymBlock));
    Int = SymTablePutBType(st, kINT, 4);
    Float = SymTablePutBType(st, kFLOAT, 4);
    Char = SymTablePutBType(st, kCHAR, 1);
    return st;
}

// 功能：釋放符號表物件。 
// 範例：SymTableFree(st);
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

// 功能：新增符號到表中
// 範例：SymTablePut(st, SymNew(NULL, GLOBAL, SymBlock));
Symbol *SymTablePut(SymTable *st, Symbol *sym) {
    char key[100];
    SymDebug(sym);
    sprintf(key, "%p:%s", sym->scope, sym->name);
    HashTablePut(st, strNew(key), sym);
    return sym;
}

// 功能：新增基本型態到符號表中 
// 範例：Int = SymTablePutBType(st, kINT, 4);
Symbol *SymTablePutBType(SymTable *st, char *name, int size) {
    Symbol *sym = SymNew(Global, name, SymBType);
    sym->size = size;
    return SymTablePut(st, sym);
}

// 功能：查出符號表中的符號。 
// 範例：Symbol *s = SymTableGet(st, Global, "int");
Symbol* SymTableGet(SymTable *st, void *scope, char *name) {
    char key[100];
    sprintf(key, "%p:%s", scope, name);
    Symbol *sym = HashTableGet(st, key);
    return sym;
}

// 功能：印出符號表以便除錯。 
// 範例：SymTableDebug(st);
void SymTableDebug(SymTable *st) {
	debug("%-4s %-8s %4s %-8s %-8s %-8s %s\n", "type", "name", "size", "this", "scope", "offset", "varType");
    HashTableEach(st, (FuncPtr1) SymDebug);
}

// ========================= 符號物件 Symbol  =========================
// 功能：建立符號 
// 範例：Symbol *Int = SymNew(Global, "int", SymBType);
Symbol *SymNew(Symbol *scope, char *name, SymType symType) {
	Symbol *s = ObjNew(Symbol, 1);
	s->name = strNew(name);
	s->scope = scope;
	s->symType = symType;
	
    if (s->symType == SymStruct) {
        s->typePtr = StructNew();
    } else if (s->symType == SymMethod) {
        s->typePtr = MethodNew();
    } else if (s->symType == SymBlock) {
        s->typePtr = BlockNew();
    } else if (s->symType == SymVar) {
        s->typePtr = PTypeNew();
    } else if (s->symType == SymBType) {
        s->typePtr = BTypeNew();
    } else
        ERROR();
	return s;
}

// 功能：釋放符號 
// 範例：SymFree(Int);
void SymFree(Symbol *s) {
    if (s->symType == SymStruct) {
        StructFree(s->typePtr);
    } else if (s->symType == SymMethod) {
        MethodFree(s->typePtr);
    } else if (s->symType == SymBlock) {
        BlockFree(s->typePtr);
    } else if (s->symType == SymVar) {
        PTypeFree(s->typePtr);
    } else if (s->symType == SymBType) {
        BTypeFree(s->typePtr);
    } else
        ERROR();
	strFree(s->name);
	ObjFree(s);
}

// 功能：印出符號以便除錯。 
// 範例：SymDebug(Int); 
void SymDebug(Symbol *s) {
	debug("%-4c %-8s %-4d %-8p %-8p ", s->symType, s->name, s->size, s, s->scope);
    if (s->symType == SymVar)
        PTypeDebug(s->typePtr);
    debug("\n");
}

// ======================== 符號 typePtr 欄位的各種型態 ==============
// 功能：建立區塊物件 
// 範例：Block *b = BlockNew(); 
Block *BlockNew() {
    Block *block = ObjNew(Block, 1);
    block->vars = ArrayNew(1);
    return block;
}

// 功能：釋放區塊物件 
// 範例：BlockFree(block); 
void BlockFree(Block *block) {
    ArrayFree(block->vars, NULL);
    ObjFree(block);
}

// 功能：建立函數物件 
// 範例：Method *m = MethodeNew(); 
Method *MethodNew() {
    Method *m = ObjNew(Method, 1);
    m->params = ArrayNew(0);
//    m->block = BlockNew();
    return m;    
}

// 功能：釋放函數物件 
// 範例：Method *m = MethodNew(); 
void MethodFree(Method *m) {
//    BlockFree(m->block);
    ArrayFree(m->params, NULL);
    ObjFree(m);
}

// 功能：建立結構物件 
// 範例：Struct *s = StructNew(); 
Struct *StructNew() {
    Struct *s = ObjNew(Struct, 1);
    s->fields = ArrayNew(1);
//    s->block = BlockNew();
    return s;
}

// 功能：釋放結構物件 
// 範例：StructFree(s); 
void StructFree(Struct *s) {
//    BlockFree(s->block);
    ArrayFree(s->fields, NULL);
    ObjFree(s);
}

// 功能：建立型態物件 (可能是基本型態或指標型態，某變數的形態)。 
// 範例：PType *ptype = PTypeNew(); 
PType *PTypeNew() {
    PType *pt = ObjNew(PType, 1);
    return pt;
}

// 功能：釋放型態物件
// 範例：PTypeFree(pt); 
void PTypeFree(PType *pt) {
    ObjFree(pt);
}

// 功能：印出型態物件
// 範例：PTypeDebug(pt); 
void PTypeDebug(PType *pt) {
    char typeStr[100];
    debug("%-20s", PTypeToStr(typeStr, pt));
}
// 功能：將 PType 印成字串到 str 中，並傳回 str。 
// 範例：char *str = PTypeToStr(str, ptype); 
char *PTypeToStr(char *str, PType *ptype) {
    sprintf(str, "%-8d %s:*%d:[%d] ", ptype->offset, ptype->typeSym->name, ptype->starCount, ptype->dimCount);
    return str;
}

// 功能：建立基本型態物件 
// 範例：BType *b = BTypeNew(); 
BType *BTypeNew() {
    BType *b = ObjNew(BType, 1);
    return b;
}

// 功能：建立基本型態物件 
// 範例：BTypeFree(b); 
void BTypeFree(BType *b) {
    ObjFree(b);
}

