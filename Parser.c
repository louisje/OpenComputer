// ���n��ĥΤ��@�����v (Public Domain License)�A�z�i�H���N�ϥΥ��n��Ψ��l�X�C 
// ���@�̹�����]���n��Ҳ��ͪ��l�`�]�]�t�L�H�ק��ҳy�����l�`�^�A���t����d���C
// �@�̡G�����

#include "Parser.h"

// �\��GParser ��R���q�����յ{���C
// �d�ҡGParserTest("test.c1");
void ParserTest(char *fileName) {
	debug("=======ParserTest()==========\n");
    SymTable *symTable = SymTableNew(); // �إ߲Ÿ��� 
    char *text = fileToStr(fileName);   // Ū�J C1 �y���{���X�A�����@�r�� 
    Tree *tree = parse(text, symTable); // ��R�ӵ{���X�A�إ߭�R��P�Ÿ���C
    SymTableDebug(symTable);            // �L�X�Ÿ���C
    TreeFree(tree);                     // �����R��C 
    strFree(text);                      // ����{���X�r�� 
    SymTableFree(symTable);             // ����Ÿ��� 
    memCheck(); // �ˬd�O����     
}

// �\��G��R���q���D�{�� 
// �d�ҡGTree *tree = parse(text, symTable); 
Tree *parse(char *text, SymTable *symTable) {  // ��R�����D�n���
	Scanner *scanner = ScannerNew(text);       // �إ߱��y�� (���J���R��) 
	Parser *p=ParserNew(scanner, symTable);    // �إ߭�R�� (�y�k��R��)
	Tree *tree = ParserParse(p, text);         // ��R�{�����y�k�� 
	ParserFree(p);                             // �����R�� 
	ScannerFree(scanner);                      // ���񱽴y�� 
	return tree;                            // �Ǧ^��R��
}

// �\��G�إ߷s����R�� Parser ���� 
// �d�ҡGParser *p = ParserNew(scanner, symTable);
Parser *ParserNew(Scanner *scanner, SymTable *symTable) {
	Parser *p = ObjNew(Parser, 1);
	p->nodeStack = ArrayNew(10);
	p->blockStack = ArrayNew(10);
	p->scanner = scanner;
	p->symTable = symTable;
    ScannerNext(scanner); // ����R���`�O�����o�U�@�� token�A�H�K isNext() �i��P�_�C
	return p;
}

// �\��G�����R�����󪺰O���� 
// �d�ҡGParserFree(p); 
void ParserFree(Parser *p) {
	ArrayFree(p->blockStack, (FuncPtr1) BlockFree);
	ArrayFree(p->nodeStack, NULL);
	ObjFree(p);
}

// �\��G��R��ӵ{���X (text)�C 
// �d�ҡGParserParse(p, text); 
Tree *ParserParse(Parser *p, char *text) { // ��R���󪺥D���
	debug("======= parsing ========\n");
	Tree *tree = parseProg(p); // �}�l��R��ӵ{�� (PROG)�A�ëإ߻y�k�� p->tree
	if (p->nodeStack->count != 0) { // �p�G��R��������|�O�Ū��A���N�O��R���\
		ERROR();// �_�h�N�O��R���ѡA���y�k���~
  }
	return tree;
}

// �y�k�GPROG = (STRUCT | METHOD | DECL ; )* 
// �\��G��R PROG �ëإ߻y�k�� 
// �d�ҡGTree *prog = parseProg(p); 
Tree *parseProg(Parser *p) { // ��R PROG �W�h
	SemProg *sem=push(p, PROG, SemProg); // �إ� PROG ���y�k��λy�N���c
	pushBlock(p, Global);  // ���J����϶� 
	while (!isNext(p, kEND)) {     //  ��R BASE�A����{�������θI�� } ����
	    if (isNext(p, "struct"))
	       parseStruct(p);
	    else { // �ѩ� METHOD �P DECL ���}�Y���O TYPE **ID ...�A�]�������P�_�O���@�ر��p�C
           if (isMethod(p)) { // �V�e���ݫ�o�{�O TYPE **ID(�A�ҥH�O Method 
                parseMethod(p);
           } else { // �_�h�N�����O DECL ; 
                parseDecl(p);
	            next(p, ";");
          }
        }
    }
    popBlock(p); // ���X����϶�
	return pop(p, PROG); // ���X PROG ����ʻy�k�� 
}

