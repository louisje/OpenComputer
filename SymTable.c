// ���n��ĥΤ��@�����v (Public Domain License)�A�z�i�H���N�ϥΥ��n��Ψ��l�X�C 
// ���@�̹�����]���n��Ҳ��ͪ��l�`�]�]�t�L�H�ק��ҳy�����l�`�^�A���t����d���C
// �@�̡G�����

#include "SymTable.h"

Symbol *Global /*����϶��Ÿ� */, *Int /*��ƫ��A�Ÿ�*/ , *Float /*�B�I�ƫ��A�Ÿ�*/ , *Char /*�r�����A�Ÿ�*/; 

// �\��G�Ÿ������յ{���C
// �d�ҡGSymTableTest();
void SymTableTest() {
	debug("=======SymTableTest()==========\n");    
    SymTable *st = SymTableNew();
    SymTableDebug(st);
    SymTableFree(st);
    memCheck();
}

// ========================= �Ÿ��� Symbol Table =========================
// �\��G�إ߲Ÿ�����C 
// �d�ҡGSymTable *st = SymTableNew();
SymTable *SymTableNew() {
    SymTable *st = HashTableNew(127);
    Global = SymTablePut(st, SymNew(NULL, GLOBAL, SymBlock));
    Int = SymTablePutBType(st, kINT, 4);
    Float = SymTablePutBType(st, kFLOAT, 4);
    Char = SymTablePutBType(st, kCHAR, 1);
    return st;
}

// �\��G����Ÿ�����C 
// �d�ҡGSymTableFree(st);
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

// �\��G�s�W�Ÿ����
// �d�ҡGSymTablePut(st, SymNew(NULL, GLOBAL, SymBlock));
Symbol *SymTablePut(SymTable *st, Symbol *sym) {
    char key[100];
    SymDebug(sym);
    sprintf(key, "%p:%s", sym->scope, sym->name);
    HashTablePut(st, strNew(key), sym);
    return sym;
}

// �\��G�s�W�򥻫��A��Ÿ��� 
// �d�ҡGInt = SymTablePutBType(st, kINT, 4);
Symbol *SymTablePutBType(SymTable *st, char *name, int size) {
    Symbol *sym = SymNew(Global, name, SymBType);
    sym->size = size;
    return SymTablePut(st, sym);
}

// �\��G�d�X�Ÿ������Ÿ��C 
// �d�ҡGSymbol *s = SymTableGet(st, Global, "int");
Symbol* SymTableGet(SymTable *st, void *scope, char *name) {
    char key[100];
    sprintf(key, "%p:%s", scope, name);
    Symbol *sym = HashTableGet(st, key);
    return sym;
}

// �\��G�L�X�Ÿ���H�K�����C 
// �d�ҡGSymTableDebug(st);
void SymTableDebug(SymTable *st) {
	debug("%-4s %-8s %4s %-8s %-8s %-8s %s\n", "type", "name", "size", "this", "scope", "offset", "varType");
    HashTableEach(st, (FuncPtr1) SymDebug);
}

// ========================= �Ÿ����� Symbol  =========================
// �\��G�إ߲Ÿ� 
// �d�ҡGSymbol *Int = SymNew(Global, "int", SymBType);
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

// �\��G����Ÿ� 
// �d�ҡGSymFree(Int);
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

// �\��G�L�X�Ÿ��H�K�����C 
// �d�ҡGSymDebug(Int); 
void SymDebug(Symbol *s) {
	debug("%-4c %-8s %-4d %-8p %-8p ", s->symType, s->name, s->size, s, s->scope);
    if (s->symType == SymVar)
        PTypeDebug(s->typePtr);
    debug("\n");
}

// ======================== �Ÿ� typePtr ��쪺�U�ث��A ==============
// �\��G�إ߰϶����� 
// �d�ҡGBlock *b = BlockNew(); 
Block *BlockNew() {
    Block *block = ObjNew(Block, 1);
    block->vars = ArrayNew(1);
    return block;
}

// �\��G����϶����� 
// �d�ҡGBlockFree(block); 
void BlockFree(Block *block) {
    ArrayFree(block->vars, NULL);
    ObjFree(block);
}

// �\��G�إߨ�ƪ��� 
// �d�ҡGMethod *m = MethodeNew(); 
Method *MethodNew() {
    Method *m = ObjNew(Method, 1);
    m->params = ArrayNew(0);
//    m->block = BlockNew();
    return m;    
}

// �\��G�����ƪ��� 
// �d�ҡGMethod *m = MethodNew(); 
void MethodFree(Method *m) {
//    BlockFree(m->block);
    ArrayFree(m->params, NULL);
    ObjFree(m);
}

// �\��G�إߵ��c���� 
// �d�ҡGStruct *s = StructNew(); 
Struct *StructNew() {
    Struct *s = ObjNew(Struct, 1);
    s->fields = ArrayNew(1);
//    s->block = BlockNew();
    return s;
}

// �\��G���񵲺c���� 
// �d�ҡGStructFree(s); 
void StructFree(Struct *s) {
//    BlockFree(s->block);
    ArrayFree(s->fields, NULL);
    ObjFree(s);
}

// �\��G�إ߫��A���� (�i��O�򥻫��A�Ϋ��Ы��A�A�Y�ܼƪ��κA)�C 
// �d�ҡGPType *ptype = PTypeNew(); 
PType *PTypeNew() {
    PType *pt = ObjNew(PType, 1);
    return pt;
}

// �\��G���񫬺A����
// �d�ҡGPTypeFree(pt); 
void PTypeFree(PType *pt) {
    ObjFree(pt);
}

// �\��G�L�X���A����
// �d�ҡGPTypeDebug(pt); 
void PTypeDebug(PType *pt) {
    char typeStr[100];
    debug("%-20s", PTypeToStr(typeStr, pt));
}
// �\��G�N PType �L���r��� str ���A�öǦ^ str�C 
// �d�ҡGchar *str = PTypeToStr(str, ptype); 
char *PTypeToStr(char *str, PType *ptype) {
    sprintf(str, "%-8d %s:*%d:[%d] ", ptype->offset, ptype->typeSym->name, ptype->starCount, ptype->dimCount);
    return str;
}

// �\��G�إ߰򥻫��A���� 
// �d�ҡGBType *b = BTypeNew(); 
BType *BTypeNew() {
    BType *b = ObjNew(BType, 1);
    return b;
}

// �\��G�إ߰򥻫��A���� 
// �d�ҡGBTypeFree(b); 
void BTypeFree(BType *b) {
    ObjFree(b);
}

