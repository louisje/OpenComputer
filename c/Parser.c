#include "Parser.h"
/*
BASE_LIST = BASE*
BASE = IF | FOR | WHILE | STMT ; | CLASS | METHOD | (PATH? (=> VAR_LIST)?)? BLOCK
IF = if (EXP) BASE (else BASE)?
FOR = for ( STMT ; EXP ; STMT) BASE
WHILE = while (EXP) BASE
STMT = (return|yield|throw) ITEM | PATH = EXP
CLASS = class Id is Id BLOCK
METHOD = def Id(VAR_LIST?) BLOCK
BLOCK = { BASE_LIST }
EXP =  ( EXP Op2 EXP ) | ITEM (Op2 ITEM)?
ITEM = Int | Float | String | Id Op1? | PATH
PATH = Id(EXP_LIST?)? (. Id(EXP_LIST?)? )*
EXP_LIST = EXP (, EXP)*
VAR_LIST = VAR (, VAR)*
VAR = Id (= EXP)?

Id = [A-Za-z_][0-9A-Za-z_]*
Int = [0-9]+
Float = [0-9]+.[0-9]+
String = ".*"
Op2 = [+-/*%&|^><=|'<<'|'>>']
Op1 = (++ | --)
*/

Parser *parse(char *text) {        // ��R�����D�n���
  Parser *p=ParserNew();           // �إ߭�R��
  ParserParse(p, text);            // �}�l��R
  return p;                        // �Ǧ^��R��
}

char* nextToken(Parser *p);
char *tokenToType(char *token);
Tree* push(Parser *p, char* term);
Tree* pop(Parser *p, char* term);
Tree *parseBaseList(Parser *p);
void parseBlock(Parser *p);
void parseFor(Parser *p);
void parseIf(Parser *p);
void parseWhile(Parser *p);
void parseBase(Parser *p);
void parseStmt(Parser *p);
void parsePath(Parser *p);
void parseExp(Parser *p);
void parseClass(Parser *p);
void parseMethod(Parser *p);
void parseItem(Parser *p);
void parseVar(Parser *p);
void parseArgList(Parser *p);
void parseVarList(Parser *p);
void error();
BOOL isEnd(Parser *p);
BOOL isNext(Parser *p, char *pTypes);
char *next(Parser *p, char *pTypes);
void prev(Parser *p);

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
    printf("parse fail:stack.count=%d", p->stack->count); //   �_�h�N���ܿ��~�T��
    error();
  }
}

void error() {
  printf("error()!\n");
  system("pause");
  exit(1);
}

// BASE_LIST = BASE*
Tree *parseBaseList(Parser *p) {             // ��R PROG=BaseList �W�h
  push(p, "BASE_LIST");                      // �إ� BaseList �����
  while (!isEnd(p) && !isNext(p, "}"))       //  ��R BASE�A����{�������θI�� } ����
      parseBase(p);
  pop(p, "BASE_LIST");                       // ���X BaseList ����R��
}                                           // ���X PROG ����R��

// BASE = IF | FOR | CLASS | METHOD | BLOCK | STMT ; 
void parseBase(Parser *p) {                 // ��R BASE �W�h
  push(p, "BASE");                          // �إ� BASE ����R��
  if (isNext(p, "if"))                      // �p�G�U�@�ӵ��J�O if
    parseIf(p);                             //  �ھ� if �W�h�i���R
  else if (isNext(p, "for"))                // �p�G�U�@�ӵ��J�O for
    parseFor(p);                            //  �ھ� FOR �W�h�i���R
  else if (isNext(p, "class"))              // �p�G�U�@�ӵ��J�O class
    parseClass(p);                          //  �ھ� class �W�h�i���R
  else if (isNext(p, "def"))                // �p�G�U�@�ӵ��J�O def
    parseMethod(p);                         //  �ھ� method �W�h�i���R
  else if (isNext(p, "{"))
    parseBlock(p);
  else {                                    // �_�h
    parseStmt(p);                           //  �ھ� STMT �W�h�i���R
    next(p, ";");                           //  ���o ;
  }
  pop(p, "BASE");                           // ���X BASE ����R��
}

// BLOCK = '{' BaseList '}'
void parseBlock(Parser *p) {
  push(p, "BLOCK");
  next(p, "{");
  parseBaseList(p);
  next(p, "}");
  pop(p, "BLOCK");
}

// CLASS = class Id (is Id)? BLOCK
void parseClass(Parser *p) {
  push(p, "CLASS");
  next(p, "class");
  next(p, ID);
  if (isNext(p, "is")) {
    next(p, "is");
    next(p, ID);
  }
  parseBlock(p);
  pop(p, "CLASS");
}