// �y�k�GMETHOD = TYPE **ID (PARAM_LIST?) BLOCK
// �\��G�P�_�쩳���U�ӬO�_�� METHOD�A�O���ܶǦ^ TRUE�A�_�h�Ǧ^ FALSE 
//       �ѩ� METHOD �P DECL ���}�Y���O TYPE **ID ...�A�]�������P�_�O���@�ر��p�C
//       ����Ʒ|�V�e���ݡA�p�G�o�{�O TYPE **ID(�A���N���ӬO Method�C 
// �d�ҡGif (isMethod(p)) parseMethod(p);
BOOL isMethod(Parser *p) {
    BOOL rzFlag = TRUE;
    Scanner *s = p->scanner; // s=���y�� 
    ScannerStore(s); // �x�s���y�����A 
    if (isNext(p, "int|byte|char|float|ID")) // ���� TYPE
        ScannerNext(s); // ���L TYPE
    else 
        rzFlag=FALSE;
    while (isNext(p, "*")) ScannerNext(s); // ���ݨò��L�P�� 
    if (isNext(p, ID))  // ���� ID
        ScannerNext(s); // ���L ID 
    else 
        rzFlag=FALSE;
    if (!isNext(p, "(")) rzFlag=FALSE; // �p�G���U�ӬO (�A����N���ӬO Method�C
    ScannerRestore(s); // ��_���y�����A�C 
    return rzFlag;
}

// �y�k�GMETHOD = TYPE **ID (PARAM_LIST?) BLOCK
// �\��G��R METHOD �ëإ߻y�k��
// �d�ҡGTree *method = parseMethod(p); 
Tree* parseMethod(Parser *p) {
	SemMethod *sem=push(p, METHOD, SemMethod); // �إ� METHOD ���y�k��λy�N���c
	sem->type=parseEType(p); // ��R ETYPE

	// ��R ** (n �ӬP��, n>=0)
	int starCount = 0; // �P���ƶq����l�� 
    while (isNext(p, "*")) { // �p�G�U�@�ӬO�P�� 
        next(p, "*"); // ���o�ӬP�� 
        starCount ++; // �N�P���ƥ[�@ 
    }
    sem->id = next(p, ID); // ��R ID
    
	// �إ� ID ���Ÿ��O�� Symbol(id, METHOD) 
	char *id = token(sem->id);	// ���o�Ÿ��W�١C 
	Symbol *sym = SymNew(Global, id, SymMethod); // �إ߲Ÿ��O�� 
	Method *method = sym->typePtr; // �]�w method ���c�C 
	method->ret.typeSym = p->ptype.typeSym; // �]�w�Ǧ^�Ÿ� 
	method->ret.starCount = p->ptype.starCount; // �]�w�Ǧ^�Ÿ����P���ӼơC 
	SymTablePut(p->symTable, sym); // �N�Ÿ��O����J�Ÿ��� 

	pushBlock(p, sym); // �N Method �Ÿ����J�϶����|
	
    sem->symMethod = sym; // �]�w�y�N���c sem �� symMethod ��� 
	
    // ��R�ѼƳ��� (PARAM_LIST?) 
  next(p, "(");
	if (!isNext(p, ")")) // �p�G���U�Ӥ��O )�A���N�O�� PARAM_LIST 
		sem->paramList = parseParamList(p); // ��R PARAM_LIST
  next(p, ")");
	
	sem->block = parseBlock(p); // ��R BLOCK
	SemBlock *semBlock = sem->block->sem;
	method->block = semBlock->symBlock; // �]�w method �Ÿ��� block �l�Ÿ� 

    popBlock(p);
	return pop(p, METHOD); // ���X METHOD ���y�k��C
}

