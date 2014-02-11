// 本軟體採用公共領域授權 (Public Domain License)，您可以任意使用本軟體及其原始碼。 
// 但作者對於任何因本軟體所產生的損害（包含他人修改後所造成的損害），不負任何責任。
// 作者：陳鍾誠

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

// 型態 Type 有：函數、結構與指標與基本型態
//   基本 ： int x;
//   指標  : int *px;
//   函數  : int total(int a[]) {...};  
//   結構  : struct Person { ... };

typedef struct Symbol { // 符號 
    struct Symbol *scope; // 所屬領域
    char    *name;  // 變數或型態名稱 
    SymType symType; // SymMethod, SymStruct, SymBlock, SymBType 或 SymVar
    void    *typePtr; // 型態結構內容：可能是 Var | BType | Block | Struct | Method 的指標
    int     size;   // 符號所佔空間的大小 (char:1, int:4, float:4, 函數:4, 結構:所有欄位總和)  
} Symbol;

#define DIM_MAX 5
typedef struct PType {
    Symbol *typeSym;
    int starCount;  // 星號的數量 (指標層次) 
    int dim[DIM_MAX], dimCount; // 陣列維度
    int   offset; // 在結構中的位址 (結構欄位位址、區域變數位址、參數位址) 
} PType;

#define PTR_SIZE 4
typedef struct BType {
    char temp; // 沒有用的填充變數,避免讓 BType 的大小為 0 
} BType;

typedef struct Block { // 區塊結構 
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

// 符號的值可能是 byte, int, float 或 pointer (包含 struct, method, type*)
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

Symbol *Global /*全域區塊符號 */, *Int /*整數型態符號*/ , *Float /*浮點數型態符號*/ , *Char /*字元型態符號*/; 

#endif
