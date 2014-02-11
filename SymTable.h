#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "Lib.h"
#include "HashTable.h"
#include "Tree.h"

// 型態 Type 有：函數、結構與指標與基本型態
//   基本 ： int x;
//   指標  : int *px;
//   函數  : int total(int a[]) {...};  
//   結構  : struct Person { ... };

typedef enum { SymVar, SymBlock, SymStruct, SymMethod, SymBType } SymType;

#define DIM_MAX 5
typedef struct Var {
//    char *name;     // 變數名稱：範例：person, x, y, i, ....
    char *typeName; // 型態名稱：int, float, StructID, MethodID, ...
    void *typePtr;
    int starCount;  // 星號的數量 (指標層次) 
    int dim[DIM_MAX], dimCount; // 陣列維度
} Var;

typedef struct Method {
//    char  *name;     // 變數名稱：範例：person, x, y, i, ....
    Var   ret;
    Array *params;
} Method;

typedef struct Struct {
//    char  *name;     // 變數名稱：範例：person, x, y, i, ....
    Array *fields;
} Struct;

// 符號的值可能是 byte, int, float 或 pointer (包含 struct, method, type*)
typedef union Value {
    BYTE  bvalue;
    int   ivalue;
    float fvalue;
    void  *pvalue;
} Value;

typedef struct Block { // 區塊
    char *name;        // 區塊名稱 (函數名稱，結構名稱，或者是匿名區塊) 
    SymType symType;
//    char *tag;         // 區塊標記 (METHOD, STRUCT, BLOCK)
/*    union {
        Struct *stru;
        Method *method;
    } u; */
    struct Block *parent; // 父區塊
} Block;

typedef struct Symbol { // 符號 
    Block *scope; // 所屬領域
    char  *name;  // 變數或型態名稱 
    SymType symType; // SymMethod, SymStruct, SymBlock, SymBType 或 SymVar
    union {
        Var    *var;
        Block  *block;
        Struct *stru;
        Method *method;
    } u;
//    Type  type;   // 型態結構
//    Value value;  // 變數值
    int   size;   // 符號所佔空間的大小 (char:1, int:4, float:4, 函數:4, 結構:所有欄位總和) 
    int   offset; // 在結構中的位址 (結構欄位位址、區域變數位址、參數位址) 
} Symbol;

/*
typedef struct SymTable {   // 符號表 
    Block *global;          // 全域區塊 Global 
    HashTable *table;       // 符號表 
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

Block Global; // 全域區塊 Global 

#endif