// �y�k�GSTRUCT = struct ID { (DECL ;)* }
// �\��G��R STRUCT �ëإ߻y�k��
// �d�ҡGTree *s = parseStruct(p);
Tree* parseStruct(Parser *p) {
	SemStruct *sem=push(p, STRUCT, SemStruct); // �إ� STRUCT �y�k�� 

	next(p, "struct"); // ��R struct 
	sem->id = next(p, ID); // ��R ID
	
	// �إ� ID ���Ÿ��O�� Symbol(id, METHOD) 
	char *id = token(sem->id);	// ���o�Ÿ��W�١C 
	Symbol *sym = SymNew(Global, id, SymStruct); // �إ߲Ÿ� -- ���c�C 
	SymTablePut(p->symTable, sym); // ��J�Ÿ���C 
	
	sem->symStruct = sym;  // �]�w�y�N���c sem �� symMethod ��� 
	
    pushBlock(p, sym); // �N Struct �϶����J���|
	
	// ��R { (DECL ;)* }
	next(p, "{"); 
	while (!isNext(p, "}")) {
        parseDecl(p);
        next(p, ";");
    }
	next(p, "}");
	
	popBlock(p); // �q�϶����|�����X Struct �϶� 
    
	return pop(p, STRUCT); // ���X STRUCT ���y�k��C
}

// �y�k�GBASE = IF | FOR | WHILE | BLOCK | STMT ;
// �\��G��R BASE �ëإ� BASE ���y�k�� 
// �d�ҡGTree *base = parseBase(p); 
Tree* parseBase(Parser *p) { // ��R BASE �W�h
	SemBase *sem=push(p, BASE, SemBase); // �إ� BASE ���y�k��λy�N���c
	if (isNext(p, "if")) // �p�G�U�@�ӵ��J�O if
		parseIf(p); // ��R IF �{���q 
	else if (isNext(p, "for")) // �p�G�U�@�ӵ��J�O for
		parseFor(p); // ��R FOR �{���q
	else if (isNext(p, "while")) // �p�G�U�@�ӵ��J�O for
		parseWhile(p); // ��R WHILE �{���q
	else if (isNext(p, "{")) // �p�G�U�@�ӵ��J�O {
		parseBlock(p); // ��R BLOCK �{���q
	else { // �_�h���ӬO STMT ; 
		parseStmt(p); // ��R STMT �{���q
		next(p, ";"); // ���o���� ;  
	}
	return pop(p, BASE); // ���X BASE ����R��
}

// �y�k�GBLOCK = { BASE* }
// �\��G��R BLOCK �ëإ߻y�k�� 
// �d�ҡGTree *block = parseBlock(p); 
Tree* parseBlock(Parser *p) {
	SemBlock *sem=push(p, BLOCK, SemBlock); // �إ� BLOCK ���y�k��λy�N���c 
	
	Symbol *pblock = peekBlock(p); // ���o���϶� 
	Symbol *sym = SymNew(pblock, "", SymBlock); // �إ߰϶��Ÿ� 
	Block *block = sym->typePtr; // �]�w block ���c�C 
	SymTablePut(p->symTable, sym); // �N���϶��[�J��Ÿ��� 

	sem->symBlock = sym; // �]�w���`�I���y�N���c symBlock �����϶�

	pushBlock(p, sym); // �N�Ÿ����J�϶����|

	next(p, "{"); // ��R { BASE* } 
	while (!isNext(p, "}")) 
        parseBase(p);
	next(p, "}");
	
	popBlock(p);  // �q�϶����|�����X Block �϶�
    
	return pop(p, BLOCK); // ���X BLOCK ���y�k��C
}

