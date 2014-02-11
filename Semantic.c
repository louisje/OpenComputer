#include "Parser.h"

Symbol* SaSymbolLookup(Parser *p, char *name) {
    int i;
    Symbol *sym = NULL;
    for (i=p->blockStack->count-1; i>=0; i--) {
        Tree *block = p->blockStack->item[i];
        sym=SymTableGet(p->symTable, block, name);
        if (sym != NULL)
            return sym;
    }
    return NULL;
}

/*
// EXP = TERM (OP2 TERM)?
void SaExp(Parser *p, Tree *node) {
     SemExp *semExp = node->sem;
     SemTerm *semTerm1 = semExp->term1->sem;
     semExp->ptype = semTerm1->ptype;
}
// TERM = ( EXP (OP2 EXP)? ) | CINT | CFLOAT | CSTR | PATH
void SaTerm(Parser *p, Tree *node) {
     SemTerm *sem = node->sem;
     PType *ptype = PTypeNew();
     
     if (sem->subTag == EXP) {
        SemExp *semExp = sem->exp1->sem;
        ObjCopy(ptype, semExp->ptype, PType);
     } else if (sem->subTag == PATH) {
        SemExp *semPath = sem->path->sem;
        ObjCopy(ptype, semPath->ptype, PType);
     }
     else if (sem->subTag == CINT) {
        ptype->typeSym = Int;
     } else if (sem->subTag == CFLOAT) {
        ptype->typeSym = Float;
     } else if (sem->subTag == CSTR) {
        ptype->typeSym = Char;
        ptype->starCount = 1;
     }
     sem->ptype = ptype;
}
// PATH = ATOM ((.|->) ATOM)*
void SaPath(Parser *p, Tree *node) {
    Tree *lastAtom = child(node, node->childs->count-1);
    SemAtom *lastSem = lastAtom->sem;
    PType *ptype = PTypeNew();
    ObjCopy(ptype, lastSem->ptype, PType);
}

// ATOM = ID (([ EXP ])* |( EXP_LIST? ))
void SaAtom(Parser *p, Tree *atomLast, Tree *op, Tree *atom) {
     Symbol *sym = NULL;
     PType *ptype = NULL;
     SemAtom *sem = atom->sem;
     char *id = token(sem->id);
     if (atomLast == NULL) {
        sym = SaSymbolLookup(p, id);
     } else {
        SemAtom *semLast = atomLast->sem;
        ASSERT(semLast->ptype.typeSym->symType == SymStruct);
        sym = SymTableGet(p->symTable, semLast->ptype.typeSym, id);
     }
     SymDebug(sym);
     
     if (sem->subTag == ID) {
        ASSERT(sym->symType == SymVar);
        ObjCopy(sem->ptype, sym->typePtr, PType);
//        sem->var = var->typeSym;
     } else if (sem->subTag == CALL) {
        ASSERT(sym->symType == SymMethod);
        Method *method = sym->typePtr;
        ObjCopy(sem->ptype, &method->ret, PType);
//        sem->typeSym = ret->typeSym;
     } else if (sem->subTag == ARRAY_MEMBER) {
        sem->ptype = sym->typePtr;
//       ERROR();
     } else {
        ERROR();
     }
     
     debug("SaAtom(id=%s)\n", id);
     PTypeDebug(sem->ptype);
     debug("\n");
}
*/

