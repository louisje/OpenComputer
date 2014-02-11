// 本軟體採用公共領域授權 (Public Domain License)，您可以任意使用本軟體及其原始碼。 
// 但作者對於任何因本軟體所產生的損害（包含他人修改後所造成的損害），不負任何責任。
// 作者：陳鍾誠

#include "Parser.h"

// 功能：Parser 剖析階段的測試程式。
// 範例：ParserTest("test.c1");
void ParserTest(char *fileName) {
	debug("=======ParserTest()==========\n");
    SymTable *symTable = SymTableNew(); // 建立符號表 
    char *text = fileToStr(fileName);   // 讀入 C1 語言程式碼，成為一字串 
    Tree *tree = parse(text, symTable); // 剖析該程式碼，建立剖析樹與符號表。
    SymTableDebug(symTable);            // 印出符號表。
    TreeFree(tree);                     // 釋放剖析樹。 
    strFree(text);                      // 釋放程式碼字串 
    SymTableFree(symTable);             // 釋放符號表 
    memCheck(); // 檢查記憶體     
}

// 功能：剖析階段的主程式 
// 範例：Tree *tree = parse(text, symTable); 
Tree *parse(char *text, SymTable *symTable) {  // 剖析器的主要函數
	Scanner *scanner = ScannerNew(text);       // 建立掃描器 (詞彙分析用) 
	Parser *p=ParserNew(scanner, symTable);    // 建立剖析器 (語法剖析用)
	Tree *tree = ParserParse(p, text);         // 剖析程式為語法樹 
	ParserFree(p);                             // 釋放頗析樹 
	ScannerFree(scanner);                      // 釋放掃描器 
	return tree;                            // 傳回剖析器
}

// 功能：建立新的剖析器 Parser 物件 
// 範例：Parser *p = ParserNew(scanner, symTable);
Parser *ParserNew(Scanner *scanner, SymTable *symTable) {
	Parser *p = ObjNew(Parser, 1);
	p->nodeStack = ArrayNew(10);
	p->blockStack = ArrayNew(10);
	p->scanner = scanner;
	p->symTable = symTable;
    ScannerNext(scanner); // 本剖析器總是先取得下一個 token，以便 isNext() 進行判斷。
	return p;
}

// 功能：釋放剖析器物件的記憶體 
// 範例：ParserFree(p); 
void ParserFree(Parser *p) {
	ArrayFree(p->blockStack, (FuncPtr1) BlockFree);
	ArrayFree(p->nodeStack, NULL);
	ObjFree(p);
}

// 功能：剖析整個程式碼 (text)。 
// 範例：ParserParse(p, text); 
Tree *ParserParse(Parser *p, char *text) { // 剖析物件的主函數
	debug("======= parsing ========\n");
	Tree *tree = parseProg(p); // 開始剖析整個程式 (PROG)，並建立語法樹 p->tree
	if (p->nodeStack->count != 0) { // 如果剖析完成後堆疊是空的，那就是剖析成功
		ERROR();// 否則就是剖析失敗，有語法錯誤
  }
	return tree;
}

// 語法：PROG = (STRUCT | METHOD | DECL ; )* 
// 功能：剖析 PROG 並建立語法樹 
// 範例：Tree *prog = parseProg(p); 
Tree *parseProg(Parser *p) { // 剖析 PROG 規則
	SemProg *sem=push(p, PROG, SemProg); // 建立 PROG 的語法樹及語意結構
	pushBlock(p, Global);  // 推入全域區塊 
	while (!isNext(p, kEND)) {     //  剖析 BASE，直到程式結束或碰到 } 為止
	    if (isNext(p, "struct"))
	       parseStruct(p);
	    else { // 由於 METHOD 與 DECL 的開頭都是 TYPE **ID ...，因此必須判斷是哪一種情況。
           if (isMethod(p)) { // 向前偷看後發現是 TYPE **ID(，所以是 Method 
                parseMethod(p);
           } else { // 否則就必須是 DECL ; 
                parseDecl(p);
	            next(p, ";");
          }
        }
    }
    popBlock(p); // 取出全域區塊
	return pop(p, PROG); // 取出 PROG 的整棵語法樹 
}