// �y�k�GFOR = for (STMT ; EXP ; STMT) BASE
// �\��G��R FOR �ëإ߻y�k��
// �d�ҡGTree *f = parseFor(p); 
Tree* parseFor(Parser *p) {                  
	SemFor *sem=push(p, FOR, SemFor); // �إ� FOR ���y�k��λy�N���c
  next(p, "for");                           // ���o for
  next(p, "(");                             // ���o (
	sem->stmt1 = parseStmt(p);                             // ��R STMT
  next(p, ";");                             // ���o ;
	sem->exp = parseExp(p);                              // ��R EXP
  next(p, ";");                             // ���o ;
	sem->stmt2 = parseStmt(p);                             // ��R STMT
  next(p, ")");                             // ���o )
  parseBase(p);                             // ��R BASE
	return pop(p, FOR);               // ���X FOR ���y�k��C
}

// �y�k�GIF = if (EXP) BASE (else BASE)?
// �\��G��R IF �ëإ߻y�k��
// �d�ҡGTree *f = parseIf(p);
Tree* parseIf(Parser *p) {
	SemIf *sem=push(p, IF, SemIf);     // �إ� IF ���y�k��λy�N���c
	next(p, "if");                     // ���o if
	next(p, "(");                      // ���o (
	sem->exp = parseExp(p);            // ��R EXP 
	next(p, ")");                      // ���o )
	sem->base1 = parseBase(p);         // ��R BASE
	if (isNext(p, "else")) {           // �p�G�U�@�ӬO else 
		next(p, "else");               // ���o else
		sem->base2 = parseBase(p);     // ��R�U�@�� BASE
  }
	return pop(p, IF);                 // ���X IF ���y�k��C
}

// �y�k�GWHILE = while (EXP) BASE
// �\��G��R WHILE �ëإ߻y�k��
// �d�ҡGTree *w = parseWhile(p);
Tree* parseWhile(Parser *p) {
	SemWhile *sem=push(p, WHILE, SemWhile);// �إ� WHILE ���y�k��λy�N���c
	next(p, "while");                      // ���o while
	next(p, "(");                          // ���o (
	sem->exp = parseExp(p);                // ��R EXP
	next(p, ")");                          // ���o )
	sem->base = parseBase(p);              // ��R BASE
	return pop(p, WHILE);                  // ���X WHILE ���y�k��C
}

// �y�k�GSTMT = return EXP | DECL | PATH (EXP_LIST) | PATH = EXP | PATH OP1
// �\��G��R STMT �ëإ߻y�k��
// �d�ҡGTree *stmt = parseStmt(p); 
Tree* parseStmt(Parser *p) {
	SemStmt *sem=push(p, STMT, SemStmt);// �إ� STMT ���y�k��λy�N���c
	if (isNext(p, "return")) { // �p�G�U�@�ӬO return�A�N��R return EXP 
    next(p, "return");
		sem->exp = parseExp(p);
	} else if (isDecl(p)) { // �p�G�O DECL 
		sem->decl = parseDecl(p); // ��R DECL 
	} else { // �_�h�U�@�ӥ����O PATH 
        sem->path = parsePath(p); // ��R PATH 
		if (isNext(p, "(")) { // �U�@�ӬO (�A�N��O PATH (EXP_LIST) �����p 
      next(p, "(");
			sem->expList = parseExpList(p);
	  next(p, ")");
		} else if (isNext(p, "=")) { // �U�@�ӬO =�A�N��O PATH = EXP �����p 
	  next(p, "=");
			sem->exp = parseExp(p);
		} else if (isNext(p, SET_OP1)) { // �U�@�ӬOOP1�A�N��O PATH OP1 �����p 
			next(p, SET_OP1);
	} else
	  ERROR();
  }
	return pop(p, STMT); // ���X STMT ���y�k��C
}

// �y�k�GPATH = ATOM ((.|->) ATOM)*
// �\��G��R PATH �ëإ߻y�k�� 
// �d�ҡGTree *path = parsePath(p);
Tree* parsePath(Parser *p) {
    SemPath *sem=push(p, PATH, SemPath);// �إ� PATH ���y�k��λy�N���c
    p->atomLast = NULL;
    p->op = NULL;
    p->atomLast = parseAtom(p);      // ��R DECL
    while (isNext(p, ".|->")) { // ���_���o (.|->) ATOM
        p->op = next(p, ".|->");
        p->atomLast = parseAtom(p);
    }
    Tree *path = pop(p, PATH); // ���X PATH ���y�k��C
    SemAtom *semLast = p->atomLast->sem;
    PTypeCopy(&sem->ptype, &semLast->ptype);
    return path;
}

