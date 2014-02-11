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

Parser *parse(char *text) {        // ��R�����D�n���
  Parser *p=ParserNew();           // �إ߭�R��
  ParserParse(p, text);            // �}�l��R
  return p;                        // �Ǧ^��R��
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

void ParserParse(Parser *p, char *text) {                 // ��R���󪺥D���
  printf("======= tokenize =======\n");                   //   �����I�s���y�����D��� tokenize() �N�{���ഫ�����J��C
  p->tokens = tokenize(text);
  printTokens(p->tokens);
  p->tokenIdx = 0;
  printf("======= parsing ========\n");
  p->tree = parseBaseList(p);                                 // �}�l��R PROG = BaseList
  if (p->stack->count != 0) {                             // �p�G��R��������|�O�Ū��A���N�O��R���\
    debug("parse fail:stack.count=%d", p->stack->count); //   �_�h�N���ܿ��~�T��
	ERROR();
  }
}

// BASE_LIST = BASE*
Tree *parseBaseList(Parser *p) {             // ��R PROG=BaseList �W�h
  push(p, "BASE_LIST");                      // �إ� BaseList �����
  while (!isEnd(p) && !isNext(p, "}"))       //  ��R BASE�A����{�������θI�� } ����
      parseBase(p);
  pop(p, "BASE_LIST");                       // ���X BaseList ����R��
}                                           // ���X PROG ����R��

// BASE = IF | FOR | WHILE | METHOD | BLOCK | STMT ;
void parseBase(Parser *p) {                 // ��R BASE �W�h
  push(p, "BASE");                          // �إ� BASE ����R��
  if (isNext(p, "if"))                      // �p�G�U�@�ӵ��J�O if
    parseIf(p);                             //  �ھ� if �W�h�i���R
  else if (isNext(p, "for"))                // �p�G�U�@�ӵ��J�O for
    parseFor(p);                            //  �ھ� FOR �W�h�i���R
  else if (isNext(p, "while"))                // �p�G�U�@�ӵ��J�O for
    parseWhile(p);                            //  �ھ� FOR �W�h�i���R
  else if (isNext(p, "def"))                // �p�G�U�@�ӵ��J�O def
    parseMethod(p);                         //  �ھ� method �W�h�i���R
  else if (isNext(p, "{"))
    parseBlock(p);
  else {                                     // �_�h
    parseStmt(p);                           //  �ھ� STMT �W�h�i���R
	next(p, ";");
  }
  pop(p, "BASE");                           // ���X BASE ����R��
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
  push(p, "FOR");                           // �إ� FOR �����
  next(p, "for");                           // ���o for
  next(p, "(");                             // ���o (
  parseStmt(p);                             // ��R STMT
  next(p, ";");                             // ���o ;
  parseCond(p);                              // ��R COND
  next(p, ";");                             // ���o ;
  parseStmt(p);                             // ��R STMT
  next(p, ")");                             // ���o )
  parseBase(p);                             // ��R BASE
  pop(p, "FOR");                            // ���X FOR ����R��
}

// IF = if (COND) BASE (else BASE)?
void parseIf(Parser *p) {                  // �إ� FOR �����
  push(p, "IF");                           // ���o for
  next(p, "if");                           // ���o (
  next(p, "(");                             // ��R STMT
  parseCond(p);                             // ���o ;
  next(p, ")");                             // ��R COND
  parseBase(p);                            // ���X FOR ����R��
  if (isNext(p, "else")) {
    next(p, "else");
    parseBase(p);
  }
  pop(p, "IF");
}

// WHILE = while (COND) BASE
void parseWhile(Parser *p) {                  // �إ� FOR �����
  push(p, "WHILE");                           // ���o for
  next(p, "while");                           // ���o (
  next(p, "(");                             // ��R STMT
  parseCond(p);                             // ���o ;
  next(p, ")");                             // ��R BASE
  parseBase(p);                            // ���X FOR ����R��
  pop(p, "WHILE");
}

// STMT = return EXP | DECL | ID (EXP_LIST) | ID = EXP | ID OP1
// �`�N�G DECL_LIST ���Ĥ@�Ӭ� TYPE
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

char *next(Parser *p, char *pTypes) {                         // �ˬd�U�@�ӵ��J�����A
  char *token = nextToken(p);                                 // ���o�U�@�ӵ��J
  if (isNext(p, pTypes)) {                                    // �p�G�OpTypes���A���@
    char *type = tokenToType(token);                          //   ���o���A
    Tree *child = TreeNew(type, token);                       //   �إߵ��J�`�I(token,type)
    Tree *parentTree = ArrayPeek(p->stack);                   //   ���o���`�I�A
    TreeAddChild(parentTree, child);                          //   �[�J���`�I�����l��
    printf("%s idx=%d, token=%s, type=%s\n",                  //   �L�X���J�H�K�[��
      level(p),p->tokenIdx,token,type);
    p->tokenIdx++;                                            //   �e�i��U�@�Ӹ`�I
    return token;                                             //   �Ǧ^�ӵ��J
  } else {                                                    // �_�h(�U�@�Ӹ`�I���A���~)
    debug("next():%s is not type(%s)\n", token, pTypes);     //   �L�X���~�T��
	ERROR();
    p->tokenIdx++;                                            //  �e�i��U�@�Ӹ`�I
    return NULL;
  }
}

Tree* push(Parser *p, char* pType) {                          // �إ� pType ���A���l��A���J���|��
  printf("%s+%s\n", level(p), pType);
  Tree* tree = TreeNew(pType, "");
  ArrayPush(p->stack, tree);
  return tree;
}

Tree* pop(Parser *p, char* pType) {                           // ���X pType���A���l��
  Tree *tree = ArrayPop(p->stack);                            // ���o���|�̤W�h���l��
  printf("%s-%s\n", level(p), tree->type);                    // �L�X�H�K�[��
  if (strcmp(tree->type, pType)!=0) {                         // �p�G���A���ŦX
    debug("pop(%s):should be %s\n",tree->type,pType);        //  �L�X���~�T��
	ERROR();
  }
  if (p->stack->count > 0) {                                  // �p�G���|���O�Ū�
    Tree *parentTree = ArrayPeek(p->stack);                   //  ���X�W�@�h��R��
    TreeAddChild(parentTree, tree);                           //  �N�غc��������R��[�J�W�@�h�`�I���A�����l��
  }
  return tree;
}