// 語法：METHOD = TYPE **ID (PARAM_LIST?) BLOCK
// 功能：判斷到底接下來是否為 METHOD，是的話傳回 TRUE，否則傳回 FALSE 
//       由於 METHOD 與 DECL 的開頭都是 TYPE **ID ...，因此必須判斷是哪一種情況。
//       本函數會向前偷看，如果發現是 TYPE **ID(，那就應該是 Method。 
// 範例：if (isMethod(p)) parseMethod(p);
BOOL isMethod(Parser *p) {
    BOOL rzFlag = TRUE;
    Scanner *s = p->scanner; // s=掃描器 
    ScannerStore(s); // 儲存掃描器狀態 
    if (isNext(p, "int|byte|char|float|ID")) // 偷看 TYPE
        ScannerNext(s); // 略過 TYPE
    else 
        rzFlag=FALSE;
    while (isNext(p, "*")) ScannerNext(s); // 偷看並略過星號 
    if (isNext(p, ID))  // 偷看 ID
        ScannerNext(s); // 略過 ID 
    else 
        rzFlag=FALSE;
    if (!isNext(p, "(")) rzFlag=FALSE; // 如果接下來是 (，那麼就應該是 Method。
    ScannerRestore(s); // 恢復掃描器狀態。 
    return rzFlag;
}

// 語法：METHOD = TYPE **ID (PARAM_LIST?) BLOCK
// 功能：剖析 METHOD 並建立語法樹
// 範例：Tree *method = parseMethod(p); 
Tree* parseMethod(Parser *p) {
	SemMethod *sem=push(p, METHOD, SemMethod); // 建立 METHOD 的語法樹及語意結構
	sem->type=parseEType(p); // 剖析 ETYPE

	// 剖析 ** (n 個星號, n>=0)
	int starCount = 0; // 星號數量的初始值 
    while (isNext(p, "*")) { // 如果下一個是星號 
        next(p, "*"); // 取得該星號 
        starCount ++; // 將星號數加一 
    }
    sem->id = next(p, ID); // 剖析 ID
    
	// 建立 ID 的符號記錄 Symbol(id, METHOD) 
	char *id = token(sem->id);	// 取得符號名稱。 
	Symbol *sym = SymNew(Global, id, SymMethod); // 建立符號記錄 
	Method *method = sym->typePtr; // 設定 method 結構。 
	method->ret.typeSym = p->ptype.typeSym; // 設定傳回符號 
	method->ret.starCount = p->ptype.starCount; // 設定傳回符號的星號個數。 
	SymTablePut(p->symTable, sym); // 將符號記錄放入符號表中 

	pushBlock(p, sym); // 將 Method 符號推入區塊堆疊
	
    sem->symMethod = sym; // 設定語意結構 sem 的 symMethod 欄位 
	
    // 剖析參數部分 (PARAM_LIST?) 
  next(p, "(");
	if (!isNext(p, ")")) // 如果接下來不是 )，那就是有 PARAM_LIST 
		sem->paramList = parseParamList(p); // 剖析 PARAM_LIST
  next(p, ")");
	
	sem->block = parseBlock(p); // 剖析 BLOCK
	SemBlock *semBlock = sem->block->sem;
	method->block = semBlock->symBlock; // 設定 method 符號的 block 子符號 

    popBlock(p);
	return pop(p, METHOD); // 取出 METHOD 的語法樹。
}

// 語法：STRUCT = struct ID { (DECL ;)* }
// 功能：剖析 STRUCT 並建立語法樹
// 範例：Tree *s = parseStruct(p);
Tree* parseStruct(Parser *p) {
	SemStruct *sem=push(p, STRUCT, SemStruct); // 建立 STRUCT 語法樹 

	next(p, "struct"); // 剖析 struct 
	sem->id = next(p, ID); // 剖析 ID
	
	// 建立 ID 的符號記錄 Symbol(id, METHOD) 
	char *id = token(sem->id);	// 取得符號名稱。 
	Symbol *sym = SymNew(Global, id, SymStruct); // 建立符號 -- 結構。 
	SymTablePut(p->symTable, sym); // 放入符號表。 
	
	sem->symStruct = sym;  // 設定語意結構 sem 的 symMethod 欄位 
	
    pushBlock(p, sym); // 將 Struct 區塊推入堆疊
	
	// 剖析 { (DECL ;)* }
	next(p, "{"); 
	while (!isNext(p, "}")) {
        parseDecl(p);
        next(p, ";");
    }
	next(p, "}");
	
	popBlock(p); // 從區塊堆疊中取出 Struct 區塊 
    
	return pop(p, STRUCT); // 取出 STRUCT 的語法樹。
}

