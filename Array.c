// ���n��ĥΤ��@�����v (Public Domain License)�A�z�i�H���N�ϥΥ��n��Ψ��l�X�C 
// ���@�̹�����]���n��Ҳ��ͪ��l�`�]�]�t�L�H�ק��ҳy�����l�`�^�A���t����d���C
// �@�̡G�����

#include <stdlib.h>
#include <string.h>
#include "Array.h"

// �\��G�ʺA�}�C���椸���ը��
// �d�ҡGArrayTest();
void ArrayTest() {
	debug("=======ArrayTest()==========\n");
  char *names[] = { "John", "Mary", "George", "Bob" };
  Array *array = ArrayNew(1);
  int i;
	for (i=0; i<4; i++) // �N�Ҧ��W�r�[�J��}�C���C 
		ArrayAdd(array, names[i]);

	ArrayEach(array, strPrintln); // �L�X��Ӱ}�C 
	printf("ArrayPop()=%s\n", ArrayPop(array)); // ����|�ΡA�u�X�̫�@�� 
	printf("ArrayPeek()=%s\n", ArrayPeek(array)); // ����|�ΡA���o�̫�@��
	for (i=0; i<4; i++) { // �ݬݥ|�ӦW�r�O�_���٦b�}�C�� 
		int arrayIdx = ArrayFind(array, names[i], (FuncPtr2) strcmp);
    printf("ArrayFind(%s)=%d\n", names[i], arrayIdx);
  }
	ArrayEach(array, strPrintln); // �L�X��Ӱ}�C 
	ArrayFree(array, NULL); // �����Ӱ}�C
  
	Array *tokens = split("abc,def*ghi", ",*", REMOVE_SPLITER); // �N�r����ά��}�C 
	ArrayEach(tokens, strPrintln); // �L�X���ε��G 
	ArrayFree(tokens, strFree);
    memCheck(); // �i��O�����ˬd 
}

// �\��G�إ߷s���ʺA�}�C
// �d�ҡGArray *array = ArrayNew(10); 
Array* ArrayNew(int size) {
  Array *array = ObjNew(Array, 1);
  array->count = 0;
  array->size = size;
  array->item = ObjNew(void*, array->size);
  return array;
}

// �\��G����ʺA�}�C
// �d�ҡGArrayFree(array); 
void ArrayFree(Array *array, FuncPtr1 freeFuncPtr) {
  if (array == NULL) return;
  if (freeFuncPtr != NULL)
    ArrayEach(array, freeFuncPtr);
  ObjFree(array->item);
  ObjFree(array);
}

// �\��G�b�}�C���s�W�@�Ӥ���
// �d�ҡGArrayAdd(array, item);
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

// �\��G���o�}�C���� i �Ӥ��� 
// �d�ҡGArray *array = ArrayNew(10); 
void* ArrayGet(Array *array, int i) {
  if (i<array->count) return array->item[i]; else return NULL;
}

// �\��G���o�̫�@�Ӥ��� 
// �d�ҡGchar *token = ArrayLast(array);
void* ArrayLast(Array *array) {
	if (array->count > 0)
    	return array->item[array->count-1];
    else
        return NULL;
}

// �\��G(�������|) ���J�@�Ӥ���
// �d�ҡGArrayPush(array, item);
void ArrayPush(Array *array, void *item) {
  ArrayAdd(array, item);
}

// �\��G(�������|) �u�X�@�Ӥ���
// �d�ҡGchar *token = ArrayPop(array);
void* ArrayPop(Array *array) {
  ASSERT(array->count>0);
	void *top = ArrayLast(array);
	array->count--;
	return top;
}

// �\��G(�������|) ���o�̤W�������� 
// �d�ҡGchar *token = ArrayPeek(array); 
void* ArrayPeek(Array *array) {
  return ArrayLast(array);
}

// �\��G�M��}�C�����S�w��������m 
// �d�ҡGint i = ArrayFind(array, "John", strcmp);
int ArrayFind(Array *array, void *data, FuncPtr2 fcmp) {
	int i;
	for (i=0; i<array->count; i++)
		if (fcmp(array->item[i], data)==0)
			return i;
	return -1;
}

// �\��G��C�Ӥ��������� f ���
// �d�ҡGArrayEach(array, strPrintln);
void ArrayEach(Array *array, FuncPtr1 f) {
	int i;
	for (i=0; i<array->count; i++)
		f(array->item[i]);
}

// =========== �P ArrahEach() �f�t���r���� ====================== 
// �\��G�C�L�r��
void strPrint(void *data) {
	printf("%s ", data);
}

// �\��G�C�L�r��ô��� 
void strPrintln(void *data) {
	printf("%s\n", data);
}

// ============ �N�r����Φ��}�C����� ============================
// �\��G�N�r��ھ� spliter ���Φ��r��}�C 
// �d�ҡGArray *array = split("abc,def*ghi", ",*", REMOVE_SPLITER);
//       ���G�|�O�]�t {"abc", "def", "ghi"} ���ʺA�}�C
Array* split(char *str, char *spliter, SplitMode mode) {
	Array *tokens = ArrayNew(5);
  int si, tokenCount=0;
  int begin=0, ti = 0;
  for (si=0; si<=strlen(str); si++) {
		if (str[si]=='\0' || strMember(str[si], spliter)) { // �I����Φr�� 
        int len = si-begin;
        if (len > 0)
				ArrayAdd(tokens, strSub(str, begin, len)); // ���Ϋ�[�J�}�C�� 
        if (mode == KEEP_SPLITER)
				ArrayAdd(tokens, strSub(str, si, 1)); // �N���Φr���]�[�J�}�C�� 
        begin = si+1;
      }
  }
  return tokens;
}
