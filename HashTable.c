// ���n��ĥΤ��@�����v (Public Domain License)�A�z�i�H���N�ϥΥ��n��Ψ��l�X�C 
// ���@�̹�����]���n��Ҳ��ͪ��l�`�]�]�t�L�H�ק��ҳy�����l�`�^�A���t����d���C

#include "HashTable.h"

// �\��G������椸���ը�ơC 
// �d�ҡGHashTableTest();
void HashTableTest() {
	debug("=======HashTableTest()==========\n");
	char *names[]= { "John","Mary","George","Mickey","Snoopy","Bob","Jack" };
  char *ids[]    = { "1",    "2",    "3",      "4",      "5",      "6",   "7" };
	HashTable* table = HashTableNew(3); // �إߤ@�Ө㦳�T�Ӧ�C�������C
  int i;
	for (i=0; i<5; i++) // �N 5 �� (�W�r�A �N��) �v�@��J�����
    HashTablePut(table, names[i], ids[i]);
	for (i=0; i<7; i++) // �ˬd 7 �ӦW�r�O�_�s�b�A�è��X��N���C 
	    debug("id=%s\n", HashTableGet(table, names[i]));
	debug("=======HashTableEach()==========\n");
	HashTableEach(table, strPrintln); // �L�X��������Ҧ������C 
	HashTableFree(table); // ��������� 
    memCheck();	// �O�����ˬd 
}

// �\��G�إ� (��,��) ���c 
// �d�ҡGEntryNew("John", "0988-1234567");
Entry* EntryNew(char *key, void *data) {
  Entry* e = ObjNew(Entry, 1);
  e->key = key;
  e->data = data;
  return e;
}

// �\��G���� (��,��) ���c 
// �d�ҡGEntryFree(e);
void EntryFree(Entry *e) {
  ObjFree(e);
}

// �\��G��� (��,��) ���� (��)
// �d�ҡGint cmp = EntryCompare(e1, e2);
int EntryCompare(Entry *e1, Entry *e2) {
  return strcmp(e1->key, e2->key);
}

// �\��G������
// �d�ҡGhash("John", HashTable->size);
int hash(char *key, int range) {
	int i, hashCode=0;
	for (i=0; i<strlen(key); i++) {
		BYTE value = (BYTE) key[i];
		hashCode = (hashCode*7 + value) % range;
	}
	return hashCode;
}

// �\��G�إߤj�p�� size ������� 
// �d�ҡGHashTable *table = HashTableNew(17);
HashTable* HashTableNew(int size) {
  Array *table = ArrayNew(size);
  int i;
  for (i=0; i<table->size; i++)
    ArrayAdd(table, ArrayNew(1));
  return table;
}

// �\��G��������� 
// �d�ҡGHashTableFree(table);
void HashTableFree(HashTable *table) {
  int ti, hi;
  for (ti=0; ti<table->size; ti++) {
    Array *hitArray = table->item[ti];
    ArrayFree(hitArray, (FuncPtr1) EntryFree);
  }
  ArrayFree(table, NULL);
}

// �\��G�M������� key �ҹ����������öǦ^
// �d�ҡGchar *tel = HashTableGet(table, name);
void *HashTableGet(HashTable *table, char *key) { 
    Entry keyEntry, *e;
  int slot = hash(key, table->size);            // ���o����� (�C��) 
  Array *hitArray = (Array*) table->item[slot]; // ���o�ӦC
	// �H�U�{���|��X�ӦC���O�_���]�t key ���t���Ǧ^ data
  keyEntry.key = key;
	int keyIdx = ArrayFind(hitArray, &keyEntry, (FuncPtr2) EntryCompare);
  if (keyIdx >= 0) { // �Y���A�h�Ǧ^�Ӱt�諸��Ƥ��� 
		e = hitArray->item[keyIdx];
    return e->data;
  }
  return NULL; // �_�h�Ǧ^ NULL 
}

// �\��G�N (key, data) �t���J����� 
// �d�ҡGHashTablePut(table, name, tel);
void HashTablePut(HashTable *table, char *key, void *data) {
    Entry keyEntry, *e;
  int slot = hash(key, table->size);            // ���o����� (�C��) 
  Array *hitArray = (Array*) table->item[slot]; // ���o�ӦC
  keyEntry.key = key;
	int keyIdx = ArrayFind(hitArray, &keyEntry, (FuncPtr2) EntryCompare);
  if (keyIdx >= 0) { // �Y���A�h�Ǧ^�Ӱt�諸��Ƥ��� 
    e = hitArray->item[keyIdx];
    e->data = data;
  } else {
    e= EntryNew(key, data);// �إ߰t�� 
    ArrayAdd(hitArray, e); // ��J�������C�� 
  }
}

// �\��G�N (key, data) �t���J����� 
// �d�ҡGHashTablePut(table, name, tel);
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

// �\��G �N�G����������ഫ���@���}�C 
// �d�ҡGArray *array = HashTableToArray(table);
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