// 語法：BASE = IF | FOR | WHILE | BLOCK | STMT ;
// 功能：剖析 BASE 並建立 BASE 的語法樹 
// 範例：Tree *base = parseBase(p); 
Tree* parseBase(Parser *p) { // 剖析 BASE 規則
	SemBase *sem=push(p, BASE, SemBase); // 建立 BASE 的語法樹及語意結構
	if (isNext(p, "if")) // 如果下一個詞彙是 if
		parseIf(p); // 剖析 IF 程式段 
	else if (isNext(p, "for")) // 如果下一個詞彙是 for
		parseFor(p); // 剖析 FOR 程式段
	else if (isNext(p, "while")) // 如果下一個詞彙是 for
		parseWhile(p); // 剖析 WHILE 程式段
	else if (isNext(p, "{")) // 如果下一個詞彙是 {
		parseBlock(p); // 剖析 BLOCK 程式段
	else { // 否則應該是 STMT ; 
		parseStmt(p); // 剖析 STMT 程式段
		next(p, ";"); // 取得分號 ;  
	}
	return pop(p, BASE); // 取出 BASE 的剖析樹
}

// 語法：BLOCK = { BASE* }
// 功能：剖析 BLOCK 並建立語法樹 
// 範例：Tree *block = parseBlock(p); 
Tree* parseBlock(Parser *p) {
	SemBlock *sem=push(p, BLOCK, SemBlock); // 建立 BLOCK 的語法樹及語意結構 
	
	Symbol *pblock = peekBlock(p); // 取得父區塊 
	Symbol *sym = SymNew(pblock, "", SymBlock); // 建立區塊符號 
	Block *block = sym->typePtr; // 設定 block 結構。 
	SymTablePut(p->symTable, sym); // 將本區塊加入到符號表中 

	sem->symBlock = sym; // 設定本節點的語意結構 symBlock 為本區塊

	pushBlock(p, sym); // 將符號推入區塊堆疊

	next(p, "{"); // 剖析 { BASE* } 
	while (!isNext(p, "}")) 
        parseBase(p);
	next(p, "}");
	
	popBlock(p);  // 從區塊堆疊中取出 Block 區塊
    
	return pop(p, BLOCK); // 取出 BLOCK 的語法樹。
}

// 語法：FOR = for (STMT ; EXP ; STMT) BASE
// 功能：剖析 FOR 並建立語法樹
// 範例：Tree *f = parseFor(p); 
Tree* parseFor(Parser *p) {                  
	SemFor *sem=push(p, FOR, SemFor); // 建立 FOR 的語法樹及語意結構
  next(p, "for");                           // 取得 for
  next(p, "(");                             // 取得 (
	sem->stmt1 = parseStmt(p);                             // 剖析 STMT
  next(p, ";");                             // 取得 ;
	sem->exp = parseExp(p);                              // 剖析 EXP
  next(p, ";");                             // 取得 ;
	sem->stmt2 = parseStmt(p);                             // 剖析 STMT
  next(p, ")");                             // 取得 )
  parseBase(p);                             // 剖析 BASE
	return pop(p, FOR);               // 取出 FOR 的語法樹。
}

// 語法：IF = if (EXP) BASE (else BASE)?
// 功能：剖析 IF 並建立語法樹
// 範例：Tree *f = parseIf(p);
Tree* parseIf(Parser *p) {
	SemIf *sem=push(p, IF, SemIf);     // 建立 IF 的語法樹及語意結構
	next(p, "if");                     // 取得 if
	next(p, "(");                      // 取得 (
	sem->exp = parseExp(p);            // 剖析 EXP 
	next(p, ")");                      // 取得 )
	sem->base1 = parseBase(p);         // 剖析 BASE
	if (isNext(p, "else")) {           // 如果下一個是 else 
		next(p, "else");               // 取得 else
		sem->base2 = parseBase(p);     // 剖析下一個 BASE
  }
	return pop(p, IF);                 // 取出 IF 的語法樹。
}

