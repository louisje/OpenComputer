// ���n��ĥΤ��@�����v (Public Domain License)�A�z�i�H���N�ϥΥ��n��Ψ��l�X�C 
// ���@�̹�����]���n��Ҳ��ͪ��l�`�]�]�t�L�H�ק��ҳy�����l�`�^�A���t����d���C

#ifndef ARRAY_H
#define ARRAY_H

#include "Lib.h"

// ========= �ʺA�}�C����Ƶ��c =================================== 
typedef struct {
	int size;    // �}�C���t�j�p
	int count;   // �}�C���t�������Ӽ� 
  void **item; // �C�Ӱ}�C����������
} Array;

// ========= ��ƫ��� (�Ω�ArrayEach(), HashTableEach()��) ======== 
typedef void (*FuncPtr1) (void *); // �@�ӰѼƪ���� 
typedef int (*FuncPtr2) (void *, void *); // ��ӰѼƪ���� 

void ArrayTest(); // �ʺA�}�C���椸���ը��

// =========== �ʺA�}�C���� Array ��������� ====================== 
Array* ArrayNew(int size);// �إ߷s�}�C 
void ArrayFree(Array *array, FuncPtr1 freeFuncPtr); // ����Ӱ}�C 
void ArrayAdd(Array *array, void *item); // �s�W�@�Ӥ��� 
void* ArrayGet(Array *array, int i); // ���o�}�C���� i �Ӥ��� 
void* ArrayLast(Array *array); // ���o�̫�@�Ӥ��� 
void ArrayPush(Array *array,void *item); // (�������|) ���J�@�Ӥ��� 
void* ArrayPop(Array *array, FuncPtr1 objFree);  //(�������|) �u�X�@�Ӥ���  
void* ArrayPeek(Array *array); //(�������|) ���o�̤W�������� 
int ArrayFind(Array *array, void *data, FuncPtr2 fcmp); // �M��}�C�����S�w��������m
void ArrayEach(Array *array, FuncPtr1 f); //��C�Ӥ��������� f ���

// =========== �P ArrahEach() �f�t���r���� ====================== 
void strPrint(void *data);  // �C�L�r��
void strPrintln(void *data);// �C�L�r��ô��� 

// ========= �r����Ψ�� ========================================= 

// ���οﶵ�AKEEP_SPLITER:�O�d���Φr���AREMOVE_SPLITER:�������Φr�� 
typedef enum { KEEP_SPLITER, REMOVE_SPLITER } SplitMode;

// �N�r��ھ� spliter ���Φ��r��}�C 
extern Array* split(char *str, char *spliter, SplitMode mode);

#endif
