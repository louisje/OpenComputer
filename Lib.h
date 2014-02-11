// ���n��ĥΤ��@�����v (Public Domain License)�A�z�i�H���N�ϥΥ��n��Ψ��l�X�C 
// ���@�̹�����]���n��Ҳ��ͪ��l�`�]�]�t�L�H�ק��ҳy�����l�`�^�A���t����d���C

#ifndef LIB_H
#define LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

// ================== �򥻱`�� ===================
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

// ================== �򥻨�� ===================
#define min(x,y)         (x < y?x:y)    // ����̤������p��
#define max(x,y)         (x > y?x:y)    // ����̤������j��

// =================== �O������ ================
void* memNew(int size);     // �O����t�m���
void  memFree(void *ptr);   // �O����������
void  memCheck();           // �O�����ˬd���
#define ObjNew(type, count) memNew(count*sizeof(type)) // ���t����Ŷ� 
#define ObjFree memFree     // ���񪫥�Ŷ�
#define ObjCopy(to, from, type) memcpy(to, from, sizeof(type)) // ���t����Ŷ�

// =================== �r���� ================== 
char* strNew(char *str); // �إߦr�� 
#define strFree memFree  // ����r�� 
char* strSub(char *str, int i, int len); // �إߤl�r�� 
#define strEqual(str1, str2) (strcmp(str1, str2)==0)// �P�_��r��O�_�۵� 
#define strMember(ch, set) (strchr(set, ch) != NULL)// ���զr�� ch �O�_�b set �� 
char* strSubstr(char *substr, char *str, int i, int len);// ���X�l�r��
BOOL  strHead(char *str, char *head);   // ���r��}�Y 
BOOL  strTail(char *str, char *tail);   // ���r�굲�� 
BOOL  strPartOf(char *token, char *set); // �ˬd�Y�r��O�_�b���X��
int   strCountChar(char *str, char *charSet);
char* strReplace(char *str, char *fromSet, char to); // ���N�r���� from �� to �r��
char* strTrim(char *trimStr, char *str, char *set);  // �N�r���Y�����Y�r���M����
char* strFill(char *toStr, char ch, int n); // �N�r�� ch ���� n ��
char* strToUpper(char *str); // �N�r���ର�j�g
char* strToLower(char *str); // �N�r���ର�p�g
#define strNoNull(s) (s==NULL?"":s) // �קK�Ǧ^ NULL 

// ================== ���~�B�z ===================== 
#ifdef DEBUG
#define ASSERT(cond)     if (!(cond)) ERROR()  // �n�D����n���ߡA�_�h�N���
#else
#define ASSERT(cond)
#endif

void debug(const char *fmt, ...); // �L�X���~�T���C 
// ���~���i������ơA�|�L�X���@��I�s�F ERROR()
#define ERROR()	do { \
                  printf("ERROR => file:%s, line: %d\n", __FILE__, __LINE__);\
                  system("pause"); exit(1); \
                } while (1)

// =================== �ɮר�� ==================== 
BYTE* fileToBytes(char *fileName, int *sizePtr); // �N�ɮ�Ū�J���� BYTE �}�C 
char* fileToStr(char *fileName); // �N�ɮ�Ū�J�����r�� 

#endif

// ================== �r��`�� ================= 
#define SPACE " \t\n\r\t"     // �ťզr��
#define ALPHA "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" // �^��r��
#define DIGIT "0123456789"    // �Ʀr�r��
#define SYMBOL "~!@#$%^&*()_+{}:<>?-=[]|\"\\;',./" // �Ÿ��r��
