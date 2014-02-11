// 本軟體採用公共領域授權 (Public Domain License)，您可以任意使用本軟體及其原始碼。 
// 但作者對於任何因本軟體所產生的損害（包含他人修改後所造成的損害），不負任何責任。

#ifndef ARRAY_H
#define ARRAY_H

#include "Lib.h"

// ========= 動態陣列的資料結構 =================================== 
typedef struct {
	int size;    // 陣列分配大小
	int count;   // 陣列內含的元素個數 
  void **item; // 每個陣列元素的指標
} Array;

// ========= 函數指標 (用於ArrayEach(), HashTableEach()中) ======== 
typedef void (*FuncPtr1) (void *); // 一個參數的函數 
typedef int (*FuncPtr2) (void *, void *); // 兩個參數的函數 

void ArrayTest(); // 動態陣列的單元測試函數

// =========== 動態陣列物件 Array 的成員函數 ====================== 
Array* ArrayNew(int size);// 建立新陣列 
void ArrayFree(Array *array, FuncPtr1 freeFuncPtr); // 釋放該陣列 
void ArrayAdd(Array *array, void *item); // 新增一個元素 
void* ArrayGet(Array *array, int i); // 取得陣列的第 i 個元素 
void* ArrayLast(Array *array); // 取得最後一個元素 
void ArrayPush(Array *array,void *item); // (模擬堆疊) 推入一個元素 
void* ArrayPop(Array *array, FuncPtr1 objFree);  //(模擬堆疊) 彈出一個元素  
void* ArrayPeek(Array *array); //(模擬堆疊) 取得最上面的元素 
int ArrayFind(Array *array, void *data, FuncPtr2 fcmp); // 尋找陣列中的特定元素的位置
void ArrayEach(Array *array, FuncPtr1 f); //對每個元素都執行 f 函數

// =========== 與 ArrahEach() 搭配的字串函數 ====================== 
void strPrint(void *data);  // 列印字串
void strPrintln(void *data);// 列印字串並換行 

// ========= 字串分割函數 ========================================= 

// 分割選項，KEEP_SPLITER:保留分割字元，REMOVE_SPLITER:移除分割字元 
typedef enum { KEEP_SPLITER, REMOVE_SPLITER } SplitMode;

// 將字串根據 spliter 分割成字串陣列 
extern Array* split(char *str, char *spliter, SplitMode mode);

#endif
