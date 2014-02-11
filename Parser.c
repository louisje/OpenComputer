#include "Parser.h"

void ParserTest(char *fileName) {
	debug("=======ParserTest()==========\n");
    SymTable *symTable = SymTableNew();
    char *text = fileToStr(fileName);
    Tree *tree = parse(text);
    TreeFree(tree);
    strFree(text);
    SymTableFree(symTable);
    memCheck(); // �ˬd�O����     
}

Tree *parse(char *text) {    // ��R�����D�n���
	Scanner *scanner = ScannerNew(text);
	Parser *p=ParserNew(scanner);              // �إ߭�R��
//	ParserParse(p, text);                      // �}�l��R
	Tree *tree = p->tree;
	ParserFree(p);
	ScannerFree(scanner);
	return tree;                            // �Ǧ^��R��
}

Parser *ParserNew(Scanner *scanner) {
	Parser *p = ObjNew(Parser, 1);
	p->stack = ArrayNew(10);
	p->blockStack = ArrayNew(10);
	p->scanner = scanner;
    ScannerNext(scanner);
	return p;
}

void ParserFree(Parser *p) {
	ArrayFree(p->blockStack, NULL);
	ArrayFree(p->stack, NULL);
	ObjFree(p);
}

void ParserParse(Parser *p, char *text) {                 // ��R���󪺥D���
	debug("======= parsing ========\n");
	p->tree = parseProg(p);                                 // �}�l��R PROG = BaseList
  if (p->stack->count != 0) {                             // �p�G��R��������|�O�Ū��A���N�O��R���\
    debug("parse fail:stack.count=%d", p->stack->count); //   �_�h�N���ܿ��~�T��
	ERROR();
  }
}

// �p�G�}�Y�O TYPE ID(�A�]�N�O  (int|byte|char|float|ID)** ID(�A���N�O Method�C 
BOOL isMethod(Parser *p) {
    BOOL rzFlag = TRUE;
    Scanner *s = p->scanner;
    ScannerStore(s);
    if (isNext(p, "int|byte|char|float|ID")) ScannerNext(s); else rzFlag=FALSE;
    while (isNext(p, "*")) ScannerNext(s);
    if (isNext(p, ID)) ScannerNext(s); else rzFlag=FALSE;
    if (!isNext(p, "(")) rzFlag=FALSE;
    ScannerRestore(s);
    return rzFlag;
}

// PROG = (STRUCT | METHOD | DECL ; )*
Tree *parseProg(Parser *p) {             // ��R PROG=BaseList �W�h
	SemProg *sem=push(p, PROG, SemProg); // �إ� BaseList �����
	p->block = BlockNew(&Global, "", SymBlock);
	ArrayPush(p->blockStack, p->block);
	while (!isEnd(p)) {     //  ��R BASE�A����{�������θI�� } ����
	    if (isNext(p, "struct"))
	       parseStruct(p);
	    else {
           if (isMethod(p)) { // METHOD = TYPE ID(DECL_LIST?) BLOCK
                parseMethod(p);
           } else { // �_�h�N�����O DECL ; 
                parseDecl(p);
	            next(p, ";");
          }
        }
    }
    p->block = ArrayPop(p->blockStack, (FuncPtr1) BlockFree);
	return pop(p, PROG);                     // ���X BaseList ����R��
}                                           // ���X PROG ����R��

// BASE = IF | FOR | WHILE | BLOCK | STMT ;
Tree* parseBase(Parser *p) {                 // ��R BASE �W�h
	SemBase *sem=push(p, BASE, SemBase);                          // �إ� BASE ����R��
  if (isNext(p, "if"))                      // �p�G�U�@�ӵ��J�O if
    parseIf(p);                             //  �ھ� if �W�h�i���R
  else if (isNext(p, "for"))                // �p�G�U�@�ӵ��J�O for
    parseFor(p);                            //  �ھ� FOR �W�h�i���R
  else if (isNext(p, "while"))                // �p�G�U�@�ӵ��J�O for
    parseWhile(p);                            //  �ھ� FOR �W�h�i���R
  else if (isNext(p, "{"))
    parseBlock(p);
  else {                                     // �_�h
    parseStmt(p);                           //  �ھ� STMT �W�h�i���R
	next(p, ";");
  }
	return pop(p, BASE);                           // ���X BASE ����R��
}

