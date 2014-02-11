// 本軟體採用公共領域授權 (Public Domain License)，您可以任意使用本軟體及其原始碼。 
// 但作者對於任何因本軟體所產生的損害（包含他人修改後所造成的損害），不負任何責任。

#ifndef LIB_H
#define LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

// ================== 基本常數 ===================
#define FALSE   0
#define TRUE    1
#define BYTE    unsigned char
#define BOOL    unsigned char
#define INT32   int
#define INT16   short
#define INT8    char
#define UINT32  unsigned int
#define UINT16  unsigned short
#define UINT8  unsigned char
#define MAX_LEN 512

// ================== 基本函數 ===================
#define min(x,y)         (x < y?x:y)    // 取兩者之中較小者
#define max(x,y)         (x > y?x:y)    // 取兩者之中較大者

// =================== 記憶體函數 ================
void* memNew(int size);     // 記憶體配置函數
void  memFree(void *ptr);   // 記憶體釋放函數
void  memCheck();           // 記憶體檢查函數
#define ObjNew(type, count) memNew(count*sizeof(type)) // 分配物件空間 
#define ObjFree memFree     // 釋放物件空間
#define ObjCopy(to, from, type) memcpy(to, from, sizeof(type)) // 分配物件空間

// =================== 字串函數 ================== 
char* strNew(char *str); // 建立字串 
#define strFree memFree  // 釋放字串 
char* strSub(char *str, int i, int len); // 建立子字串 
#define strEqual(str1, str2) (strcmp(str1, str2)==0)// 判斷兩字串是否相等 
#define strMember(ch, set) (strchr(set, ch) != NULL)// 測試字元 ch 是否在 set 當中 
char* strSubstr(char *substr, char *str, int i, int len);// 取出子字串
BOOL  strHead(char *str, char *head);   // 比對字串開頭 
BOOL  strTail(char *str, char *tail);   // 比對字串結尾 
BOOL  strPartOf(char *token, char *set); // 檢查某字串是否在集合中
int   strCountChar(char *str, char *charSet);
char* strReplace(char *str, char *fromSet, char to); // 取代字元集 from 為 to 字元
char* strTrim(char *trimStr, char *str, char *set);  // 將字串頭尾的某字元清除掉
char* strFill(char *toStr, char ch, int n); // 將字元 ch 重複 n 次
char* strToUpper(char *str); // 將字串轉為大寫
char* strToLower(char *str); // 將字串轉為小寫
#define strNoNull(s) (s==NULL?"":s) // 避免傳回 NULL 

// ================== 錯誤處理 ===================== 
#ifdef DEBUG
#define ASSERT(cond)     if (!(cond)) ERROR()  // 要求條件要成立，否則就算錯
#else
#define ASSERT(cond)
#endif

void debug(const char *fmt, ...); // 印出錯誤訊息。 
// 錯誤報告巨集函數，會印出哪一行呼叫了 ERROR()
#define ERROR()	do { \
                  printf("ERROR => file:%s, line: %d\n", __FILE__, __LINE__);\
                  system("pause"); exit(1); \
                } while (1)

// =================== 檔案函數 ==================== 
BYTE* fileToBytes(char *fileName, int *sizePtr); // 將檔案讀入成為 BYTE 陣列 
char* fileToStr(char *fileName); // 將檔案讀入成為字串 

#endif

// ================== 字串常數 ================= 
#define SPACE " \t\n\r\t"     // 空白字元
#define ALPHA "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" // 英文字母
#define DIGIT "0123456789"    // 數字字元
#define SYMBOL "~!@#$%^&*()_+{}:<>?-=[]|\"\\;',./" // 符號字元
