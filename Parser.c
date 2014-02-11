#include "Parser.h"
/*
BASE_LIST = BASE*
BASE = IF | FOR | WHILE | BLOCK | METHOD | STMT ;
IF = if (COND) BASE (else BASE)?
FOR = for (STMT ; COND ; STMT) BASE
WHILE = while (COND) BASE
METHOD = def ID(DECL_LIST) BLOCK
STMT = return EXP | DECL | ID (EXP_LIST) | ID = EXP | ID OP1
DECL = TYPE VAR_LIST
BLOCK = { BASE_LIST }
VAR = ID (= EXP)?
COND = EXP COND_OP EXP
EXP = ITEM (OP2 EXP)? | ( EXP (OP2 EXP)? )
ITEM = INT | FLOAT | STRING | ID | ID(EXP_LIST?)
EXP_LIST = EXP (, EXP)*
VAR_LIST = VAR (, VAR)*
DECL_LIST = DECL (, DECL)*

ID = [A-Za-z_][0-9A-Za-z_]*
INT = [0-9]+
FLOAT = [0-9]+.[0-9]+
STRING = ".*"
OP2 = [+-/*%&|^><=|'<<'|'>>']
OP1 = ++ | --
COND_OP = |<|>|<=|>=|==|!=
TYPE = int | byte | float | ptr
*/

Parser *parse(char *text) {        // 剖析器的主要函數
  Parser *p=ParserNew();           // 建立剖析器
  ParserParse(p, text);            // 開始剖析
  return p;                        // 傳回剖析器
}

char* nextToken(Parser *p);
char* tokenToType(char *token);
Tree* push(Parser *p, char* term);
Tree* pop(Parser *p, char* term);
Tree* parseBaseList(Parser *p);
void parseBlock(Parser *p);
void parseFor(Parser *p);
void parseIf(Parser *p);
void parseWhile(Parser *p);
void parseBase(Parser *p);
void parseStmt(Parser *p);
void parseDecl(Parser *p);
void parseCond(Parser *p);
void parseExp(Parser *p);
void parseMethod(Parser *p);
void parseItem(Parser *p);
void parseExpList(Parser *p);
void parseDeclList(Parser *p);
void parseVarList(Parser *p);
BOOL isEnd(Parser *p);
BOOL isNext(Parser *p, char *pTypes);
char *next(Parser *p, char *pTypes);

Parser *ParserNew() {
  Parser *parser = ObjNew(Parser, 1);
  parser->tokens = NULL;
  parser->tree = NULL;
  parser->stack = ArrayNew(10);
  return parser;
}

void ParserFree(Parser *parser) {
  ArrayFree(parser->tokens, strFree);
  ArrayFree(parser->stack, NULL);
  TreeFree(parser->tree);
  ObjFree(parser);
}

void ParserParse(Parser *p, char *text) {                 // 剖析物件的主函數
  printf("======= tokenize =======\n");                   //   首先呼叫掃描器的主函數 tokenize() 將程式轉換為詞彙串列
  p->tokens = tokenize(text);
  printTokens(p->tokens);
  p->tokenIdx = 0;
  printf("======= parsing ========\n");
  p->tree = parseBaseList(p);                                 // 開始剖析 PROG = BaseList
  if (p->stack->count != 0) {                             // 如果剖析完成後堆疊是空的，那就是剖析成功
    debug("parse fail:stack.count=%d", p->stack->count); //   否則就提示錯誤訊息
	ERROR();
  }
}

// BASE_LIST = BASE*
Tree *parseBaseList(Parser *p) {             // 剖析 PROG=BaseList 規則
  push(p, "BASE_LIST");                      // 建立 BaseList 的樹根
  while (!isEnd(p) && !isNext(p, "}"))       //  剖析 BASE，直到程式結束或碰到 } 為止
      parseBase(p);
  pop(p, "BASE_LIST");                       // 取出 BaseList 的剖析樹
}                                           // 取出 PROG 的剖析樹

// BASE = IF | FOR | WHILE | METHOD | BLOCK | STMT ;
void parseBase(Parser *p) {                 // 剖析 BASE 規則
  push(p, "BASE");                          // 建立 BASE 的剖析樹
  if (isNext(p, "if"))                      // 如果下一個詞彙是 if
    parseIf(p);                             //  根據 if 規則進行剖析
  else if (isNext(p, "for"))                // 如果下一個詞彙是 for
    parseFor(p);                            //  根據 FOR 規則進行剖析
  else if (isNext(p, "while"))                // 如果下一個詞彙是 for
    parseWhile(p);                            //  根據 FOR 規則進行剖析
  else if (isNext(p, "def"))                // 如果下一個詞彙是 def
    parseMethod(p);                         //  根據 method 規則進行剖析
  else if (isNext(p, "{"))
    parseBlock(p);
  else {                                     // 否則
    parseStmt(p);                           //  根據 STMT 規則進行剖析
	next(p, ";");
  }
  pop(p, "BASE");                           // 取出 BASE 的剖析樹
}