// 語法：WHILE = while (EXP) BASE
// 功能：剖析 WHILE 並建立語法樹
// 範例：Tree *w = parseWhile(p);
Tree* parseWhile(Parser *p) {
	SemWhile *sem=push(p, WHILE, SemWhile);// 建立 WHILE 的語法樹及語意結構
	next(p, "while");                      // 取得 while
	next(p, "(");                          // 取得 (
	sem->exp = parseExp(p);                // 剖析 EXP
	next(p, ")");                          // 取得 )
	sem->base = parseBase(p);              // 剖析 BASE
	return pop(p, WHILE);                  // 取出 WHILE 的語法樹。
}

// 語法：STMT = return EXP | DECL | PATH (EXP_LIST) | PATH = EXP | PATH OP1
// 功能：剖析 STMT 並建立語法樹
// 範例：Tree *stmt = parseStmt(p); 
Tree* parseStmt(Parser *p) {
	SemStmt *sem=push(p, STMT, SemStmt);// 建立 STMT 的語法樹及語意結構
	if (isNext(p, "return")) { // 如果下一個是 return，就剖析 return EXP 
    next(p, "return");
		sem->exp = parseExp(p);
	} else if (isDecl(p)) { // 如果是 DECL 
		sem->decl = parseDecl(p); // 剖析 DECL 
	} else { // 否則下一個必須是 PATH 
        sem->path = parsePath(p); // 剖析 PATH 
		if (isNext(p, "(")) { // 下一個是 (，代表是 PATH (EXP_LIST) 的情況 
      next(p, "(");
			sem->expList = parseExpList(p);
	  next(p, ")");
		} else if (isNext(p, "=")) { // 下一個是 =，代表是 PATH = EXP 的情況 
	  next(p, "=");
			sem->exp = parseExp(p);
		} else if (isNext(p, SET_OP1)) { // 下一個是OP1，代表是 PATH OP1 的情況 
			next(p, SET_OP1);
	} else
	  ERROR();
  }
	return pop(p, STMT); // 取出 STMT 的語法樹。
}

// 語法：PATH = ATOM ((.|->) ATOM)*
// 功能：剖析 PATH 並建立語法樹 
// 範例：Tree *path = parsePath(p);
Tree* parsePath(Parser *p) {
    SemPath *sem=push(p, PATH, SemPath);// 建立 PATH 的語法樹及語意結構
    p->atomLast = NULL;
    p->op = NULL;
    p->atomLast = parseAtom(p);      // 剖析 DECL
    while (isNext(p, ".|->")) { // 不斷取得 (.|->) ATOM
        p->op = next(p, ".|->");
        p->atomLast = parseAtom(p);
    }
    Tree *path = pop(p, PATH); // 取出 PATH 的語法樹。
    SemAtom *semLast = p->atomLast->sem;
    PTypeCopy(&sem->ptype, &semLast->ptype);
    return path;
}

// 語法：ATOM = ID (([ EXP ])* |( EXP_LIST? ))
// 功能：剖析 ATOM 並建立語法樹
// 範例：Tree *atom = parseAtom(p);
Tree* parseAtom(Parser *p) {
    SemAtom *sem=push(p, ATOM, SemAtom); // 建立 ATOM 的語法樹及語意結構
    Symbol *sym = NULL;    
    sem->id = next(p, ID); // 取得 ID
    char *id = token(sem->id);
    
    if (p->atomLast == NULL) {
       sym = SymbolLookup(p, id);
    } else {
       SemAtom *semLast = p->atomLast->sem;
       ASSERT(semLast->ptype.typeSym->symType == SymStruct);
       sym = SymTableGet(p->symTable, semLast->ptype.typeSym, id);
    }
    SymDebug(sym);
    
    if (isNext(p, "(")) { // 如果接下來是 (，則應該是函數呼叫 ID ( EXP_LIST? )
       sem->subTag = CALL;
       next(p, "(");
       if (!isNext(p, ")"))
           sem->expList = parseExpList(p);
       next(p, ")");
       Method *method = sym->typePtr;
       PTypeCopy(&sem->ptype, &method->ret);
    } else if (isNext(p, "[")) { // 如果接下來是 [，則應該是陣列宣告 ID ([ EXP ])*
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
    } else { // 只有 ID 
        sem->subTag = ID;
        PTypeCopy(&sem->ptype, sym->typePtr);
    }
    Tree *atom = pop(p, ATOM); // 取出 ATOM 的語法樹。
   return atom;
}