// BLOCK = { BASE* }
Tree* parseBlock(Parser *p) {
	SemBlock *sem=push(p, BLOCK, SemBlock);
	p->block = BlockNew(p->block, "", SymBlock);
	ArrayPush(p->blockStack, p->block);
  next(p, "{");
	while (!isNext(p, "}")) 
        parseBase(p);
  next(p, "}");
    p->block = ArrayPop(p->blockStack, (FuncPtr1) BlockFree);
	return pop(p, BLOCK);
}

// METHOD = TYPE **ID (PARAM_LIST?) BLOCK
Tree* parseMethod(Parser *p) {
	SemMethod *sem=push(p, METHOD, SemMethod);
	sem->type=parseType(p); // ��R TYPE

	// ��R ** (n �ӬP��, n>=0)
	int starCount = 0; // ���ݬݦ��X�ӬP��
    while (isNext(p, "*")) { 
        next(p, "*");
        starCount ++;
    }
    sem->id = next(p, ID); // ��R ID
    
	char *id = token(sem->id);
	
	// �إ� ID ���Ÿ��O�� Symbol(id, METHOD) 
//	Symbol *sym = SymNew(&Global, id, METHOD);

	// �إ� Method ���c 
	Method *method = MethodNew();
	method->ret.typeName = p->decl.typeName;
	method->ret.starCount = p->decl.starCount;
//	SymTablePut(p->symTable, sym);
	
	// �إ߷s�϶��ñ��J���| 
	p->block = BlockNew(p->block, id, SymMethod);
	ArrayPush(p->blockStack, p->block);
	
    // ��R�ѼƳ��� (PARAM_LIST?) 
  next(p, "(");
	if (!isNext(p, ")")) // ��R PARAM_LIST?
		sem->paramList = parseParamList(p); 
  next(p, ")");
	sem->block = parseBlock(p); // ��R BLOCK
    p->block = ArrayPop(p->blockStack, (FuncPtr1) BlockFree);
	return pop(p, METHOD);
}

// STRUCT = struct ID { (DECL ;)* }
Tree* parseStruct(Parser *p) {
	SemStruct *sem=push(p, STRUCT, SemStruct);

	next(p, "struct"); // ��R struct 
	sem->id = next(p, ID); // ��R ID
	char *id = token(sem->id);
	
	// �إ� ID ���Ÿ��O�� Symbol(id, METHOD) 
	Symbol *sym = SymNew(&Global, id, SymStruct);
//	SymTablePut(p->symTable, sym);
	
	// �إ߷s�϶��ñ��J���| 
	p->block = BlockNew(p->block, id, SymStruct);
	ArrayPush(p->blockStack, p->block);
	
	// ��R { (DECL ;)* }
	next(p, "{"); 
	while (!isNext(p, "}")) {
        parseDecl(p);
        next(p, ";");
    }
	next(p, "}");
	return pop(p, STRUCT);
}

// FOR = for (STMT ; EXP ; STMT) BASE
Tree* parseFor(Parser *p) {                  
	SemFor *sem=push(p, FOR, SemFor);                           // �إ� FOR �����
  next(p, "for");                           // ���o for
  next(p, "(");                             // ���o (
	sem->stmt1 = parseStmt(p);                             // ��R STMT
  next(p, ";");                             // ���o ;
	sem->exp = parseExp(p);                              // ��R EXP
  next(p, ";");                             // ���o ;
	sem->stmt2 = parseStmt(p);                             // ��R STMT
  next(p, ")");                             // ���o )
  parseBase(p);                             // ��R BASE
	return pop(p, FOR);                            // ���X FOR ����R��
}