// BLOCK = { BASE_LIST }
void parseBlock(Parser *p) {
  push(p, "BLOCK");
  next(p, "{");
  parseBaseList(p);
  next(p, "}");
  pop(p, "BLOCK");
}

// METHOD = def ID(DECL_LIST) BLOCK
void parseMethod(Parser *p) {
  push(p, "METHOD");
  next(p, "def");
  next(p, ID);
  next(p, "(");
  if (!isNext(p, ")"))
    parseDeclList(p);
  next(p, ")");
  parseBlock(p);
  pop(p, "METHOD");
}

// FOR = for ( STMT ; COND ; STMT) BASE
void parseFor(Parser *p) {                  
  push(p, "FOR");                           // 建立 FOR 的樹根
  next(p, "for");                           // 取得 for
  next(p, "(");                             // 取得 (
  parseStmt(p);                             // 剖析 STMT
  next(p, ";");                             // 取得 ;
  parseCond(p);                              // 剖析 COND
  next(p, ";");                             // 取得 ;
  parseStmt(p);                             // 剖析 STMT
  next(p, ")");                             // 取得 )
  parseBase(p);                             // 剖析 BASE
  pop(p, "FOR");                            // 取出 FOR 的剖析樹
}

// IF = if (COND) BASE (else BASE)?
void parseIf(Parser *p) {                  // 建立 FOR 的樹根
  push(p, "IF");                           // 取得 for
  next(p, "if");                           // 取得 (
  next(p, "(");                             // 剖析 STMT
  parseCond(p);                             // 取得 ;
  next(p, ")");                             // 剖析 COND
  parseBase(p);                            // 取出 FOR 的剖析樹
  if (isNext(p, "else")) {
    next(p, "else");
    parseBase(p);
  }
  pop(p, "IF");
}

// WHILE = while (COND) BASE
void parseWhile(Parser *p) {                  // 建立 FOR 的樹根
  push(p, "WHILE");                           // 取得 for
  next(p, "while");                           // 取得 (
  next(p, "(");                             // 剖析 STMT
  parseCond(p);                             // 取得 ;
  next(p, ")");                             // 剖析 BASE
  parseBase(p);                            // 取出 FOR 的剖析樹
  pop(p, "WHILE");
}

// STMT = return EXP | DECL | ID (EXP_LIST) | ID = EXP | ID OP1
// 注意： DECL_LIST 的第一個為 TYPE
void parseStmt(Parser *p) {
  push(p, "STMT");
  if (isNext(p, "return")) {
    next(p, "return");
    parseExp(p);
  } else if (isNext(p, TYPE))
    parseDecl(p);
  else if (isNext(p, ID)) {
    next(p, ID);
	if (isNext(p, "(")) {
      next(p, "(");
      parseExpList(p);
	  next(p, ")");
	} else if (isNext(p, "=")) {
	  next(p, "=");
	  parseExp(p);
	} else if (isNext(p, OP1)) {
	  next(p, OP1);
	} else
	  ERROR();
  }
  pop(p, "STMT");
}

// DECL = TYPE VAR_LIST
void parseDecl(Parser *p) {
  push(p, "DECL");
  next(p, TYPE);
  parseVarList(p);
  pop(p, "DECL");  
}

// VAR = ID (= EXP)?
void parseVar(Parser *p) {
  push(p, "VAR");
  next(p, ID);
  if (isNext(p, "=")) {
    next(p, "=");
    parseExp(p);
  }
  pop(p, "VAR");  
}

/*
// CALL = Id(EXP_LIST?)?
void parseCall(Parser *p) {
  push(p, "CALL");
  next(p, "id");
  if (isNext(p, "(")) {
    next(p, "(");
    if (!isNext(p, ")"))
      parseExpList(p);
    next(p, ")");
  }
  pop(p, "CALL");  
}
*/

// COND = EXP COND_OP EXP
void parseCond(Parser *p) {
  push(p, "COND");
  parseExp(p);
  next(p, COND_OP);
  parseExp(p);
  pop(p, "COND");
}