/*
// =================== Semantic Analysis ====================================
void semanticAnalysis(Tree *tree, SymTable *symTable) { // Semantic Analysis
    SA *sa = SaNew(tree, symTable);
    SaTree(sa, tree);
    SaFree(sa);
}

SA *SaNew(Tree *tree, SymTable *symTable) {
	SA *sa = ObjNew(SA, 1);
	sa->tree = tree;
	sa->symTable = symTable;
	sa->blockStack = ArrayNew(10);
	return sa;
}

void SaFree(SA *sa) {
    ArrayFree(sa->blockStack, NULL);
    ObjFree(sa);
}

void SaAllChilds(SA *sa, Tree *node) {
    int i;
    if (node == NULL || node->childs == NULL) return;
    for (i=0; i<node->childs->count; i++) {
         Tree *c = child(node, i);
         SaTree(sa, c);
    }
} */
/*
Tree* SaPushBlock(SA *sa, Tree *block) {
    ArrayPush(sa->blockStack, block);
}

Tree* SaPopBlock(SA *sa) {
    return ArrayPop(sa->blockStack);
}

Tree* SaPeekBlock(SA *sa) {
    return ArrayPeek(sa->blockStack);
}
char *op2type(Tree *a, Tree *op, Tree *b) {
    if (a->type != b->type)
        ERROR();
    else
        return a->type;
}

void setIdType(Tree *id, char *type) {
    debug("setIdType(%s, %s)\n", token(id), type);
    id->type = strNew(type);
}
*/
/*

Symbol* SaSymbolLookup(SA *sa, char *name) {
    int i;
    Symbol *sym = NULL;
    for (i=sa->blockStack->count-1; i>=0; i--) {
        Tree *block = sa->blockStack->item[i];
        sym=SymTableGet(sa->symTable, block, name);
        if (sym != NULL)
            return sym;
    }
    return NULL;
}

Symbol* SaGlobalLookup(SA *sa, char *name) {
    return SymTableGet(sa->symTable, sa->tree, name);
}
// 取得 pStruct (STRUCT) 的子欄位 field 的 
Symbol* SaFindStructField(SA *sa, char *structType, char *fieldName) { 
    Symbol *structSym = SaGlobalLookup(sa, structType);
    return SymTableGet(sa->symTable, structSym->node, fieldName);
}
void strip(char *str, char c) {
    int last = strlen(str)-1;
    if (str[last] == c)
        str[last] = '\0';
}
*/

