#include "Parser.h"

void ParserTest(char *fileName) {
	debug("=======ParserTest()==========\n");
    SymTable *symTable = SymTableNew();
    char *text = fileToStr(fileName);
    Tree *tree = parse(text);
    TreeFree(tree);
    strFree(text);
    SymTableFree(symTable);
    memCheck(); // 檢查記憶體     
}

Tree *parse(char *text) {    // 剖析器的主要函數
	Scanner *scanner = ScannerNew(text);
	Parser *p=ParserNew(scanner);              // 建立剖析器
//	ParserParse(p, text);                      // 開始剖析
	Tree *tree = p->tree;
	ParserFree(p);
	ScannerFree(scanner);
	return tree;                            // 傳回剖析器
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

void ParserParse(Parser *p, char *text) {                 // 剖析物件的主函數
	debug("======= parsing ========\n");
	p->tree = parseProg(p);                                 // 開始剖析 PROG = BaseList
  if (p->stack->count != 0) {                             // 如果剖析完成後堆疊是空的，那就是剖析成功
    debug("parse fail:stack.count=%d", p->stack->count); //   否則就提示錯誤訊息
	ERROR();
  }
}

// 如果開頭是 TYPE ID(，也就是  (int|byte|char|float|ID)** ID(，那就是 Method。 
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
Tree *parseProg(Parser *p) {             // 剖析 PROG=BaseList 規則
	SemProg *sem=push(p, PROG, SemProg); // 建立 BaseList 的樹根
	p->block = BlockNew(&Global, "", SymBlock);
	ArrayPush(p->blockStack, p->block);
	while (!isEnd(p)) {     //  剖析 BASE，直到程式結束或碰到 } 為止
	    if (isNext(p, "struct"))
	       parseStruct(p);
	    else {
           if (isMethod(p)) { // METHOD = TYPE ID(DECL_LIST?) BLOCK
                parseMethod(p);
           } else { // 否則就必須是 DECL ; 
                parseDecl(p);
	            next(p, ";");
          }
        }
    }
    p->block = ArrayPop(p->blockStack, (FuncPtr1) BlockFree);
	return pop(p, PROG);                     // 取出 BaseList 的剖析樹
}                                           // 取出 PROG 的剖析樹

// BASE = IF | FOR | WHILE | BLOCK | STMT ;
Tree* parseBase(Parser *p) {                 // 剖析 BASE 規則
	SemBase *sem=push(p, BASE, SemBase);                          // 建立 BASE 的剖析樹
  if (isNext(p, "if"))                      // 如果下一個詞彙是 if
    parseIf(p);                             //  根據 if 規則進行剖析
  else if (isNext(p, "for"))                // 如果下一個詞彙是 for
    parseFor(p);                            //  根據 FOR 規則進行剖析
  else if (isNext(p, "while"))                // 如果下一個詞彙是 for
    parseWhile(p);                            //  根據 FOR 規則進行剖析
  else if (isNext(p, "{"))
    parseBlock(p);
  else {                                     // 否則
    parseStmt(p);                           //  根據 STMT 規則進行剖析
	next(p, ";");
  }
	return pop(p, BASE);                           // 取出 BASE 的剖析樹
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
	sem->type=parseType(p); // 剖析 TYPE

	// 剖析 ** (n 個星號, n>=0)
	int starCount = 0; // 數屬看有幾個星號
    while (isNext(p, "*")) { 
        next(p, "*");
        starCount ++;
    }
    sem->id = next(p, ID); // 剖析 ID
    
	char *id = token(sem->id);
	
	// 建立 ID 的符號記錄 Symbol(id, METHOD) 
//	Symbol *sym = SymNew(&Global, id, METHOD);

	// 建立 Method 結構 
	Method *method = MethodNew();
	method->ret.typeName = p->decl.typeName;
	method->ret.starCount = p->decl.starCount;
//	SymTablePut(p->symTable, sym);
	
	// 建立新區塊並推入堆疊 
	p->block = BlockNew(p->block, id, SymMethod);
	ArrayPush(p->blockStack, p->block);
	
    // 剖析參數部分 (PARAM_LIST?) 
  next(p, "(");
	if (!isNext(p, ")")) // 剖析 PARAM_LIST?
		sem->paramList = parseParamList(p); 
  next(p, ")");
	sem->block = parseBlock(p); // 剖析 BLOCK
    p->block = ArrayPop(p->blockStack, (FuncPtr1) BlockFree);
	return pop(p, METHOD);
}

// STRUCT = struct ID { (DECL ;)* }
Tree* parseStruct(Parser *p) {
	SemStruct *sem=push(p, STRUCT, SemStruct);

	next(p, "struct"); // 剖析 struct 
	sem->id = next(p, ID); // 剖析 ID
	char *id = token(sem->id);
	
	// 建立 ID 的符號記錄 Symbol(id, METHOD) 
	Symbol *sym = SymNew(&Global, id, SymStruct);
//	SymTablePut(p->symTable, sym);
	
	// 建立新區塊並推入堆疊 
	p->block = BlockNew(p->block, id, SymStruct);
	ArrayPush(p->blockStack, p->block);
	
	// 剖析 { (DECL ;)* }
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
	SemFor *sem=push(p, FOR, SemFor);                           // 建立 FOR 的樹根
  next(p, "for");                           // 取得 for
  next(p, "(");                             // 取得 (
	sem->stmt1 = parseStmt(p);                             // 剖析 STMT
  next(p, ";");                             // 取得 ;
	sem->exp = parseExp(p);                              // 剖析 EXP
  next(p, ";");                             // 取得 ;
	sem->stmt2 = parseStmt(p);                             // 剖析 STMT
  next(p, ")");                             // 取得 )
  parseBase(p);                             // 剖析 BASE
	return pop(p, FOR);                            // 取出 FOR 的剖析樹
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
Tree* parseWhile(Parser *p) {                  // 建立 FOR 的樹根
	SemWhile *sem=push(p, WHILE, SemWhile);                           // 取得 for
  next(p, "while");                           // 取得 (
  next(p, "(");                             // 剖析 STMT
	sem->exp = parseExp(p);                             // 取得 ;
  next(p, ")");                             // 剖析 BASE
	sem->base = parseBase(p);                            // 取出 FOR 的剖析樹
	return pop(p, WHILE);
}

// 如果開頭是 TYPE ID，也就是  (int|byte|char|float|ID)** ID，那就是 DECL。 
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
	} else if (isDecl(p)) { // (isNext(p, SET_BTYPE)) { // DECL_LIST 的第一個為 TYPE
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
    while (isNext(p, "*")) { // 剖析 ** 
        next(p, "*");
        starCount ++;
    }
	sem->id = next(p, ID); // 剖析 ID
	char *id = token(sem->id);
	
	// 建立 ID 的符號記錄 Symbol(id, DECL)
//	Symbol *sym = SymNew(p->block, id, DECL);
	Var *var = ObjNew(Var, 1);
//	sym->u.decl = decl;
//	SymDebug(sym);
	var->typeName = p->decl.typeName;
	var->dimCount = 0;
    while (isNext(p, "[")) { // 剖析 ([ CINT ])*
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

BOOL isNext(Parser *p, char *pTags) {                        // 檢查下一個詞彙的型態
    Scanner *s = p->scanner;
	char tTags[MAX_LEN+1];
	sprintf(tTags, "|%s|", pTags);
	if (strPartOf(s->tag, tTags))
    return TRUE;
  else
    return FALSE;
}

Tree *TokenNodeNew(char *token, char *tag) {
    Tree *node = TreeNew(tag);  //   建立詞彙節點(token,type)
    SemToken *sem = ObjNew(SemToken, 1);
    sem->token = strNew(token);
    node->sem = sem;
    return node;
}

Tree *next(Parser *p, char *pTags) {                         // 檢查下一個詞彙的型態
    Scanner *s = p->scanner;
	if (isNext(p, pTags)) {                                    // 如果是pTypes型態之一
	    Tree *child = TokenNodeNew(s->token, s->tag);  //   建立詞彙節點(token,type)
    Tree *parentTree = ArrayPeek(p->stack);                   //   取得父節點，
    TreeAddChild(parentTree, child);                          //   加入父節點成為子樹
		if (strEqual(s->tag, s->token))
		   debug("%s KEY:%s\n", level(p), s->tag);
		else
	       debug("%s %s:%s\n", level(p), s->tag, s->token);
        ScannerNext(s);
		return child;                                             //   傳回該詞彙
  } else {                                                    // 否則(下一個節點型態錯誤)
		debug("next():token=%s, tag=%s is not in tag(%s)\n", s->token, s->tag, pTags);     //   印出錯誤訊息
	ERROR();
    return NULL;
  }
}

Tree* push1(Parser *p, char* pTag) { // 建立 pType 型態的子樹，推入堆疊中
	debug("%s+%s\n", level(p), pTag);
//	p->node = TreeNew(pTag);
//	ArrayPush(p->stack, p->node);
//	return p->node;
	Tree *node = TreeNew(pTag);
	ArrayPush(p->stack, node);
	return node;
}

Tree* pop(Parser *p, char* pTag) { // 取出 pTag 標記的子樹
	Tree *tree = ArrayPop(p->stack, NULL); // 取得堆疊最上層的子樹
	debug("%s-%s\n", level(p), tree->tag); // 印出以便觀察
	if (strcmp(tree->tag, pTag)!=0) {  // 如果型態不符合
		debug("pop(%s):should be %s\n",tree->tag, pTag); //  印出錯誤訊息
	ERROR();
  }
  if (p->stack->count > 0) {                                  // 如果堆疊不是空的
    Tree *parentTree = ArrayPeek(p->stack);                   //  取出上一層剖析樹
	  TreeAddChild(parentTree, tree);  //  將建構完成的剖析樹掛到樹上，成為子樹
  }
  return tree;
}
