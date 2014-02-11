// 本軟體採用公共領域授權 (Public Domain License)，您可以任意使用本軟體及其原始碼。 
// 但作者對於任何因本軟體所產生的損害（包含他人修改後所造成的損害），不負任何責任。
// 作者：陳鍾誠

#include <stdlib.h>
#include <string.h>
#include "Array.h"

// 功能：動態陣列的單元測試函數
// 範例：ArrayTest();
void ArrayTest() {
	debug("=======ArrayTest()==========\n");
  char *names[] = { "John", "Mary", "George", "Bob" };
  Array *array = ArrayNew(1);
  int i;
	for (i=0; i<4; i++) // 將所有名字加入到陣列中。 
		ArrayAdd(array, names[i]);

	ArrayEach(array, strPrintln); // 印出整個陣列 
	printf("ArrayPop()=%s\n", ArrayPop(array)); // 當堆疊用，彈出最後一個 
	printf("ArrayPeek()=%s\n", ArrayPeek(array)); // 當堆疊用，取得最後一個
	for (i=0; i<4; i++) { // 看看四個名字是否都還在陣列中 
		int arrayIdx = ArrayFind(array, names[i], (FuncPtr2) strcmp);
    printf("ArrayFind(%s)=%d\n", names[i], arrayIdx);
  }
	ArrayEach(array, strPrintln); // 印出整個陣列 
	ArrayFree(array, NULL); // 釋放整個陣列
  
	Array *tokens = split("abc,def*ghi", ",*", REMOVE_SPLITER); // 將字串分割為陣列 
	ArrayEach(tokens, strPrintln); // 印出分割結果 
	ArrayFree(tokens, strFree);
    memCheck(); // 進行記憶體檢查 
}

// 功能：建立新的動態陣列
// 範例：Array *array = ArrayNew(10); 
Array* ArrayNew(int size) {
  Array *array = ObjNew(Array, 1);
  array->count = 0;
  array->size = size;
  array->item = ObjNew(void*, array->size);
  return array;
}

// 功能：釋放動態陣列
// 範例：ArrayFree(array); 
void ArrayFree(Array *array, FuncPtr1 freeFuncPtr) {
  if (array == NULL) return;
  if (freeFuncPtr != NULL)
    ArrayEach(array, freeFuncPtr);
  ObjFree(array->item);
  ObjFree(array);
}

// 功能：在陣列中新增一個元素
// 範例：ArrayAdd(array, item);
void ArrayAdd(Array *array, void *item) {
  ASSERT(array->count <= array->size);
  if (array->count == array->size) {
    int newSize = array->size*2;
    void **newItems = ObjNew(void*, newSize);
    memcpy(newItems, array->item, array->size*sizeof(void*));
    ObjFree(array->item);
    array->item = newItems;
    array->size = newSize;
  }
  array->item[array->count++] = item;
}

// 功能：取得陣列的第 i 個元素 
// 範例：Array *array = ArrayNew(10); 
void* ArrayGet(Array *array, int i) {
  if (i<array->count) return array->item[i]; else return NULL;
}

// 功能：取得最後一個元素 
// 範例：char *token = ArrayLast(array);
void* ArrayLast(Array *array) {
	if (array->count > 0)
    	return array->item[array->count-1];
    else
        return NULL;
}

// 功能：(模擬堆疊) 推入一個元素
// 範例：ArrayPush(array, item);
void ArrayPush(Array *array, void *item) {
  ArrayAdd(array, item);
}

// 功能：(模擬堆疊) 彈出一個元素
// 範例：char *token = ArrayPop(array);
void* ArrayPop(Array *array) {
  ASSERT(array->count>0);
	void *top = ArrayLast(array);
	array->count--;
	return top;
}

// 功能：(模擬堆疊) 取得最上面的元素 
// 範例：char *token = ArrayPeek(array); 
void* ArrayPeek(Array *array) {
  return ArrayLast(array);
}

// 功能：尋找陣列中的特定元素的位置 
// 範例：int i = ArrayFind(array, "John", strcmp);
int ArrayFind(Array *array, void *data, FuncPtr2 fcmp) {
	int i;
	for (i=0; i<array->count; i++)
		if (fcmp(array->item[i], data)==0)
			return i;
	return -1;
}

// 功能：對每個元素都執行 f 函數
// 範例：ArrayEach(array, strPrintln);
void ArrayEach(Array *array, FuncPtr1 f) {
	int i;
	for (i=0; i<array->count; i++)
		f(array->item[i]);
}

// =========== 與 ArrahEach() 搭配的字串函數 ====================== 
// 功能：列印字串
void strPrint(void *data) {
	printf("%s ", data);
}

// 功能：列印字串並換行 
void strPrintln(void *data) {
	printf("%s\n", data);
}

// ============ 將字串分割成陣列的函數 ============================
// 功能：將字串根據 spliter 分割成字串陣列 
// 範例：Array *array = split("abc,def*ghi", ",*", REMOVE_SPLITER);
//       結果會是包含 {"abc", "def", "ghi"} 的動態陣列
Array* split(char *str, char *spliter, SplitMode mode) {
	Array *tokens = ArrayNew(5);
  int si, tokenCount=0;
  int begin=0, ti = 0;
  for (si=0; si<=strlen(str); si++) {
		if (str[si]=='\0' || strMember(str[si], spliter)) { // 碰到分割字元 
        int len = si-begin;
        if (len > 0)
				ArrayAdd(tokens, strSub(str, begin, len)); // 分割後加入陣列中 
        if (mode == KEEP_SPLITER)
				ArrayAdd(tokens, strSub(str, si, 1)); // 將分割字元也加入陣列中 
        begin = si+1;
      }
  }
  return tokens;
}
