#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "Lib.h"
#include "HashTable.h"
#include "Tree.h"

// ���A Type ���G��ơB���c�P���лP�򥻫��A
//   �� �G int x;
//   ����  : int *px;
//   ���  : int total(int a[]) {...};  
//   ���c  : struct Person { ... };

typedef enum { SymVar, SymBlock, SymStruct, SymMethod, SymBType } SymType;

#define DIM_MAX 5
typedef struct Var {
//    char *name;     // �ܼƦW�١G�d�ҡGperson, x, y, i, ....
    char *typeName; // ���A�W�١Gint, float, StructID, MethodID, ...
    void *typePtr;
    int starCount;  // �P�����ƶq (���мh��) 
    int dim[DIM_MAX], dimCount; // �}�C����
} Var;

typedef struct Method {
//    char  *name;     // �ܼƦW�١G�d�ҡGperson, x, y, i, ....
    Var   ret;
    Array *params;
} Method;

typedef struct Struct {
//    char  *name;     // �ܼƦW�١G�d�ҡGperson, x, y, i, ....
    Array *fields;
} Struct;

// �Ÿ����ȥi��O byte, int, float �� pointer (�]�t struct, method, type*)
typedef union Value {
    BYTE  bvalue;
    int   ivalue;
    float fvalue;
    void  *pvalue;
} Value;

typedef struct Block { // �϶�
    char *name;        // �϶��W�� (��ƦW�١A���c�W�١A�Ϊ̬O�ΦW�϶�) 
    SymType symType;
//    char *tag;         // �϶��аO (METHOD, STRUCT, BLOCK)
/*    union {
        Struct *stru;
        Method *method;
    } u; */
    struct Block *parent; // ���϶�
} Block;

typedef struct Symbol { // �Ÿ� 
    Block *scope; // ���ݻ��
    char  *name;  // �ܼƩΫ��A�W�� 
    SymType symType; // SymMethod, SymStruct, SymBlock, SymBType �� SymVar
    union {
        Var    *var;
        Block  *block;
        Struct *stru;
        Method *method;
    } u;
//    Type  type;   // ���A���c
//    Value value;  // �ܼƭ�
    int   size;   // �Ÿ��Ҧ��Ŷ����j�p (char:1, int:4, float:4, ���:4, ���c:�Ҧ�����`�M) 
    int   offset; // �b���c������} (���c����}�B�ϰ��ܼƦ�}�B�ѼƦ�}) 
} Symbol;

/*
typedef struct SymTable {   // �Ÿ��� 
    Block *global;          // ����϶� Global 
    HashTable *table;       // �Ÿ��� 
} SymTable;
*/

#define SymTable HashTable

SymTable *SymTableNew();
void SymTablePutType(SymTable *st, char *name, int size);
Symbol *SymTablePut(SymTable *table, Symbol *sym);
Symbol* SymTableGet(SymTable *table, void *scope, char *name);
void SymTableFree(SymTable *table);
void SymTableDebug(SymTable *table);

Symbol *SymNew(Block *scope, char *name, SymType symType);
void SymDebug(void *data);
void SymFree(Symbol *s);

Block *BlockNew(void *parent, char *name, SymType symType);
void BlockFree(Block *block);

Method *MethodNew();
void MethodFree();

Struct *StructNew();
void StructFree();

Var *VarNew();
void VarFree(Var *v);

Block Global; // ����϶� Global 

#endif
