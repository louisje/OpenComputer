// 本軟體採用公共領域授權 (Public Domain License)，您可以任意使用本軟體及其原始碼。 
// 但作者對於任何因本軟體所產生的損害（包含他人修改後所造成的損害），不負任何責任。

#include "HashTable.h"

// 功能：雜湊表的單元測試函數。 
// 範例：HashTableTest();
void HashTableTest() {
	debug("=======HashTableTest()==========\n");
	char *names[]= { "John","Mary","George","Mickey","Snoopy","Bob","Jack" };
  char *ids[]    = { "1",    "2",    "3",      "4",      "5",      "6",   "7" };
	HashTable* table = HashTableNew(3); // 建立一個具有三個串列的雜湊表。
  int i;
	for (i=0; i<5; i++) // 將 5 對 (名字， 代號) 逐一放入雜湊表中
    HashTablePut(table, names[i], ids[i]);
	for (i=0; i<7; i++) // 檢查 7 個名字是否存在，並取出其代號。 
	    debug("id=%s\n", HashTableGet(table, names[i]));
	debug("=======HashTableEach()==========\n");
	HashTableEach(table, strPrintln); // 印出雜湊表中的所有元素。 
	HashTableFree(table); // 釋放雜湊表 
    memCheck();	// 記憶體檢查 
}

// 功能：建立 (鍵,值) 結構 
// 範例：EntryNew("John", "0988-1234567");
Entry* EntryNew(char *key, void *data) {
  Entry* e = ObjNew(Entry, 1);
  e->key = key;
  e->data = data;
  return e;
}

// 功能：釋放 (鍵,值) 結構 
// 範例：EntryFree(e);
void EntryFree(Entry *e) {
  ObjFree(e);
}

// 功能：比較 (鍵,值) 中的 (鍵)
// 範例：int cmp = EntryCompare(e1, e2);
int EntryCompare(Entry *e1, Entry *e2) {
  return strcmp(e1->key, e2->key);
}

// 功能：雜湊函數
// 範例：hash("John", HashTable->size);
int hash(char *key, int range) {
	int i, hashCode=0;
	for (i=0; i<strlen(key); i++) {
		BYTE value = (BYTE) key[i];
		hashCode = (hashCode*7 + value) % range;
	}
	return hashCode;
}

// 功能：建立大小為 size 的雜湊表 
// 範例：HashTable *table = HashTableNew(17);
HashTable* HashTableNew(int size) {
  Array *table = ArrayNew(size);
  int i;
  for (i=0; i<table->size; i++)
    ArrayAdd(table, ArrayNew(1));
  return table;
}

// 功能：釋放雜湊表 
// 範例：HashTableFree(table);
void HashTableFree(HashTable *table) {
  int ti, hi;
  for (ti=0; ti<table->size; ti++) {
    Array *hitArray = table->item[ti];
    ArrayFree(hitArray, (FuncPtr1) EntryFree);
  }
  ArrayFree(table, NULL);
}

// 功能：尋找雜湊表中 key 所對應的元素並傳回
// 範例：char *tel = HashTableGet(table, name);
void *HashTableGet(HashTable *table, char *key) { 
    Entry keyEntry, *e;
  int slot = hash(key, table->size);            // 取得雜湊值 (列號) 
  Array *hitArray = (Array*) table->item[slot]; // 取得該列
	// 以下程式會找出該列中是否有包含 key 的配對後傳回 data
  keyEntry.key = key;
	int keyIdx = ArrayFind(hitArray, &keyEntry, (FuncPtr2) EntryCompare);
  if (keyIdx >= 0) { // 若有，則傳回該配對的資料元素 
		e = hitArray->item[keyIdx];
    return e->data;
  }
  return NULL; // 否則傳回 NULL 
}

// 功能：將 (key, data) 配對放入雜湊表中 
// 範例：HashTablePut(table, name, tel);
void HashTablePut(HashTable *table, char *key, void *data) {
    Entry keyEntry, *e;
  int slot = hash(key, table->size);            // 取得雜湊值 (列號) 
  Array *hitArray = (Array*) table->item[slot]; // 取得該列
  keyEntry.key = key;
	int keyIdx = ArrayFind(hitArray, &keyEntry, (FuncPtr2) EntryCompare);
  if (keyIdx >= 0) { // 若有，則傳回該配對的資料元素 
    e = hitArray->item[keyIdx];
    e->data = data;
  } else {
    e= EntryNew(key, data);// 建立配對 
    ArrayAdd(hitArray, e); // 放入對應的列中 
  }
}

// 功能：將 (key, data) 配對放入雜湊表中 
// 範例：HashTablePut(table, name, tel);
void HashTableEach(HashTable *table, FuncPtr1 f) {
  int i, j;
  for (i=0; i<table->count; i++) {
    Array *hits = table->item[i];
    for (j=0; j<hits->count; j++) {
      Entry *e = hits->item[j];
      f(e->data);
    }
  }
}

// 功能： 將二維的雜湊表轉換為一維陣列 
// 範例：Array *array = HashTableToArray(table);
Array* HashTableToArray(HashTable *table) {
  Array *array = ArrayNew(table->count);
  int i, j;
  for (i=0; i<table->count; i++) {
    Array *hits = table->item[i];
    for (j=0; j<hits->count; j++) {
      Entry *e = hits->item[j];
      ArrayAdd(array, e->data);
    }
  }
  return array;
}