// EXP = ITEM (OP2 EXP)? | ( EXP (OP2 EXP)? )
void parseExp(Parser *p) {
  push(p, "EXP");
  if (isNext(p, "(")) {
    next(p, "(");
    parseExp(p);
	if (!isNext(p, ")")) {
	  next(p, OP2);
      parseExp(p);
	}
    next(p, ")");
  } else {
    parseItem(p);
    if (isNext(p, OP2)) {
      next(p, OP2);
      parseExp(p);
    }
  }
  pop(p, "EXP");
}

// ITEM = INT | FLOAT | STRING | ID | ID(EXP_LIST?)
void parseItem(Parser *p) {
  push(p, "ITEM");
  if (isNext(p, INTEGER))
    next(p, INTEGER);
  else if (isNext(p, FLOAT))
    next(p, FLOAT);
  else if (isNext(p, STRING))
    next(p, STRING);
  else if (isNext(p, ID)) {
    next(p, ID);
	if (isNext(p, "(")) {
      next(p, "(");
	  if (!isNext(p, ")"))
	    parseExpList(p);
	  next(p, ")");
	}
  }
  pop(p, "ITEM");
}

// EXP_LIST = EXP (, EXP)*
void parseExpList(Parser *p) {
  push(p, "EXP_LIST");
  parseExp(p);
  while (isNext(p, ",")) {
    next(p, ",");
    parseExp(p);
  }
  pop(p, "EXP_LIST");  
}

// VAR_LIST = VAR (, VAR)*
void parseVarList(Parser *p) {
  push(p, "VAR_LIST");
  parseVar(p);
  while (isNext(p, ",")) {
    next(p, ",");
    parseVar(p);
  }
  pop(p, "VAR_LIST");  
}

// DECL_LIST = DECL (, DECL)*
void parseDeclList(Parser *p) {
  push(p, "DECL_LIST");
  parseDecl(p);
  while (isNext(p, ",")) {
    next(p, ",");
    parseDecl(p);
  }
  pop(p, "DECL_LIST");  
}

char* level(Parser *p) {
  return strSpaces(p->stack->count);
}

char* nextToken(Parser *p) {
  return (char*) p->tokens->item[p->tokenIdx];
}

BOOL isEnd(Parser *p) {
  return (p->tokenIdx >= p->tokens->count);
}

BOOL isNext(Parser *p, char *pTypes) {
  char *token = nextToken(p);
  if (token == NULL) return FALSE;
  char *type = tokenToType(token);
  char tTypes[MAX_LEN+1];
  sprintf(tTypes, "|%s|", pTypes);
  if (strPartOf(type, tTypes))
    return TRUE;
  else
    return FALSE;
}

char *next(Parser *p, char *pTypes) {                         // 檢查下一個詞彙的型態
  char *token = nextToken(p);                                 // 取得下一個詞彙
  if (isNext(p, pTypes)) {                                    // 如果是pTypes型態之一
    char *type = tokenToType(token);                          //   取得型態
    Tree *child = TreeNew(type, token);                       //   建立詞彙節點(token,type)
    Tree *parentTree = ArrayPeek(p->stack);                   //   取得父節點，
    TreeAddChild(parentTree, child);                          //   加入父節點成為子樹
    printf("%s idx=%d, token=%s, type=%s\n",                  //   印出詞彙以便觀察
      level(p),p->tokenIdx,token,type);
    p->tokenIdx++;                                            //   前進到下一個節點
    return token;                                             //   傳回該詞彙
  } else {                                                    // 否則(下一個節點型態錯誤)
    debug("next():%s is not type(%s)\n", token, pTypes);     //   印出錯誤訊息
	ERROR();
    p->tokenIdx++;                                            //  前進到下一個節點
    return NULL;
  }
}

Tree* push(Parser *p, char* pType) {                          // 建立 pType 型態的子樹，推入堆疊中
  printf("%s+%s\n", level(p), pType);
  Tree* tree = TreeNew(pType, "");
  ArrayPush(p->stack, tree);
  return tree;
}

Tree* pop(Parser *p, char* pType) {                           // 取出 pType型態的子樹
  Tree *tree = ArrayPop(p->stack);                            // 取得堆疊最上層的子樹
  printf("%s-%s\n", level(p), tree->type);                    // 印出以便觀察
  if (strcmp(tree->type, pType)!=0) {                         // 如果型態不符合
    debug("pop(%s):should be %s\n",tree->type,pType);        //  印出錯誤訊息
	ERROR();
  }
  if (p->stack->count > 0) {                                  // 如果堆疊不是空的
    Tree *parentTree = ArrayPeek(p->stack);                   //  取出上一層剖析樹
    TreeAddChild(parentTree, tree);                           //  將建構完成的剖析樹加入上一層節點中，成為子樹
  }
  return tree;
}