// IF = if (EXP) BASE (else BASE)?
Tree* parseIf(Parser *p) {
	SemIf *sem=push(p, IF, SemIf);
	next(p, "if");
	next(p, "(");
	sem->exp = parseExp(p);
	next(p, ")");
	sem->base1 = parseBase(p);
  if (isNext(p, "else")) {
    next(p, "else");
		sem->base2 = parseBase(p);
  }
	return pop(p, IF);
}

// WHILE = while (EXP) BASE
Tree* parseWhile(Parser *p) {                  // �إ� FOR �����
	SemWhile *sem=push(p, WHILE, SemWhile);                           // ���o for
  next(p, "while");                           // ���o (
  next(p, "(");                             // ��R STMT
	sem->exp = parseExp(p);                             // ���o ;
  next(p, ")");                             // ��R BASE
	sem->base = parseBase(p);                            // ���X FOR ����R��
	return pop(p, WHILE);
}

// �p�G�}�Y�O TYPE ID�A�]�N�O  (int|byte|char|float|ID)** ID�A���N�O DECL�C 
BOOL isDecl(Parser *p) {
    BOOL rzFlag = TRUE;
    Scanner *s = p->scanner;
    ScannerStore(s);
    if (isNext(p, "int|byte|char|float|ID")) ScannerNext(s); else rzFlag=FALSE;
    while (isNext(p, "*")) ScannerNext(s);
    if (!isNext(p, ID)) rzFlag=FALSE;
    ScannerRestore(s);
    return rzFlag;
}

// STMT = return EXP | DECL | PATH (EXP_LIST) | PATH = EXP | PATH OP1
Tree* parseStmt(Parser *p) {
	SemStmt *sem=push(p, STMT, SemStmt);
  if (isNext(p, "return")) {
    next(p, "return");
		sem->exp = parseExp(p);
	} else if (isDecl(p)) { // (isNext(p, SET_BTYPE)) { // DECL_LIST ���Ĥ@�Ӭ� TYPE
		sem->decl = parseDecl(p);
	} else {
        sem->path = parsePath(p);
	if (isNext(p, "(")) {
      next(p, "(");
			sem->expList = parseExpList(p);
	  next(p, ")");
	} else if (isNext(p, "=")) {
	  next(p, "=");
			sem->exp = parseExp(p);
		} else if (isNext(p, SET_OP1)) {
			next(p, SET_OP1);
	} else
	  ERROR();
  }
	return pop(p, STMT);
}

// PATH = ATOM (.|->) ATOM)*
Tree* parsePath(Parser *p) {
    SemPath *sem=push(p, PATH, SemPath);
    parseAtom(p);
    while (isNext(p, ".|->")) {
        next(p, ".|->");
        parseAtom(p);
    }
    return pop(p, PATH);
}

// ID (([ EXP ])* |( EXP_LIST? ))
Tree* parseAtom(Parser *p) {
    SemAtom *sem=push(p, ATOM, SemAtom);
    sem->id = next(p, ID);
    sem->subTag = ID;
    if (isNext(p, "(")) {
       next(p, "(");
       if (!isNext(p, ")"))
           sem->expList = parseExpList(p);
       next(p, ")");
       sem->subTag = CALL;
    } else if (isNext(p, "[")) {
        sem->subTag = ARRAY_MEMBER;
        Array *exps = ArrayNew(2);
        while (isNext(p, "[")) {
            next(p, "[");
            Tree *exp = parseExp(p);
            ArrayAdd(exps, exp);
            next(p, "]");
        }
    }
    return pop(p, ATOM);
}

// PARAM = TYPE VAR
Tree* parseParam(Parser *p) {
	SemParam *sem = push(p, PARAM, SemParam);
	sem->type = parseType(p);
	sem->var = parseVar(p);
	return pop(p, PARAM);  
}

