// ���n��ĥΤ��@�����v (Public Domain License)�A�z�i�H���N�ϥΥ��n��Ψ��l�X�C 
// ���@�̹�����]���n��Ҳ��ͪ��l�`�]�]�t�L�H�ק��ҳy�����l�`�^�A���t����d���C
// �@�̡G����� 2011.7.4

#ifndef SCANNER_H
#define SCANNER_H

#include "Lib.h"

typedef struct {        // ���y�������󵲺c              
  char *text;           //   ��J���{�� (text) 
	int len;               //   �{�����`����
	// �`�N�G�H�U�� xSave ���O�b ScannerStore() �P ScannerRestore() �ɨϥΪ��ƥ��C 
	int i, iSave;          //   �ثe���J����m 
	int line, lineSave;    //   �ثe���J���渹 
	int pos, posSave;      //   �ثe���J���_�l�I 
	char *tag, *tagSave;   //   ���J���аO 
	char token[100], tokenSave[100]; // �ثe�����J 
} Scanner;

void ScannerTest(char *fileName);   // Scanner ���J���R���q�����յ{���C 
Scanner* ScannerNew(char *pText);   // �إ߷s�����J���R Scanner ���� 
void ScannerFree(Scanner *s);       // ���� Scanner ���� 
void ScannerStore(Scanner *s);      // �x�s Scanner ���ثe���A 
void ScannerRestore(Scanner *s);    // ��_ Scanner ���x�s���A
BOOL ScannerIsNext(Scanner *s, char *pTags); // �ˬd�U�@�ӵ��J�O�_�ŦX tag �аO�C 
void ScannerNext(Scanner *s);       // ���o�U�@�ӵ��J (token) 
char ch(Scanner *s);                // ���o�ثe�r�� 
void cnext(Scanner *s);             // �e�i��U�@�Ӧr�� 
char *tokenToTag(char *token);      // ����J (token) �i��аO (tag)

// �ŧi Token �ܼơA�]�t����r if, for, �B��l ++, / �P �D�׺ݶ��� IF, FOR... 
#define DEF(var, str) extern char var[];
#include "Token.h"
#undef DEF

#endif
