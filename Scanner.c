// ���n��ĥΤ��@�����v (Public Domain License)�A�z�i�H���N�ϥΥ��n��Ψ��l�X�C 
// ���@�̹�����]���n��Ҳ��ͪ��l�`�]�]�t�L�H�ק��ҳy�����l�`�^�A���t����d���C
// �@�̡G����� 2011.7.4

#include <string.h>
#include "Scanner.h"

// �ŧi����r���r���ܼơA���O char kIF[]="if"; ...char EXP[]="EXP";...
#define DEF(var, str) char var[]=str;
#include "Token.h"
#undef DEF

// �ŧi����r�}�C�A gTagList={...,"if", ...,"EXP", ... };
char *gTokenList[] = {
#define DEF(var, str) var,
#include "Token.h"
#undef DEF
};

// �\��GScanner ���J���R���q�����յ{���C
// �d�ҡGScannerTest("test.c1");
void ScannerTest(char *fileName) {
	debug("======================ScannerTest()=========================\n");    
    char *text = fileToStr(fileName); // Ū����ӵ{���ɡA�����@�Ӧr�� text 
    Scanner *s = ScannerNew(text); // �إ� Scanner ���� 
    while (TRUE) { // ���_���y���J�A�����ɮ׵��� 
        ScannerNext(s); // ���o�U�@�ӵ��J
        debug("token=%-10s tag=%-10s line=%-4d pos=%-3d\n", 
              s->token, s->tag, s->line, s->pos);
        if (s->tag == kEND) // �w�g��{������
            break; // �������y 
    }
    ScannerFree(s); // ���� Scanner ���� 
    strFree(text); // ����r�� text
    memCheck(); // �ˬd�O���� 
}

// �\��G�إ߷s�����J���R Scanner ���� 
// �d�ҡGScanner *s = ScannerNew(text);
Scanner* ScannerNew(char *pText) {
	Scanner *s = ObjNew(Scanner, 1);
	s->text = pText;
	s->len = strlen(pText);
	s->i = 0;
	s->line = 1;
    s->pos = 1;
//	ScannerNext(s);
	return s;
}

// �\��G���� Scanner ���� 
// �d�ҡGScannerFree(s);
void ScannerFree(Scanner *s) {
	ObjFree(s);
}

// �\��G�x�s Scanner ���ثe���A
// �����G��R�ɭY�u���ݡv�᭱�X�� token�A�N�����ϥ� ScannerStore() �x�s�A�M��I�s 
//       ScannerNext() ���ݡA����A�� ScannerRestore() ��_�A�H������Ӱ��ݹL�{�C 
// �d�ҡGScannerStore(s);
void ScannerStore(Scanner *s) {
    s->iSave = s->i;
    s->posSave = s->pos;
    s->lineSave = s->line;
    s->tagSave = s->tag;
    strcpy(s->tokenSave, s->token);
}

// �\��G��_ Scanner ���x�s���A 
// �d�ҡGScannerRestore(s);
void ScannerRestore(Scanner *s) {
    s->i = s->iSave;
    s->pos = s->posSave;
    s->line = s->lineSave;
    s->tag = s->tagSave;
    strcpy(s->token, s->tokenSave);
}

// �\��G�ˬd�U�@�ӵ��J�O�_�ŦX tag �аO�C
// �d�ҡGif (ScannerIsNext(s, "+|-|*|/")) ScannerNext(s);
BOOL ScannerIsNext(Scanner *s, char *pTags) { // �ˬd�U�@�ӵ��J�����A
	char tTags[MAX_LEN+1];
	sprintf(tTags, "|%s|", pTags);
	if (strPartOf(s->tag, tTags))
		return TRUE;
    else
		return FALSE;
}

// �\��G���o�ثe�r��
// �d�ҡGwhile (strMember(ch(s), DIGIT)) cnext(s);
char ch(Scanner *s) {
    return s->text[s->i];
}

