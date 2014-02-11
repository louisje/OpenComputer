// ���n��ĥΤ��@�����v (Public Domain License)�A�z�i�H���N�ϥΥ��n��Ψ��l�X�C 
// ���@�̹�����]���n��Ҳ��ͪ��l�`�]�]�t�L�H�ק��ҳy�����l�`�^�A���t����d���C

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

// ================== �O����޲z��� ============================ 
#ifdef DEBUG
int memNewCount = 0; // �`�@���t�F�X���O����C 
int memFreeCount=0; // �`�@����F�X���O����C 
#endif

// �\��G�O����t�m��ơA���t�Ŷ��òM���� 0�A�ðO�����t���� 
void* memNew(int size) {
	void *ptr=malloc(size); // ���t�O���� 
	ASSERT(ptr != NULL);    // �p�G���t���ѫh���~ 
	memset(ptr, 0, size);   // �N�O���餺�e�M�Ŭ��s�C 
#ifdef DEBUG
	memNewCount++;          // �N���t���ƥ[�@ 
#endif
	return ptr;             // �Ǧ^���t���O������� 
}

// �\��G�O���������ơA�ðO�����񦸼�
void memFree(void *ptr) {
	ASSERT(ptr != NULL);    // ���i���񥼤��t���O���� 
    free(ptr);              // ����O���� 
#ifdef DEBUG
	memFreeCount++;         // �N���񦸼ƥ[�@ 
#endif
}

// �\��G�O�����ˬd��ơA�ݬݤ��t���ƻP���񦸼ƬO�_�ۦP
// => �p�G�ۦP�h�j�P�W���ӨS�����~ 
// => ��M���ܤp�����|�٬O�����A�u�O���t�P���񦸼ƫꥩ�ۦP�Ӥw
void memCheck() { 
#ifdef DEBUG
	debug("Memory:newCount=%d freeCount=%d\n", memNewCount, memFreeCount); 
	ASSERT(memNewCount == memFreeCount); // �p�G���t�P���񦸼Ƥ��P�A���N���� 
#endif
}

// ================== �ɮ׺޲z��� ============================ 
// �\��G�N�ɮ�Ū�J�����@�� BYTE �϶� 
BYTE* fileToBytes(char *fileName, int *sizePtr) {
	FILE *file = fopen(fileName, "rb");    // �}���ɮ� 
	fseek(file, 0 , SEEK_END);             // �����ɧ�
	long size = ftell(file);               // �ɧ���m = �ɮפj�p 
	rewind(file);                          // �^���ɮ׶}�Y 
	BYTE *buffer = (BYTE*) memNew(size+1); // ���t�϶� 
	fread (buffer,size,1,file);            // �N�ɮ�Ū��Ӱ϶��� 
	fclose(file);                          // �����ɮ� 
	*sizePtr = size;                       // �Ǧ^�ɮפj�p 
	return buffer;                         // �Ǧ^�Ӱ϶� 
}

// �\��G�N�ɮ�Ū�J�����@�Ӧr�� 
char* fileToStr(char *fileName) {
	int size;                                      // �ɮפj�p 
	BYTE *buffer = fileToBytes(fileName, &size);   // �N�ɮ�Ū�J�� BYTE �϶��� 
	buffer[size] = '\0';                           // �]�w�r�굲���r�� 
	return (char*) buffer;                         // �Ǧ^�Ӧr�� 
}

// ================== �r��ާ@��� ========================
// �\��G�إ߷s�r�� (�ƻs�@��) 
// �d�ҡGstrNew("abc") �|�Ǧ^ "abc"
char *strNew(char *str) {
	char *rzStr = memNew(strlen(str)+1);
  strcpy(rzStr, str);
  return rzStr;
}

// �\��G�إߤl�r��
// �d�ҡGstrSub("0123456789", 3, 2) �|�Ǧ^ "34"
char *strSub(char *str, int i, int len) {
	char *rzStr = memNew(len+1);
  strSubstr(rzStr, str, i, len);
  return rzStr;
}

// �\��G�ˬd�r��}�Y�O�_�� head 
// �d�ҡGstrHead("abcdefg", "abc") �|�Ǧ^ TRUE
BOOL strHead(char *str, char *head) { 
  return (strncmp(str, head, strlen(head))==0);
}

// �\��G�ˬd�r�굲���O�_�� tail
// �d�ҡGstrTail("abcdefg", "efg") �|�Ǧ^ TRUE
BOOL strTail(char *str, char *tail) {
  int strLen = strlen(str), tailLen = strlen(tail);
  if (strLen < tailLen) return FALSE;
  return (strcmp(str+strLen-tailLen, tail)==0);
}

// �\��G���Φr�ꬰ�ⳡ�� 
// �d�ҡGstrCut("abcd/efg", "/", head, tail) �|�]�w head = abcd, tail=efg 
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

// �\��G�p��r�ꤤ���X�� charSet �����r�� 
// �d�ҡGstrCountChar("aabbccdd", "bc") �|�Ǧ^ 4 
int strCountChar(char *str, char *charSet) {
  int i, count=0;
  for (i=0; i<strlen(str); i++)
    if (strMember(str[i], charSet))
      count++;
  return count;
}

// �\��G�N�l�r��ƻs�� substr ���C 
// �d�ҡGstrSubstr(substr, "0123456789", 3, 4) �|�N "3456" ��J substr
char* strSubstr(char *substr, char *str, int i, int len) {
    ASSERT(len >= 0);
  strncpy(substr, &str[i], len);
  substr[len]='\0';
	return substr;
}
 
// �\��G�ˬd token �O�_�����X�r�� set �������� 
// �d�ҡGstrPartOf("int", "byte|int|float") �|�Ǧ^ TRUE 
BOOL strPartOf(char *token, char *set) {
  ASSERT(token != NULL && set != NULL);
	ASSERT(strlen(token) < MAX_LEN);
	char ttoken[MAX_LEN+2];
  sprintf(ttoken, "|%s|", token);
  return (strstr(set, ttoken)!=NULL);
}

// �\��G�N�r���Y�����Y�r���M���� 
// �d�ҡGstrTrim(trimStr, "***abc+++", "*+") �|�N trimStr �]�� "abc" 
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

// �\��G�r����N�A�N str �� fromSet ���X�����r���אּ to
// �d�ҡGstrReplace("abcabc", "bc", '*') �|�N "abcabc" �אּ "a**a**"
char* strReplace(char *str, char *fromSet, char to) {
  int i;
	for (i=0; i<strlen(str); i++) {
		if (strMember(str[i], fromSet))
      str[i] = to;
    }
	return str;
}

// �\��G�N�r�� ch ���� n ���A�H�r��Φ��Ǧ^ 
// �d�ҡGstrFill(str, 'a', 5) �|�N str �]�w�� "aaaaa" 
char* strFill(char *str, char ch, int n) {
	memset(str, ch, n);
	str[n] = '\0';
	return str;
}

// �\��G�N�r���ର�j�g 
// �d�ҡGstrToUpper("aBcDe") �|�Ǧ^ "ABCDE"
char* strToUpper(char *str) {
  int i;
  for (i = 0; i<strlen(str); i++)
    str[i] = toupper(str[i]);
	return str;
}

// �\��G�N�r���ର�p�g 
// �d�ҡGstrToLower("aBcDe") �|�Ǧ^ "abcde"
char* strToLower(char *str) {
	int i;
	for (i = 0; i<strlen(str); i++)
		str[i] = tolower(str[i]);
	return str;
}

// ================== ���~�B�z ===================== 
// �\��G�L�X���~�T���A�Ϊk�P printf �ۦP�C 
void debug(const char *fmt, ...) {
#ifdef DEBUG
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
#endif
}