// ATOM = ID (([ EXP ])* |( EXP_LIST? ))
/*void SaAtom(SA *sa, Tree *atomLast, char *op, Tree *atom) {
    SemAtom *sem = atom->sem;
    char *id = token(sem->id);
    Symbol *sym = NULL;
    char typeNow[100];
    if (atomLast == NULL) {
        sym = SaSymbolLookup(sa, id);
    } else {
        strcpy(typeNow, atomLast->type);
        if (strEqual(op, "->"))
           strip(typeNow, '*');
        char *fieldName = token(sem->id); // birtyday
//        sym = SaFindStructField(sa, typeNow, fieldName); // fieldType=type(birthday)
    }
    
    if (sem->subTag == CALL) {
        atom->type = newStr(sym->type);
    } else if (sem->subTag == ID) {
        atom->type = newStr(sym->type);
    } else if (sem->subTag == ARRAY_MEMBER) {
        strcpy(typeNow, sym->type);
        int i;
        for (i=0; i<sem->exps->count; i++)
            strip(typeNow, '*'); // 型態 type* 變成 type，去除星號。 
    }
    if (sym == NULL) ERROR();
    SymDebug(sym);
    setIdType(sem->id, sym->type); // node(birthday).type = fieldType
    strcpy(typeNow, sym->type);
    int i;
    for (i=1; i<atom->childs->count; i++) {
        Tree *c = child(atom, i);
        debug("typeNow = %s\n", typeNow);
        if (c->tag == EXP) // ID [ EXP ]
            strip(typeNow, '*'); // 型態 type* 變成 type，去除星號。 
    }
    atom->type = newStr(typeNow);
    debug("atom->type=%s\n", atom->type);
}

void starType(char *type, int starCount, char *rzType) {
    char stars[] = "***********************";
    ASSERT(starCount < strlen(stars));
    stars[starCount]='\0';
    sprintf(rzType, "%s%s", type, stars);
}
*/
/*
void SaTree(SA *sa, Tree *node) {
    int i;
    char type[100];
    Array *childs = node->childs;
    char *tag = node->tag;
//     Tree *scope = SaPeekBlock(sa);
    
    // Top-Down: 遞回下降時，設定每個節點的形態。 
    if (tag == PROG) { // PROG = (STRUCT | METHOD | DECL ; )*
//        SaPushBlock(sa, node);
    } else if (tag == STRUCT) { // STRUCT = struct ID { DECL_LIST ; }
        SemStruct *sem = node->sem;
        sem->fields = ArrayNew(2);
        char *id = token(sem->id);
        Symbol *sym = SymNew(scope, id, STRUCT);
        SymTablePut(sa->symTable, sym);
//        SaPushBlock(sa, node);
    } else if (tag == METHOD) { // METHOD = TYPE ** ID(DECL_LIST?) BLOCK
        SemMethod *sem = node->sem;
        char *id = token(sem->id);
        Symbol *sym = SymNew(scope, id, METHOD);
        SymTablePut(sa->symTable, sym);
        SemType *semType = sem->type->sem;
        char returnType[100];
        starType(semType->type, sem->starCount, returnType);
        sem->id->type = newStr(type);
        debug("returnType = %s\n", returnType);
//        ASSERT(FALSE); 
//        SaPushBlock(sa, node);
    } else if (tag == BLOCK) { // BLOCK = { BASE_LIST }
        SemBlock *sem = node->sem;
        Symbol *sym = SymNew(scope, "", BLOCK);
        SymTablePut(sa->symTable, sym);
//        SaPushBlock(sa, node);
    } else if (tag == DECL) { // DECL = TYPE VAR_LIST
        // 在 TYPE 時設定 sa->type，給 VAR 節點使用。 
    } else if (tag == TYPE) { // TYPE = (byte | char | int | float | ID) (最後一個 ID 必須是 TYPE [STRUCT])
        Tree *c0 = child(node, 0);
        strcpy(sa->type, token(c0));
    } else if (tag == VAR) { // VAR = ** ID ([ CINT ])* (= EXP)?
        SemVar *sem = node->sem;
//        starType(sa->type, sem->starCount, type);
//        char *id = token(sem->id);
//        Symbol *sym = SymNew(scope, id, VAR);
//        SymTablePut(sa->symTable, sym);
    } else if (tag == PATH) { // PATH = ATOM (.|->) ATOM)*  ex: p.birthday.year 
        // p => Person*
        Tree *atom = child(node,0); // p
        SaAtom(sa, NULL, NULL, atom); // node(p).type = Person*
        Tree *atomLast = atom;
        for (i=1; i<node->childs->count; i+=2) { // Type Chaining，不斷設定子代的形態。 
            char *op = token(child(node, i));
            Tree *atom = child(node, i+1);
            SaAtom(sa, atomLast, op, atom);
            atomLast = atom;
        }
    } else if (tag == CINT) {
        node->type = kINT;
    } else if (tag == CFLOAT) {
        node->type = kFLOAT;
    } else if (tag == CSTR) {
        node->type = kSTR;
    }
    
    SaAllChilds(sa, node);
    
    // Bottom-UP: 由下而上返回時檢查型態的相容性。 
    if (tag == PROG) { // PROG = (STRUCT | METHOD | DECL ; )*
        Tree *block = SaPopBlock(sa);
    } else if (tag == STRUCT) { // STRUCT = struct ID { DECL_LIST ; }
        Tree *block = SaPopBlock(sa);
    } else if (tag == METHOD) { // METHOD = TYPE ID(DECL_LIST?) BLOCK
        Tree *block = SaPopBlock(sa);
    } else if (tag == BLOCK) { // BLOCK = { BASE_LIST }
        Tree *block = SaPopBlock(sa);
    } else if (tag == STMT) { // STMT = return EXP | DECL | PATH (EXP_LIST) | PATH = EXP | PATH OP1
        SemStmt *sem = node->sem;
        if (sem->path != NULL && sem->exp != NULL)
            checkTypeCompatable(path, exp);
        else if (sem->op1 != NULL)
            checkType(path, INT);
        if (sem->exp != NULL)
            checkTypeCompatable(type, sem->exp);
    } else if (tag == EXP) { // EXP = TERM (OP2 TERM)?
        SemExp *sem = node->sem;
        if (sem->term1 != NULL && sem->term2 != NULL)
            node->type = op2type(sem->term1, sem->op, sem->term2);
    } else if (tag == TERM) { // TERM = ( EXP (OP2 EXP)? ) | CINT | CFLOAT | CSTR | PATH
        SemTerm *sem = node->sem;
        if (sem->exp1 != NULL && sem->exp2 != NULL)
            node->type = op2type(sem->exp1, sem->op, sem->exp2);
    }
}
*/
/*
SemStruct* StructNew() {
    SemStruct *s = ObjNew(SemStruct, 1);
    s->fields = ArrayNew(2);
}

void StructFree(SemStruct *s) {
    ArrayFree(s->fields, (FuncPtr1) ObjFree);
    ObjFree(s);
}*/
/*
Var* VarListFind(Array* vars, char *name) {
    int i;
    for (i=0; i<vars->count; i++) {
        Var *var = vars->item[i];
        if (strEqual(name, var->name))
            return var;
    }
    return NULL;
}
*/
