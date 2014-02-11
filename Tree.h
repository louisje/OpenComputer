#ifndef TREE_H
#define TREE_H

#include "Array.h"
#include "Scanner.h"

typedef struct _Tree {
    char *tag, *type;
    void *sem;
  Array* childs;
} Tree;

Tree *TreeNew(char *tag);
void TreeFree(Tree *tree);
void TreeAddChild(Tree *tree, Tree *child);
void TreePrintLevel(Tree *tree, int level);
void TreePrint(Tree *tree);
Tree* child(Tree *node, int i);
char* token(Tree *node);
void debugNode(Tree *node);
void NodeFree(Tree *node);
Tree *TokenNodeNew(char *token, char *tag);
Tree *TokenNodeFree(Tree *node);

#endif