// DECL = TYPE VAR_LIST
Tree* parseDecl(Parser *p) {
	SemDecl *sem = push(p, DECL, SemDecl);
	sem->type = parseType(p);
	sem->varList = parseVarList(p);
	return pop(p, DECL);  
}

// TYPE = (int | byte | char | float | ID) // ID is STRUCT_TYPE
Tree* parseType(Parser *p) {
    SemType *sem=push(p, TYPE, SemType);
    Tree *type = next(p, "int|byte|char|float|ID");
//    char *typeName = token(type);
//    debug("typeName=%s\n", typeName);
//    Symbol *sym = SymTableGet(p->symTable, &Global, typeName);
//    SymDebug(sym);
    p->decl.typeName = token(type);
//    sem->type = token(type);
    return pop(p, TYPE);
}

// VAR = ** ID ([ CINT ])* (= EXP)?
Tree* parseVar(Parser *p) {
	SemVar *sem = push(p, VAR, SemVar);	
	int starCount = 0;
    while (isNext(p, "*")) { // ��R ** 
        next(p, "*");
        starCount ++;
    }
	sem->id = next(p, ID); // ��R ID
	char *id = token(sem->id);
	
	// �إ� ID ���Ÿ��O�� Symbol(id, DECL)
//	Symbol *sym = SymNew(p->block, id, DECL);
	Var *var = ObjNew(Var, 1);
//	sym->u.decl = decl;
//	SymDebug(sym);
	var->typeName = p->decl.typeName;
	var->dimCount = 0;
    while (isNext(p, "[")) { // ��R ([ CINT ])*
        next(p, "[");
        Tree *cint = next(p, "CINT");
        ASSERT(var->dimCount<DIM_MAX);
        var->dim[var->dimCount++] = atoi(token(cint));
        next(p, "]");
    }

    if (p->block->symType == SymStruct) {
//        Struct *pStruct = p->block->type.pStruct;
//        ArrayAdd(pStruct->fields, sym);
    }
    
//	SymTablePut(p->symTable, sym);
    
  if (isNext(p, "=")) {
    next(p, "=");
		sem->exp = parseExp(p);
  }
	return pop(p, VAR);  
}

// EXP = TERM (OP2 TERM)?
Tree* parseExp(Parser *p) {
	SemExp *sem = push(p, EXP, SemExp);
	sem->term1 = parseTerm(p);
	if (isNext(p, SET_OP2)) {
		sem->op = next(p, SET_OP2);
		sem->term2 = parseTerm(p);
  }
	return pop(p, EXP);
}

// TERM = ( EXP (OP2 EXP)? ) | INTEGER | FLOAT | STRING | PATH
Tree* parseTerm(Parser *p) {
	SemTerm *sem = push(p, TERM, SemTerm);
  if (isNext(p, "(")) {
    next(p, "(");
		sem->exp1 = parseExp(p);
	if (!isNext(p, ")")) {
			next(p, SET_OP2);
			sem->exp2 = parseExp(p);
	}
    next(p, ")");
	} else if (isNext(p, "CINT|CFLOAT|CSTR")) {
	    next(p, "CINT|CFLOAT|CSTR");
	} else
        parsePath(p);
	return pop(p, TERM);
}

// VAR_LIST = VAR (, VAR)*
Tree* parseVarList(Parser *p) {
	SemVarList *sem = push(p, VAR_LIST, SemVarList);
	parseVar(p);
  while (isNext(p, ",")) {
    next(p, ",");
		parseVar(p);
  }
	return pop(p, VAR_LIST);  
}

// EXP_LIST = EXP (, EXP)*
Tree* parseExpList(Parser *p) {
	SemExpList *sem = push(p, EXP_LIST, SemExpList);
	parseExp(p);
  while (isNext(p, ",")) {
    next(p, ",");
		parseExp(p);
  }
	return pop(p, EXP_LIST);  
}