// METHOD = def Id(ID_LIST?) BLOCK
void parseMethod(Parser *p) {
  push(p, "METHOD");
  next(p, "def");
  next(p, ID);
  next(p, "(");
  if (!isNext(p, ")"))
    parseVarList(p);
  next(p, ")");
  parseBlock(p);
  pop(p, "METHOD");
}

// FOR = for ( STMT ; EXP ; STMT) BASE | for Id in EXP BASE
void parseFor(Parser *p) {                  // �إ� FOR �����
  push(p, "FOR");                           // ���o for
  next(p, "for");                           // ���o (
  next(p, "(");                             // ��R STMT
  parseStmt(p);                             // ���o ;
  next(p, ";");                             // ��R COND
  parseExp(p);                              // ���o ;
  next(p, ";");                             // ��R STMT
  parseStmt(p);                             // ���o )
  next(p, ")");                             // ��R BASE
  parseBase(p);                             // ���X FOR ����R��
  pop(p, "FOR");
}

// IF = if (EXP) BASE (else BASE)?
void parseIf(Parser *p) {                  // �إ� FOR �����
  push(p, "IF");                           // ���o for
  next(p, "if");                           // ���o (
  next(p, "(");                             // ��R STMT
  parseExp(p);                             // ���o ;
  next(p, ")");                             // ��R COND
  parseBase(p);                            // ���X FOR ����R��
  if (isNext(p, "else")) {
    next(p, "else");
    parseBase(p);
  }
  pop(p, "IF");
}

// WHILE = while (EXP) BASE
void parseWhile(Parser *p) {                  // �إ� FOR �����
  push(p, "WHILE");                           // ���o for
  next(p, "while");                           // ���o (
  next(p, "(");                             // ��R STMT
  parseExp(p);                             // ���o ;
  next(p, ")");                             // ��R BLOCK
  parseBase(p);                            // ���X FOR ����R��
  pop(p, "WHILE");
}

// STMT = (return|yield|throw) PATH | PATH = PATH
void parseStmt(Parser *p) {
  push(p, "STMT");
  if (isNext(p, "return|yield|throw")) {
    next(p, "return|yield|throw");
    parsePath(p);
  } else {
    parsePath(p);
    if (isNext(p, "=")) {
      next(p, "=");
      parsePath(p);
    }
  }
  pop(p, "STMT");
}

// PATH = EXP (. EXP)*
void parsePath(Parser *p) {
  push(p, "PATH");
//  if (isNext(p, ".")) next(p, ".");
  parseExp(p);
  while (isNext(p, ".")) {\
    next(p, ".");
    parseExp(p);
  }
  pop(p, "PATH");  
}

// EXP =  ( EXP Op EXP ) | ITEM (Op ITEM)?
void parseExp(Parser *p) {
  push(p, "EXP");
  if (isNext(p, "(")) {
    next(p, "(");
    parseExp(p);
    next(p, OP2);
    parseExp(p);
    next(p, ")");
  } else {
    parseItem(p);
    if (isNext(p, OP2)) {
      next(p, OP2);
      parseItem(p);
    }
  }
  pop(p, "EXP");
}

// ITEM = Int | Float | String | Id (++|--)? | Id(ARG_LIST?) | Id
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
    if (isNext(p, OP1))
      next(p, OP1);
    else if (isNext(p, "(")) {
      next(p, "(");
      if (isNext(p, ID))
        parseArgList(p);
      next(p, ")");
    }
  }
  pop(p, "ITEM");
}

// ARG_LIST = PATH (, PATH)*
void parseArgList(Parser *p) {
  push(p, "ARG_LIST");
  parsePath(p);
  while (isNext(p, ",")) {
    next(p, ",");
    parsePath(p);
  }
  pop(p, "ARG_LIST");  
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

// VAR = Id (= PATH)?
void parseVar(Parser *p) {
  push(p, "VAR");
  next(p, ID);
  if (isNext(p, "=")) {
    next(p, "=");
    parsePath(p);
  }
  pop(p, "VAR");
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

void prev(Parser *p) {
    Tree *parentTree = ArrayPeek(p->stack);                   //   ���o���`�I�A
    Tree *last = ArrayPop(parentTree->childs);
    TreeFree(last);
    printf("%s prev()\n", level(p));
    p->tokenIdx--;
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
    printf("next():%s is not type(%s)\n", token, pTypes);     //   �L�X���~�T��
    error();
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
    printf("pop(%s):should be %s\n",tree->type,pType);        //  �L�X���~�T��
	error();
  }
  if (p->stack->count > 0) {                                  // �p�G���|���O�Ū�
    Tree *parentTree = ArrayPeek(p->stack);                   //  ���X�W�@�h��R��
    TreeAddChild(parentTree, tree);                           //  �N�غc��������R��[�J�W�@�h�`�I���A�����l��
  }
  return tree;
}