// �y�k�GATOM = ID (([ EXP ])* |( EXP_LIST? ))
// �\��G��R ATOM �ëإ߻y�k��
// �d�ҡGTree *atom = parseAtom(p);
Tree* parseAtom(Parser *p) {
    SemAtom *sem=push(p, ATOM, SemAtom); // �إ� ATOM ���y�k��λy�N���c
    Symbol *sym = NULL;    
    sem->id = next(p, ID); // ���o ID
    char *id = token(sem->id);
    
    if (p->atomLast == NULL) {
       sym = SymbolLookup(p, id);
    } else {
       SemAtom *semLast = p->atomLast->sem;
       ASSERT(semLast->ptype.typeSym->symType == SymStruct);
       sym = SymTableGet(p->symTable, semLast->ptype.typeSym, id);
    }
    SymDebug(sym);
    
    if (isNext(p, "(")) { // �p�G���U�ӬO (�A�h���ӬO��ƩI�s ID ( EXP_LIST? )
       sem->subTag = CALL;
       next(p, "(");
       if (!isNext(p, ")"))
           sem->expList = parseExpList(p);
       next(p, ")");
       Method *method = sym->typePtr;
       PTypeCopy(&sem->ptype, &method->ret);
    } else if (isNext(p, "[")) { // �p�G���U�ӬO [�A�h���ӬO�}�C�ŧi ID ([ EXP ])*
        sem->subTag = ARRAY_MEMBER;
        int idxCount = 0;
        while (isNext(p, "[")) {
            next(p, "[");
            Tree *exp = parseExp(p);
            next(p, "]");
            idxCount ++;
        }
        PTypeCopy(&sem->ptype, sym->typePtr);
        sem->ptype.dimCount -= idxCount;
    } else { // �u�� ID 
        sem->subTag = ID;
        PTypeCopy(&sem->ptype, sym->typePtr);
    }
    Tree *atom = pop(p, ATOM); // ���X ATOM ���y�k��C
   return atom;
}

// �y�k�GPARAM = TYPE VAR
// �\��G��R PARAM �ëإ߻y�k��
// �d�ҡGTree *param = parseParam(p); 
Tree* parseParam(Parser *p) {
	SemParam *sem = push(p, PARAM, SemParam);// �إ� PARAM ���y�k��λy�N���c
	sem->type = parseEType(p); // ��R TYPE
	sem->var = parseVar(p); // ��R VAR
	return pop(p, PARAM); // ���X PARAM ���y�k��C
}

// �y�k�GDECL = TYPE VAR_LIST
// �\��G�P�_�쩳���U�ӬO�_�� DECL�A�O���ܶǦ^ TRUE�A�_�h�Ǧ^ FALSE 
//       ����Ʒ|�V�e���ݡA�p�G�o�{�O (int|byte|char|float|ID)** ID�A���N�O DECL
// �d�ҡGif (isDecl(p)) parseDecl(p);
BOOL isDecl(Parser *p) {
    BOOL rzFlag = TRUE;
    Scanner *s = p->scanner;                // s=���y�� 
    ScannerStore(s);                        // �x�s���y�����A 
    if (isNext(p, "int|byte|char|float|ID"))// ���� TYPE
        ScannerNext(s);                     // ���L TYPE
    else 
        rzFlag=FALSE;
    while (isNext(p, "*")) ScannerNext(s);  // ���ݨò��L�P�� 
    if (!isNext(p, ID)) rzFlag=FALSE;       // ���� ID
    ScannerRestore(s);                      // ��_���y�����A�C 
    return rzFlag;
}

