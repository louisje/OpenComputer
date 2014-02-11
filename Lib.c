// 本軟體採用公共領域授權 (Public Domain License)，您可以任意使用本軟體及其原始碼。 
// 但作者對於任何因本軟體所產生的損害（包含他人修改後所造成的損害），不負任何責任。

#include "Lib.h"

void LibTest() {
	debug("=======LibTest()==========\n");
    char str[100];
    char *abc = strNew("abc");
    debug("strNew(abc)=%s\n", abc);
    char *sub = strSub("0123456789", 3, 2);
    debug("strSub(0123456789, 3, 2)=%s\n", sub);
    debug("strHead(abcdefg, abc)=%d\n", strHead("abcdefg", "abc"));
    debug("strTail(abcdefg, efg)=%d\n", strTail("abcdefg", "efg"));
    debug("strCountChar(aabbccdd, bc)=%d\n", strCountChar("aabbccdd", "bc"));
    debug("strEqual(abc, abc)=%d\n", strEqual("abc", "abc"));
    debug("strMember(b, abc)=%d\n", strMember('b', "abc"));
    debug("strPartOf(int, |int|str|)=%d\n", strPartOf("int","|int|str|"));
    debug("strSubstr(str, 01234567,3,4)=%s\n", strSubstr(str, "01234567",3,4));
    debug("strTrim(str, ***abc+++, *+)=%s\n", strTrim(str, "***abc+++", "*+"));
    debug("strFill(str, 'a', 5)=%s\n", strFill(str, 'a', 5));
    strcpy(str, "abcabc");
    debug("strReplace(str, abcabc, bc, *)=%s\n", strReplace(str, "bc", '*'));
    strcpy(str, "aBcDe");
    debug("strToUpper(str, aBcDe)=%s\n", strToUpper(str));
    strcpy(str, "aBcDe");
    debug("strToLower(str, aBcDe)=%s\n", strToLower(str));
    strFree(sub);
    strFree(abc);
    char *text = fileToStr("Lib.c");
    debug("text:len=%d [1..10]=%s\n", strlen(text),strSubstr(str, text,1,10));
    strFree(text);
    memCheck();
}

// ================== 記憶體管理函數 ============================ 
#ifdef DEBUG
int memNewCount = 0; // 總共分配了幾次記憶體。 
int memFreeCount=0; // 總共釋放了幾次記憶體。 
#endif

// 功能：記憶體配置函數，分配空間並清除為 0，並記錄分配次數 
void* memNew(int size) {
	void *ptr=malloc(size); // 分配記憶體 
	ASSERT(ptr != NULL);    // 如果分配失敗則錯誤 
	memset(ptr, 0, size);   // 將記憶體內容清空為零。 
#ifdef DEBUG
	memNewCount++;          // 將分配次數加一 
#endif
	return ptr;             // 傳回分配的記憶體指標 
}

// 功能：記憶體釋放函數，並記錄釋放次數
void memFree(void *ptr) {
	ASSERT(ptr != NULL);    // 不可釋放未分配的記憶體 
    free(ptr);              // 釋放記憶體 
#ifdef DEBUG
	memFreeCount++;         // 將釋放次數加一 
#endif
}

// 功能：記憶體檢查函數，看看分配次數與釋放次數是否相同
// => 如果相同則大致上應該沒有錯誤 
// => 當然有很小的機會還是有錯，只是分配與釋放次數恰巧相同而已
void memCheck() { 
#ifdef DEBUG
	debug("Memory:newCount=%d freeCount=%d\n", memNewCount, memFreeCount); 
	ASSERT(memNewCount == memFreeCount); // 如果分配與釋放次數不同，那就有錯 
#endif
}

// ================== 檔案管理函數 ============================ 
// 功能：將檔案讀入成為一個 BYTE 區塊 
BYTE* fileToBytes(char *fileName, int *sizePtr) {
	FILE *file = fopen(fileName, "rb");    // 開啟檔案 
	fseek(file, 0 , SEEK_END);             // 移到檔尾
	long size = ftell(file);               // 檔尾位置 = 檔案大小 
	rewind(file);                          // 回到檔案開頭 
	BYTE *buffer = (BYTE*) memNew(size+1); // 分配區塊 
	fread (buffer,size,1,file);            // 將檔案讀到該區塊中 
	fclose(file);                          // 關閉檔案 
	*sizePtr = size;                       // 傳回檔案大小 
	return buffer;                         // 傳回該區塊 
}

// 功能：將檔案讀入成為一個字串 
char* fileToStr(char *fileName) {
	int size;                                      // 檔案大小 
	BYTE *buffer = fileToBytes(fileName, &size);   // 將檔案讀入到 BYTE 區塊中 
	buffer[size] = '\0';                           // 設定字串結尾字元 
	return (char*) buffer;                         // 傳回該字串 
}

