// ���n��ĥΤ��@�����v (Public Domain License)�A�z�i�H���N�ϥΥ��n��Ψ��l�X�C 
// ���@�̹�����]���n��Ҳ��ͪ��l�`�]�]�t�L�H�ק��ҳy�����l�`�^�A���t����d���C

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "Array.h"

// ========= ����� (��,��) ���c =============== 
typedef struct {
	char *key;  // ��
	void *data; // �� 
} Entry;

Entry* EntryNew(char *key, void *data); // �إ� (��,��) ���c 
int EntryCompare(Entry *e1, Entry *e2); // ��� (��,��) ���� (��)
void EntryFree(Entry *e);

int hash(char *key, int range); // ������ 

#define HashTable Array // ���������O�@�ӤG�����ʺA�}�C

// ========== ���ꪫ�󪺦������ =================
void HashTableTest(); // ������椸���ը�ơC 
HashTable* HashTableNew(int size); // �إߤj�p�� size ������� 
void HashTableFree(HashTable *table); // ��������� 
void HashTablePut(HashTable *table, char *key, void *data); // �N (��,��) ��J����� 
void *HashTableGet(HashTable *table, char *key); // �ھ�(��)���o�����������
void HashTableEach(HashTable *table, FuncPtr1 f); // ��C�Ӥ������I�s�@�� f ��ơC 
Array* HashTableToArray(HashTable *table); // �N�G����������ഫ���@���}�C 

#endif
