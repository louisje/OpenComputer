#ifndef LIB_H
#define LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

// �򥻱`�� 
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

// �򥻨�� 
#define min(x,y)         (x < y?x:y)
#define max(x,y)         (x > y?x:y)
#define ASSERT(cond)     assert(cond)
#define ObjNew(type, count) newMemory(count*sizeof(type))
#define ObjFree freeMemory
#define strFree freeMemory
#define ERROR()	do { printf("ERROR => file:%s, line: %d\n", __FILE__, __LINE__);system("pause");exit(1); } while (1)

void* newMemory(int size);
void freeMemory(void *ptr);
void checkMemory();
char *newStr(char *str);
char *newSubstr(char *str, int i, int len);
BYTE* fileToBytes(char *fileName, int *sizePtr);
BYTE* fileToBytesMin(char *fileName, int *sizePtr, int minSize);
char* fileToStr(char *fileName);

// �r���� 
#define strEqual(str1, str2) (strcmp(str1, str2)==0)
#define strMember(ch, set) (strchr(set, ch) != NULL)
void strSubstr(char *substr, char *str, int i, int len);
void strReplace(char *str, char *from, char to);
void strTrim(char *trimStr, char *str, char *set);
char *strSpaces(int len);
void strToUpper(char *str);
BOOL strPartOf(char *token, char *set);
void strPrint(void *data);
void strPrintln(void *data);

// ���~�B�z
void debug(const char *fmt, ...);

// ��ƫ��� (�Ω�ArrayEach(), HashTableEach()��)
typedef void (*FuncPtr1) (void *);
typedef int (*FuncPtr2) (void *, void *);

extern char SPLITER[];
extern char SPACE[];
extern char ALPHA[];
extern char DIGIT[];

#endif