// 語法：PARAM = TYPE VAR
// 功能：剖析 PARAM 並建立語法樹
// 範例：Tree *param = parseParam(p); 
Tree* parseParam(Parser *p) {
	SemParam *sem = push(p, PARAM, SemParam);// 建立 PARAM 的語法樹及語意結構
	sem->type = parseEType(p); // 剖析 TYPE
	sem->var = parseVar(p); // 剖析 VAR
	return pop(p, PARAM); // 取出 PARAM 的語法樹。
}

// 語法：DECL = TYPE VAR_LIST
// 功能：判斷到底接下來是否為 DECL，是的話傳回 TRUE，否則傳回 FALSE 
//       本函數會向前偷看，如果發現是 (int|byte|char|float|ID)** ID，那就是 DECL
// 範例：if (isDecl(p)) parseDecl(p);
BOOL isDecl(Parser *p) {
    BOOL rzFlag = TRUE;
    Scanner *s = p->scanner;                // s=掃描器 
    ScannerStore(s);                        // 儲存掃描器狀態 
    if (isNext(p, "int|byte|char|float|ID"))// 偷看 TYPE
        ScannerNext(s);                     // 略過 TYPE
    else 
        rzFlag=FALSE;
    while (isNext(p, "*")) ScannerNext(s);  // 偷看並略過星號 
    if (!isNext(p, ID)) rzFlag=FALSE;       // 偷看 ID
    ScannerRestore(s);                      // 恢復掃描器狀態。 
    return rzFlag;
}

// 語法：DECL = TYPE VAR_LIST
// 功能：剖析 PROG 並建立語法樹 
// 範例：Tree *decl = parseDecl(p);
Tree* parseDecl(Parser *p) {
	SemDecl *sem = push(p, DECL, SemDecl);// 建立 DECL 的語法樹及語意結構
	sem->type = parseEType(p); // 剖析 ETYPE
	sem->varList = parseVarList(p); // 剖析 VAR_LIST
	return pop(p, DECL); // 取出 DECL 的語法樹。
}

// 語法：ETYPE = (int | byte | char | float | ID) // ID is STRUCT_TYPE
// 功能：剖析 ETYPE 並建立語法樹
// 範例：Tree *etype = parseEType(p); 
Tree* parseEType(Parser *p) {
    SemEType *sem=push(p, ETYPE, SemEType);// 建立 TYPE 的語法樹及語意結構
    Tree *etype = next(p, "int|byte|char|float|ID");  // 取得 (int | byte | char | float | ID)
    char *etypeName = token(etype); // 取得型態名稱 
   	p->ptype.typeSym = SymTableGet(p->symTable, Global, etypeName); // 從符號表中查出該型態的符號
   	ASSERT(p->ptype.typeSym != NULL);
    return pop(p, ETYPE); // 取出 TYPE 的語法樹。
}