// DECL_LIST = DECL (; DECL)*
Tree* parseDeclList(Parser *p) {
	SemDeclList *sem=push(p, DECL_LIST, SemDeclList);
  parseDecl(p);
	while (isNext(p, ";")) {
		next(p, ";");
    parseDecl(p);
  }
	return pop(p, DECL_LIST);
}

// PARAM_LIST = PARAM (, PARAM)*
Tree* parseParamList(Parser *p) {
	SemParamList *sem=push(p, PARAM_LIST, SemParamList);
	parseParam(p);
	while (isNext(p, ";")) {
		next(p, ";");
   		parseParam(p);
	}
	return pop(p, PARAM_LIST);
}

// ========================== Library ====================================
char* level(Parser *p) {
	return strFill(p->spaces, ' ', p->stack->count);
}

BOOL isEnd(Parser *p) {
	return isNext(p, kEND);
}

BOOL isNext(Parser *p, char *pTags) {                        // �ˬd�U�@�ӵ��J�����A
    Scanner *s = p->scanner;
	char tTags[MAX_LEN+1];
	sprintf(tTags, "|%s|", pTags);
	if (strPartOf(s->tag, tTags))
    return TRUE;
  else
    return FALSE;
}

Tree *TokenNodeNew(char *token, char *tag) {
    Tree *node = TreeNew(tag);  //   �إߵ��J�`�I(token,type)
    SemToken *sem = ObjNew(SemToken, 1);
    sem->token = strNew(token);
    node->sem = sem;
    return node;
}

Tree *next(Parser *p, char *pTags) {                         // �ˬd�U�@�ӵ��J�����A
    Scanner *s = p->scanner;
	if (isNext(p, pTags)) {                                    // �p�G�OpTypes���A���@
	    Tree *child = TokenNodeNew(s->token, s->tag);  //   �إߵ��J�`�I(token,type)
    Tree *parentTree = ArrayPeek(p->stack);                   //   ���o���`�I�A
    TreeAddChild(parentTree, child);                          //   �[�J���`�I�����l��
		if (strEqual(s->tag, s->token))
		   debug("%s KEY:%s\n", level(p), s->tag);
		else
	       debug("%s %s:%s\n", level(p), s->tag, s->token);
        ScannerNext(s);
		return child;                                             //   �Ǧ^�ӵ��J
  } else {                                                    // �_�h(�U�@�Ӹ`�I���A���~)
		debug("next():token=%s, tag=%s is not in tag(%s)\n", s->token, s->tag, pTags);     //   �L�X���~�T��
	ERROR();
    return NULL;
  }
}

Tree* push1(Parser *p, char* pTag) { // �إ� pType ���A���l��A���J���|��
	debug("%s+%s\n", level(p), pTag);
//	p->node = TreeNew(pTag);
//	ArrayPush(p->stack, p->node);
//	return p->node;
	Tree *node = TreeNew(pTag);
	ArrayPush(p->stack, node);
	return node;
}

Tree* pop(Parser *p, char* pTag) { // ���X pTag �аO���l��
	Tree *tree = ArrayPop(p->stack, NULL); // ���o���|�̤W�h���l��
	debug("%s-%s\n", level(p), tree->tag); // �L�X�H�K�[��
	if (strcmp(tree->tag, pTag)!=0) {  // �p�G���A���ŦX
		debug("pop(%s):should be %s\n",tree->tag, pTag); //  �L�X���~�T��
	ERROR();
  }
  if (p->stack->count > 0) {                                  // �p�G���|���O�Ū�
    Tree *parentTree = ArrayPeek(p->stack);                   //  ���X�W�@�h��R��
	  TreeAddChild(parentTree, tree);  //  �N�غc��������R�𱾨��W�A�����l��
  }
  return tree;
}
