#include <stdio.h>
#include "Tree.h"

Tree *TreeNew(char *tag) {
  Tree *rzTree = ObjNew(Tree, 1);
	rzTree->tag = tag;
  rzTree->childs = NULL;
  return rzTree;
}
/*
void NodeFree(Tree *node) {
    if (node->tag == STRUCT) {
        SemStruct *sem = node->sem;
        Struct *stru = sem->stru;
        StructFree(stru);
    } else if (node->tag == METHOD) {
        SemMethod *sem = node->sem;
        Struct *method = sem->method;
        MethodFree(method);
    } else if (node->tag == DECL) {
        SemDecl *sem = node->sem;
        Decl *decl = sem->decl;
        DeclFree(decl);
    }
    ObjFree(node->sem);
}
*/

void TreeFree(Tree *tree) {
  if (tree == NULL) return;
        
  if (tree->childs != NULL) {
    int i;
		for (i=0; i<tree->childs->count; i++) {
      TreeFree(tree->childs->item[i]);
        }
    ArrayFree(tree->childs, NULL);
  }
    ObjFree(tree->sem);
  ObjFree(tree);
}

void TreeAddChild(Tree *tree, Tree *child) {        // 在樹中加入一個新的子樹。
  ASSERT(tree != NULL && child != NULL);
  if (tree->childs == NULL)
    tree->childs = ArrayNew(1);
  ArrayAdd(tree->childs, child);
}

void TreePrintLevel(Tree *tree, int level) { // 將該樹列印出來。
	ASSERT(tree != NULL && tree->tag != NULL);
	//  printf("address of tree->type=%d, tree->token=%d\n", tree->type, tree->token);
	char spaces[MAX_LEN];
	ASSERT(level < MAX_LEN);
	strFill(spaces, ' ', level);
	if (tree->childs == NULL) {
        printf("%s+%s:%s:%s\n", spaces, tree->tag, token(tree), strNoNull(tree->type)); // Terminal
	} else {
    	printf("%s+%s:%s\n", spaces, tree->tag, strNoNull(tree->type));
    int i;
    for (i=0; i<tree->childs->count; i++)
      TreePrintLevel(tree->childs->item[i], level + 1);
		printf("%s-%s\n", spaces, tree->tag);
  }
}

void TreePrint(Tree *tree) {
  TreePrintLevel(tree, 0);
}

Tree* child(Tree *node, int i) {
    if (node->childs == NULL)
        return NULL;
    if (i>=0 && i<node->childs->count)
        return node->childs->item[i];
    else if (i<0 && i>-1*node->childs->count) {
        return node->childs->item[node->childs->count+i];
    }
    return NULL;
}

void debugNode(Tree *node) {
   debug("tag=%s type=%s\n", node->tag, node->type);
}