// �\��G�e�i��U�@�Ӧr�� 
// �d�ҡGwhile (strMember(ch(s), DIGIT)) cnext(s);
void cnext(Scanner *s) {
    s->i++;s->pos++;
}

#define OP "+-*/%<=>!&|"        // �B��Ÿ��r���� (�ΨӨ��o +,-,*,/, ++, ...) 

// �\��GScanner ���J���R���q�����յ{���C
// �d�ҡGScannerTest("test.c1");
void ScannerNext(Scanner *s) { // ���y�U�@�ӵ��J
	while (strMember(ch(s), SPACE)) { // �����ť�
	    if (ch(s)=='\n') {
            s->line++;
            s->pos = 1;
        }
		cnext(s);
    }
	if (s->i >= s->len) { // �p�G�W�L�{������ 
	    s->tag = kEND; // �Ǧ^ tag = kEND 
	    s->token[0] = '\0'; // �Ǧ^ token = �Ŧr�� 
	    return;
    }
	char c = ch(s); // ���o�U�@�Ӧr��
	int begin = s->i; // �O����J�}�l�I
	if (c == '\"') { // �p�G�O " �N��r��}�Y
	    // �r��`�� : string = ".."
		cnext(s); // ���L "
		while (ch(s) != '\"') cnext(s); // �@��Ū��U�@�� " �Ÿ�����C
		cnext(s); // ���L "
	} else if (strMember(c, OP)) { // �p�G�OOP(+-*/<=>!���Ÿ�)
  	    // �B��Ÿ� : OP = ++, --, <=, >=, ...
		while (strMember(ch(s), OP)) cnext(s); // �@��Ū�줣�OOP����
	} else if (strMember(c, DIGIT)) { // �p�G�O�Ʀr
   	    // �Ʀr�`�� : number = 312, 77568, ... 
		while (strMember(ch(s), DIGIT)) cnext(s); // �@��Ū�줣�O�Ʀr����
		// �B�I�`�� : float = 3.14, ... 
		if (ch(s) == '.') cnext(s); // ���o�p���I 
		while (strMember(ch(s), DIGIT)) cnext(s); // ���o�p�Ƴ��� 
	} else if (strMember(c, ALPHA)) { // �p�G�O�^��r��
	    // �򥻵��J : token = int, sum, i, for, if, x1y2z, ....  
		while (strMember(ch(s), ALPHA) || strMember(ch(s), DIGIT)) 
            cnext(s); // �@��Ū�줣�O�^��r�� (�μƦr)����
	} else // ��L�Ÿ��A����Ū����@�r�� 
		cnext(s); // �Ǧ^��@�r��
		
    // �r�걽�y���F�A�]�w token ��(begin�KtextIdx) �������l�r��
	strSubstr(s->token, s->text, begin, (s->i) - begin); 
	
	// �]�w token ���аO tag
	s->tag = tokenToTag(s->token);
}

// �\��GScanner ���J���R���q�����յ{���C
// �d�ҡGScannerTest("test.c1");
char *tokenToTag(char *token) { // �P�_�è��o token�����A
    if (token[0] == '\"') // �p�G�H�Ÿ� " �}�Y�A�h
		return CSTR; // ���A�� STRING
	else if (strMember(token[0], DIGIT)) {// �p�G�O�Ʀr�}�Y�A�h
		if (strMember('.', token))
			return CFLOAT;
		else
			return CINT;
	} else { // �_�h (���O +,-,*,/,>,<,�K.)
        char *tag = NULL;
        // �Y�O keyword (�]�t ����r if, for �P +, ->, {, ++ ���X�k�Ÿ�
        // �h�Ǧ^�d���G (�r�����)�C
  int i;
	    for (i=0; gTokenList[i] != kEND; i++) {
            if (strEqual(token, gTokenList[i])) // ���� token�A�Ǧ^�r����СC
               return gTokenList[i]; 
        }
	    if (strMember(token[0], ALPHA)) // �p�G�O�^��r���}�Y
		   return ID; // �h���A�� ID
        else
            ERROR();
  }
}