// 語法：VAR = ** ID ([ CINT ])* (= EXP)?
// 功能：剖析 VAR 並建立語法樹
// 範例：Tree *var = parseVar(p); 
Tree* parseVar(Parser *p) {
	SemVar *sem = push(p, VAR, SemVar);	// 建立 VAR 的語法樹及語意結構
	int starCount = 0; // 星號數量初始值為 0 
    while (isNext(p, "*")) { // 剖析 ** 
        next(p, "*"); // 取得星號 
        starCount ++; // 計算星號數量 
    }
	sem->id = next(p, ID); // 剖析 ID
	
	// 建立 ID 的符號記錄 Symbol(id, SymVar)
	Symbol *pblock = peekBlock(p); // 取得父區塊符號
	char *id = token(sem->id); // 取得變數名稱 
	Symbol *sym = SymNew(pblock, id, SymVar); // 建立變數符號 
	PType *ptype = sym->typePtr; // 取出變數結構 
    ptype->starCount = starCount; // 設定變數結構中的星號數量 
    ptype->typeSym = p->ptype.typeSym; // 設定變數結構中的符號 
    int typeSize = 0;
    if (starCount > 0)
       typeSize = PTR_SIZE;
    else
       typeSize = ptype->typeSym->size;
	ptype->dimCount = 0;  // 設定變數結構中的陣列維度
	SymTablePut(p->symTable, sym); // 將變數加入符號表中 
    
    int cells = 1;
    while (isNext(p, "[")) { // 剖析 ([ CINT ])*
        next(p, "[");
        Tree *cint = next(p, "CINT");
        ASSERT(ptype->dimCount<DIM_MAX);
        int dim = atoi(token(cint));
        cells = cells * dim;
        ptype->dim[ptype->dimCount++] = dim;
        next(p, "]");
    }
    sym->size = typeSize * cells;

    if (pblock->symType == SymStruct) { // 如果父區塊是 Struct
        Struct *stru = pblock->typePtr;
        ArrayAdd(stru->fields, sym); // 就將此變數加入 Struct 的欄位中。 
    } else if (pblock->symType == SymMethod) {
        Method *method = pblock->typePtr;
        ArrayAdd(method->params, sym); // 就將此變數加入 Struct 的欄位中。         
    } else if (pblock->symType == SymBlock) {
        Block *block = pblock->typePtr;
        ArrayAdd(block->vars, sym); // 就將此變數加入 Struct 的欄位中。 
    }
    ptype->offset = pblock->size;
    pblock->size += sym->size;
    
	if (isNext(p, "=")) { // 剖析 (= EXP)?
    next(p, "=");
		sem->exp = parseExp(p);
  }
	return pop(p, VAR);  // 取出 VAR 的語法樹。
}

// 語法：EXP = TERM (OP2 TERM)?
// 功能：剖析 EXP 並建立語法樹
// 範例：Tree *exp = parseExp(p); 
Tree* parseExp(Parser *p) {
	SemExp *sem = push(p, EXP, SemExp);// 建立 EXP 的語法樹及語意結構
	sem->term1 = parseTerm(p); // 剖析 TERM 
	if (isNext(p, SET_OP2)) { // 如果接下來是 OP2 ，則剖析 (OP2 TERM)?
		sem->op = next(p, SET_OP2);
		sem->term2 = parseTerm(p);
  }
	Tree *exp = pop(p, EXP); // 取出 EXP 的語法樹。
    SemTerm *semTerm1 = sem->term1->sem;
    ObjCopy(&sem->ptype, &semTerm1->ptype, PType);
	return exp;
}

// 語法：TERM = ( EXP (OP2 EXP)? ) | INTEGER | FLOAT | STRING | PATH
// 功能：剖析 TERM 並建立語法樹
// 範例：Tree *term = parseTerm(p); 
Tree* parseTerm(Parser *p) {
	SemTerm *sem = push(p, TERM, SemTerm);// 建立 TERM 的語法樹及語意結構
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
    Tree *term = pop(p, TERM); // 取出 TERM 的語法樹。
    return term;
}

// 語法：VAR_LIST = VAR (, VAR)*
// 功能：剖析 VarList 並建立語法樹
// 範例：Tree *varList = parseVarList(p); 
Tree* parseVarList(Parser *p) {
	SemVarList *sem = push(p, VAR_LIST, SemVarList);// 建立 VAR_LIST 的語法樹及語意結構
	parseVar(p);
  while (isNext(p, ",")) {
    next(p, ",");
		parseVar(p);
  }
	return pop(p, VAR_LIST); // 取出 VAR_LIST 的語法樹。
}

