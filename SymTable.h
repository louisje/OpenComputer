// ���n��ĥΤ��@�����v (Public Domain License)�A�z�i�H���N�ϥΥ��n��Ψ��l�X�C 
// ���@�̹�����]���n��Ҳ��ͪ��l�`�]�]�t�L�H�ק��ҳy�����l�`�^�A���t����d���C
// �@�̡G�����

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "Lib.h"
#include "HashTable.h"
#include "Tree.h"

typedef char SymType;
#define SymBlock  'B'
#define SymMethod 'M'
#define SymStruct 'S'
#define SymVar    'V'
#define SymBType  'T'

// ���A Type ���G��ơB���c�P���лP�򥻫��A
//   �� �G int x;
//   ����  : int *px;
//   ���  : int total(int a[]) {...};  
//   ���c  : struct Person { ... };

typedef struct Symbol { // �Ÿ� 
    struct Symbol *scope; // ���ݻ��
    char    *name;  // �ܼƩΫ��A�W�� 
    SymType symType; // SymMethod, SymStruct, SymBlock, SymBType �� SymVar
    void    *typePtr; // ���A���c���e�G�i��O Var | BType | Block | Struct | Method ������
    int     size;   // �Ÿ��Ҧ��Ŷ����j�p (char:1, int:4, float:4, ���:4, ���c:�Ҧ�����`�M)  
} Symbol;

#define DIM_MAX 5
typedef struct PType {
    Symbol *typeSym;
    int starCount;  // �P�����ƶq (���мh��) 
    int dim[DIM_MAX], dimCount; // �}�C����
    int   offset; // �b���c������} (���c����}�B�ϰ��ܼƦ�}�B�ѼƦ�}) 
} PType;

#define PTR_SIZE 4
typedef struct BType {
    char temp; // �S���Ϊ���R�ܼ�,�קK�� BType ���j�p�� 0 
} BType;

typedef struct Block { // �϶����c 
    Array *vars;
} Block;

typedef struct Method {
    PType ret;
    Array *params;
    Symbol *block;
} Method;

typedef struct Struct {
    Array *fields;
//    Block *block;
} Struct;

// �Ÿ����ȥi��O byte, int, float �� pointer (�]�t struct, method, type*)
typedef union Value {
    BYTE  bvalue;
    int   ivalue;
    float fvalue;
    void  *pvalue;
} Value;

#define SymTable HashTable

SymTable *SymTableNew();
Symbol *SymTablePutBType(SymTable *st, char *name, int size);
Symbol *SymTablePut(SymTable *table, Symbol *sym);
Symbol *SymTableGet(SymTable *table, void *scope, char *name);
void SymTableFree(SymTable *table);
void SymTableDebug(SymTable *table);

Symbol *SymNew(Symbol *scope, char *name, SymType symType);
void SymFree(Symbol *s);

Block *BlockNew();
void BlockFree(Block *block);

Method *MethodNew();
void MethodFree();

Struct *StructNew();
void StructFree();

PType *PTypeNew();
void PTypeFree(PType *pt);
char *PTypeToStr(char *str, PType *ptype);

BType *BTypeNew();
void BTypeFree(BType *b);

void SymDebug(Symbol *s);
void PTypeDebug(PType *pt);

Symbol *Global /*����϶��Ÿ� */, *Int /*��ƫ��A�Ÿ�*/ , *Float /*�B�I�ƫ��A�Ÿ�*/ , *Char /*�r�����A�Ÿ�*/; 

#endif