// ================== 字串操作函數 ========================
// 功能：建立新字串 (複製一份) 
// 範例：strNew("abc") 會傳回 "abc"
char *strNew(char *str) {
	char *rzStr = memNew(strlen(str)+1);
  strcpy(rzStr, str);
  return rzStr;
}

// 功能：建立子字串
// 範例：strSub("0123456789", 3, 2) 會傳回 "34"
char *strSub(char *str, int i, int len) {
	char *rzStr = memNew(len+1);
  strSubstr(rzStr, str, i, len);
  return rzStr;
}

// 功能：檢查字串開頭是否為 head 
// 範例：strHead("abcdefg", "abc") 會傳回 TRUE
BOOL strHead(char *str, char *head) { 
  return (strncmp(str, head, strlen(head))==0);
}

// 功能：檢查字串結尾是否為 tail
// 範例：strTail("abcdefg", "efg") 會傳回 TRUE
BOOL strTail(char *str, char *tail) {
  int strLen = strlen(str), tailLen = strlen(tail);
  if (strLen < tailLen) return FALSE;
  return (strcmp(str+strLen-tailLen, tail)==0);
}

// 功能：切割字串為兩部分 
// 範例：strCut("abcd/efg", "/", head, tail) 會設定 head = abcd, tail=efg 
void strCut(char *str, char *spliter, char *head, char *tail) {
    int i, len = strlen(str);
    strcpy(head, str);
    for (i=0; i<len; i++) {
        if (strMember(str[i], spliter)) {
            if (head != NULL)
                strSubstr(head, str, 0, i);
            if (tail != NULL)
                strSubstr(tail, str, i+1, len-(i+1));
            break;
        }
    }
}

// 功能：計算字串中有幾個 charSet 中的字元 
// 範例：strCountChar("aabbccdd", "bc") 會傳回 4 
int strCountChar(char *str, char *charSet) {
  int i, count=0;
  for (i=0; i<strlen(str); i++)
    if (strMember(str[i], charSet))
      count++;
  return count;
}

// 功能：將子字串複製到 substr 中。 
// 範例：strSubstr(substr, "0123456789", 3, 4) 會將 "3456" 放入 substr
char* strSubstr(char *substr, char *str, int i, int len) {
    ASSERT(len >= 0);
  strncpy(substr, &str[i], len);
  substr[len]='\0';
	return substr;
}
 
// 功能：檢查 token 是否為集合字串 set 中的元素 
// 範例：strPartOf("int", "byte|int|float") 會傳回 TRUE 
BOOL strPartOf(char *token, char *set) {
  ASSERT(token != NULL && set != NULL);
	ASSERT(strlen(token) < MAX_LEN);
	char ttoken[MAX_LEN+2];
  sprintf(ttoken, "|%s|", token);
  return (strstr(set, ttoken)!=NULL);
}

// 功能：將字串頭尾的某字元清除掉 
// 範例：strTrim(trimStr, "***abc+++", "*+") 會將 trimStr 設為 "abc" 
char* strTrim(char *trimStr, char *str, char *set) {
  char *start, *stop;
  for (start = str; strMember(*start, set); start++);
  for (stop = str+strlen(str)-1; stop > str && strMember(*stop, set); stop--);
  if (start <= stop) {
    strncpy(trimStr, start, stop-start+1);
    trimStr[stop-start+1]='\0';
  } else
    strcpy(trimStr, "");
	return trimStr;
}

// 功能：字串取代，將 str 中 fromSet 集合內的字元改為 to
// 範例：strReplace("abcabc", "bc", '*') 會將 "abcabc" 改為 "a**a**"
char* strReplace(char *str, char *fromSet, char to) {
  int i;
	for (i=0; i<strlen(str); i++) {
		if (strMember(str[i], fromSet))
      str[i] = to;
    }
	return str;
}

// 功能：將字元 ch 重複 n 次，以字串形式傳回 
// 範例：strFill(str, 'a', 5) 會將 str 設定為 "aaaaa" 
char* strFill(char *str, char ch, int n) {
	memset(str, ch, n);
	str[n] = '\0';
	return str;
}

// 功能：將字串轉為大寫 
// 範例：strToUpper("aBcDe") 會傳回 "ABCDE"
char* strToUpper(char *str) {
  int i;
  for (i = 0; i<strlen(str); i++)
    str[i] = toupper(str[i]);
	return str;
}

// 功能：將字串轉為小寫 
// 範例：strToLower("aBcDe") 會傳回 "abcde"
char* strToLower(char *str) {
	int i;
	for (i = 0; i<strlen(str); i++)
		str[i] = tolower(str[i]);
	return str;
}

// ================== 錯誤處理 ===================== 
// 功能：印出錯誤訊息，用法與 printf 相同。 
void debug(const char *fmt, ...) {
#ifdef DEBUG
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
#endif
}
