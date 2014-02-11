// 本軟體採用公共領域授權 (Public Domain License)，您可以任意使用本軟體及其原始碼。 
// 但作者對於任何因本軟體所產生的損害（包含他人修改後所造成的損害），不負任何責任。

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "Array.h"

// ========= 雜湊表的 (鍵,值) 結構 =============== 
typedef struct {
	char *key;  // 鍵
	void *data; // 值 
} Entry;

Entry* EntryNew(char *key, void *data); // 建立 (鍵,值) 結構 
int EntryCompare(Entry *e1, Entry *e2); // 比較 (鍵,值) 中的 (鍵)
void EntryFree(Entry *e);

int hash(char *key, int range); // 雜湊函數 

#define HashTable Array // 本雜湊表其實是一個二維的動態陣列

// ========== 雜湊物件的成員函數 =================
void HashTableTest(); // 雜湊表的單元測試函數。 
HashTable* HashTableNew(int size); // 建立大小為 size 的雜湊表 
void HashTableFree(HashTable *table); // 釋放雜湊表 
void HashTablePut(HashTable *table, char *key, void *data); // 將 (鍵,值) 放入雜湊表中 
void *HashTableGet(HashTable *table, char *key); // 根據(鍵)取得雜湊表中的元素
void HashTableEach(HashTable *table, FuncPtr1 f); // 對每個元素都呼叫一次 f 函數。 
Array* HashTableToArray(HashTable *table); // 將二維的雜湊表轉換為一維陣列 

#endif