// 語法：EXP_LIST = EXP (, EXP)*
// 功能：剖析 EXP_LIST 並建立語法樹
// 範例：Tree *expList = parseExpList(p); 
Tree* parseExpList(Parser *p) {
	SemExpList *sem = push(p, EXP_LIST, SemExpList);// 建立 EXP_LIST 的語法樹及語意結構
	parseExp(p);
  while (isNext(p, ",")) {
    next(p, ",");
		parseExp(p);
  }
	return pop(p, EXP_LIST); // 取出 EXP_LIST 的語法樹。
}

// 語法：DECL_LIST = DECL (; DECL)*
// 功能：剖析 DECL_LIST 並建立語法樹
// 範例：Tree *declList = parseDeclList(p); 
Tree* parseDeclList(Parser *p) {
	SemDeclList *sem=push(p, DECL_LIST, SemDeclList);// 建立 DECL_LIST 的語法樹及語意結構
  parseDecl(p);
	while (isNext(p, ";")) {
		next(p, ";");
    parseDecl(p);
  }
	return pop(p, DECL_LIST); // 取出 DECL_LIST 的語法樹。
}

// 語法：PARAM_LIST = PARAM (, PARAM)*
// 功能：剖析 PARAM_LIST 並建立語法樹
// 範例：Tree *paramList = parseParamList(p); 
Tree* parseParamList(Parser *p) {
	SemParamList *sem=push(p, PARAM_LIST, SemParamList);// 建立 PARAM_LIST 的語法樹及語意結構
	parseParam(p);
	while (isNext(p, ";")) {
		next(p, ";");
   		parseParam(p);
	}
	return pop(p, PARAM_LIST); // 取出 PARAM_LIST 的語法樹。
}

// ========================== Library ====================================
char* level(Parser *p) {
	return strFill(p->spaces, ' ', p->nodeStack->count);
}

BOOL isNext(Parser *p, char *tags) { // 檢查下一個詞彙的型態
    Scanner *s = p->scanner;
	char tTags[MAX_LEN+1];
	sprintf(tTags, "|%s|", tags);
	if (strPartOf(s->tag, tTags))
    return TRUE;
  else
    return FALSE;
}

Tree *next(Parser *p, char *tags) { // 檢查下一個詞彙的型態
    Scanner *s = p->scanner;
	if (isNext(p, tags)) {            // 如果是pTypes型態之一
	    Tree *child = TreeNew(s->tag);
        child->sem = strNew(s->token);  //   建立詞彙節點(token,type)
	    Tree *parentTree = ArrayPeek(p->nodeStack);                   //   取得父節點，
    TreeAddChild(parentTree, child);                          //   加入父節點成為子樹
		if (strEqual(s->tag, s->token))
		   debug("%s KEY:%s\n", level(p), s->tag);
		else
	       debug("%s %s:%s\n", level(p), s->tag, s->token);
        ScannerNext(s);
		return child;                                             //   傳回該詞彙
  } else {                                                    // 否則(下一個節點型態錯誤)
		debug("next():token=%s, tag=%s is not in tag(%s)\n", s->token, s->tag, tags);     //   印出錯誤訊息
	ERROR();
    return NULL;
  }
}

Tree* push1(Parser *p, char* tag) { // 建立 pType 型態的子樹，推入堆疊中
	debug("%s+%s\n", level(p), tag);
	Tree *node = TreeNew(tag);
	ArrayPush(p->nodeStack, node);
	return node;
}

Tree* pop(Parser *p, char* tag) { // 取出 pTag 標記的子樹
	Tree *tree = ArrayPop(p->nodeStack); // 取得堆疊最上層的子樹
	debug("%s-%s\n", level(p), tree->tag); // 印出以便觀察
	if (strcmp(tree->tag, tag)!=0) {  // 如果型態不符合
		debug("pop(%s):should be %s\n",tree->tag, tag); //  印出錯誤訊息
	ERROR();
  }
	if (p->nodeStack->count > 0) { // 如果堆疊不是空的
	  Tree *parentTree = ArrayPeek(p->nodeStack); //  取出上一層剖析樹
	  TreeAddChild(parentTree, tree);  //  將建構完成的剖析樹掛到樹上，成為子樹
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