// �y�k�GDECL = TYPE VAR_LIST
// �\��G��R PROG �ëإ߻y�k�� 
// �d�ҡGTree *decl = parseDecl(p);
Tree* parseDecl(Parser *p) {
	SemDecl *sem = push(p, DECL, SemDecl);// �إ� DECL ���y�k��λy�N���c
	sem->type = parseEType(p); // ��R ETYPE
	sem->varList = parseVarList(p); // ��R VAR_LIST
	return pop(p, DECL); // ���X DECL ���y�k��C
}

// �y�k�GETYPE = (int | byte | char | float | ID) // ID is STRUCT_TYPE
// �\��G��R ETYPE �ëإ߻y�k��
// �d�ҡGTree *etype = parseEType(p); 
Tree* parseEType(Parser *p) {
    SemEType *sem=push(p, ETYPE, SemEType);// �إ� TYPE ���y�k��λy�N���c
    Tree *etype = next(p, "int|byte|char|float|ID");  // ���o (int | byte | char | float | ID)
    char *etypeName = token(etype); // ���o���A�W�� 
   	p->ptype.typeSym = SymTableGet(p->symTable, Global, etypeName); // �q�Ÿ����d�X�ӫ��A���Ÿ�
   	ASSERT(p->ptype.typeSym != NULL);
    return pop(p, ETYPE); // ���X TYPE ���y�k��C
}

// �y�k�GVAR = ** ID ([ CINT ])* (= EXP)?
// �\��G��R VAR �ëإ߻y�k��
// �d�ҡGTree *var = parseVar(p); 
Tree* parseVar(Parser *p) {
	SemVar *sem = push(p, VAR, SemVar);	// �إ� VAR ���y�k��λy�N���c
	int starCount = 0; // �P���ƶq��l�Ȭ� 0 
    while (isNext(p, "*")) { // ��R ** 
        next(p, "*"); // ���o�P�� 
        starCount ++; // �p��P���ƶq 
    }
	sem->id = next(p, ID); // ��R ID
	
	// �إ� ID ���Ÿ��O�� Symbol(id, SymVar)
	Symbol *pblock = peekBlock(p); // ���o���϶��Ÿ�
	char *id = token(sem->id); // ���o�ܼƦW�� 
	Symbol *sym = SymNew(pblock, id, SymVar); // �إ��ܼƲŸ� 
	PType *ptype = sym->typePtr; // ���X�ܼƵ��c 
    ptype->starCount = starCount; // �]�w�ܼƵ��c�����P���ƶq 
    ptype->typeSym = p->ptype.typeSym; // �]�w�ܼƵ��c�����Ÿ� 
    int typeSize = 0;
    if (starCount > 0)
       typeSize = PTR_SIZE;
    else
       typeSize = ptype->typeSym->size;
	ptype->dimCount = 0;  // �]�w�ܼƵ��c�����}�C����
	SymTablePut(p->symTable, sym); // �N�ܼƥ[�J�Ÿ��� 
    
    int cells = 1;
    while (isNext(p, "[")) { // ��R ([ CINT ])*
        next(p, "[");
        Tree *cint = next(p, "CINT");
        ASSERT(ptype->dimCount<DIM_MAX);
        int dim = atoi(token(cint));
        cells = cells * dim;
        ptype->dim[ptype->dimCount++] = dim;
        next(p, "]");
    }
    sym->size = typeSize * cells;

    if (pblock->symType == SymStruct) { // �p�G���϶��O Struct
        Struct *stru = pblock->typePtr;
        ArrayAdd(stru->fields, sym); // �N�N���ܼƥ[�J Struct ����줤�C 
    } else if (pblock->symType == SymMethod) {
        Method *method = pblock->typePtr;
        ArrayAdd(method->params, sym); // �N�N���ܼƥ[�J Struct ����줤�C         
    } else if (pblock->symType == SymBlock) {
        Block *block = pblock->typePtr;
        ArrayAdd(block->vars, sym); // �N�N���ܼƥ[�J Struct ����줤�C 
    }
    ptype->offset = pblock->size;
    pblock->size += sym->size;
    
	if (isNext(p, "=")) { // ��R (= EXP)?
    next(p, "=");
		sem->exp = parseExp(p);
  }
	return pop(p, VAR);  // ���X VAR ���y�k��C
}

// �y�k�GEXP = TERM (OP2 TERM)?
// �\��G��R EXP �ëإ߻y�k��
// �d�ҡGTree *exp = parseExp(p); 
Tree* parseExp(Parser *p) {
	SemExp *sem = push(p, EXP, SemExp);// �إ� EXP ���y�k��λy�N���c
	sem->term1 = parseTerm(p); // ��R TERM 
	if (isNext(p, SET_OP2)) { // �p�G���U�ӬO OP2 �A�h��R (OP2 TERM)?
		sem->op = next(p, SET_OP2);
		sem->term2 = parseTerm(p);
  }
	Tree *exp = pop(p, EXP); // ���X EXP ���y�k��C
    SemTerm *semTerm1 = sem->term1->sem;
    ObjCopy(&sem->ptype, &semTerm1->ptype, PType);
	return exp;
}

// �y�k�GTERM = ( EXP (OP2 EXP)? ) | INTEGER | FLOAT | STRING | PATH
// �\��G��R TERM �ëإ߻y�k��
// �d�ҡGTree *term = parseTerm(p); 
Tree* parseTerm(Parser *p) {
	SemTerm *sem = push(p, TERM, SemTerm);// �إ� TERM ���y�k��λy�N���c
  if (isNext(p, "(")) {
        sem->subTag = EXP;
    next(p, "(");
		sem->exp1 = parseExp(p);
	if (!isNext(p, ")")) {
			next(p, SET_OP2);
			sem->exp2 = parseExp(p);
	}
    next(p, ")");
		SemExp *semExp1 = sem->exp1->sem;
        ObjCopy(&sem->ptype, &semExp1->ptype, PType);
	} else if (isNext(p, "CINT")) {
        sem->subTag = CINT;
	    next(p, "CINT");
        sem->ptype.typeSym = Int;
	} else if (isNext(p, "CSTR")) {
        sem->subTag = CSTR;
	    next(p, "CSTR");
        sem->ptype.typeSym = Char;
        sem->ptype.starCount = 1;
	} else if (isNext(p, "CFLOAT")) {
        sem->subTag = CFLOAT;
	    next(p, "CFLOAT");
        sem->ptype.typeSym = Float;
	} else {
        sem->subTag = PATH;
        sem->path = parsePath(p);
		SemPath *semPath = sem->path->sem;
        ObjCopy(&sem->ptype, &semPath->ptype, PType);
    }
    Tree *term = pop(p, TERM); // ���X TERM ���y�k��C
    return term;
}

// �y�k�GVAR_LIST = VAR (, VAR)*
// �\��G��R VarList �ëإ߻y�k��
// �d�ҡGTree *varList = parseVarList(p); 
Tree* parseVarList(Parser *p) {
	SemVarList *sem = push(p, VAR_LIST, SemVarList);// �إ� VAR_LIST ���y�k��λy�N���c
	parseVar(p);
  while (isNext(p, ",")) {
    next(p, ",");
		parseVar(p);
  }
	return pop(p, VAR_LIST); // ���X VAR_LIST ���y�k��C
}

// �y�k�GEXP_LIST = EXP (, EXP)*
// �\��G��R EXP_LIST �ëإ߻y�k��
// �d�ҡGTree *expList = parseExpList(p); 
Tree* parseExpList(Parser *p) {
	SemExpList *sem = push(p, EXP_LIST, SemExpList);// �إ� EXP_LIST ���y�k��λy�N���c
	parseExp(p);
  while (isNext(p, ",")) {
    next(p, ",");
		parseExp(p);
  }
	return pop(p, EXP_LIST); // ���X EXP_LIST ���y�k��C
}

// �y�k�GDECL_LIST = DECL (; DECL)*
// �\��G��R DECL_LIST �ëإ߻y�k��
// �d�ҡGTree *declList = parseDeclList(p); 
Tree* parseDeclList(Parser *p) {
	SemDeclList *sem=push(p, DECL_LIST, SemDeclList);// �إ� DECL_LIST ���y�k��λy�N���c
  parseDecl(p);
	while (isNext(p, ";")) {
		next(p, ";");
    parseDecl(p);
  }
	return pop(p, DECL_LIST); // ���X DECL_LIST ���y�k��C
}

// �y�k�GPARAM_LIST = PARAM (, PARAM)*
// �\��G��R PARAM_LIST �ëإ߻y�k��
// �d�ҡGTree *paramList = parseParamList(p); 
Tree* parseParamList(Parser *p) {
	SemParamList *sem=push(p, PARAM_LIST, SemParamList);// �إ� PARAM_LIST ���y�k��λy�N���c
	parseParam(p);
	while (isNext(p, ";")) {
		next(p, ";");
   		parseParam(p);
	}
	return pop(p, PARAM_LIST); // ���X PARAM_LIST ���y�k��C
}

// ========================== Library ====================================
char* level(Parser *p) {
	return strFill(p->spaces, ' ', p->nodeStack->count);
}

BOOL isNext(Parser *p, char *tags) { // �ˬd�U�@�ӵ��J�����A
    Scanner *s = p->scanner;
	char tTags[MAX_LEN+1];
	sprintf(tTags, "|%s|", tags);
	if (strPartOf(s->tag, tTags))
    return TRUE;
  else
    return FALSE;
}

Tree *next(Parser *p, char *tags) { // �ˬd�U�@�ӵ��J�����A
    Scanner *s = p->scanner;
	if (isNext(p, tags)) {            // �p�G�OpTypes���A���@
	    Tree *child = TreeNew(s->tag);
        child->sem = strNew(s->token);  //   �إߵ��J�`�I(token,type)
	    Tree *parentTree = ArrayPeek(p->nodeStack);                   //   ���o���`�I�A
    TreeAddChild(parentTree, child);                          //   �[�J���`�I�����l��
		if (strEqual(s->tag, s->token))
		   debug("%s KEY:%s\n", level(p), s->tag);
		else
	       debug("%s %s:%s\n", level(p), s->tag, s->token);
        ScannerNext(s);
		return child;                                             //   �Ǧ^�ӵ��J
  } else {                                                    // �_�h(�U�@�Ӹ`�I���A���~)
		debug("next():token=%s, tag=%s is not in tag(%s)\n", s->token, s->tag, tags);     //   �L�X���~�T��
	ERROR();
    return NULL;
  }
}

Tree* push1(Parser *p, char* tag) { // �إ� pType ���A���l��A���J���|��
	debug("%s+%s\n", level(p), tag);
	Tree *node = TreeNew(tag);
	ArrayPush(p->nodeStack, node);
	return node;
}

Tree* pop(Parser *p, char* tag) { // ���X pTag �аO���l��
	Tree *tree = ArrayPop(p->nodeStack); // ���o���|�̤W�h���l��
	debug("%s-%s\n", level(p), tree->tag); // �L�X�H�K�[��
	if (strcmp(tree->tag, tag)!=0) {  // �p�G���A���ŦX
		debug("pop(%s):should be %s\n",tree->tag, tag); //  �L�X���~�T��
	ERROR();
  }
	if (p->nodeStack->count > 0) { // �p�G���|���O�Ū�
	  Tree *parentTree = ArrayPeek(p->nodeStack); //  ���X�W�@�h��R��
	  TreeAddChild(parentTree, tree);  //  �N�غc��������R�𱾨��W�A�����l��
  }
  return tree;
}

char *token(Tree *node) {
    return (char*) node->sem;
}

void pushBlock(Parser *p, Symbol *sym) {
    ArrayPush(p->blockStack, sym);
}

Symbol* SymbolLookup(Parser *p, char *name) {
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
