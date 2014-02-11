// 本軟體採用公共領域授權 (Public Domain License)，您可以任意使用本軟體及其原始碼。 
// 但作者對於任何因本軟體所產生的損害（包含他人修改後所造成的損害），不負任何責任。
// 作者：陳鍾誠 2011.7.4

#include <string.h>
#include "Scanner.h"

// 宣告關鍵字的字串變數，像是 char kIF[]="if"; ...char EXP[]="EXP";...
#define DEF(var, str) char var[]=str;
#include "Token.h"
#undef DEF

// 宣告關鍵字陣列， gTagList={...,"if", ...,"EXP", ... };
char *gTokenList[] = {
#define DEF(var, str) var,
#include "Token.h"
#undef DEF
};

// 功能：Scanner 詞彙分析階段的測試程式。
// 範例：ScannerTest("test.c1");
void ScannerTest(char *fileName) {
	debug("======================ScannerTest()=========================\n");    
    char *text = fileToStr(fileName); // 讀取整個程式檔，成為一個字串 text 
    Scanner *s = ScannerNew(text); // 建立 Scanner 物件 
    while (TRUE) { // 不斷掃描詞彙，直到檔案結束 
        ScannerNext(s); // 取得下一個詞彙
        debug("token=%-10s tag=%-10s line=%-4d pos=%-3d\n", 
              s->token, s->tag, s->line, s->pos);
        if (s->tag == kEND) // 已經到程式結尾
            break; // 結束掃描 
    }
    ScannerFree(s); // 釋放 Scanner 物件 
    strFree(text); // 釋放字串 text
    memCheck(); // 檢查記憶體 
}

// 功能：建立新的詞彙分析 Scanner 物件 
// 範例：Scanner *s = ScannerNew(text);
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

// 功能：釋放 Scanner 物件 
// 範例：ScannerFree(s);
void ScannerFree(Scanner *s) {
	ObjFree(s);
}

// 功能：儲存 Scanner 的目前狀態
// 說明：剖析時若「偷看」後面幾個 token，就必須使用 ScannerStore() 儲存，然後呼叫 
//       ScannerNext() 偷看，之後再用 ScannerRestore() 恢復，以完成整個偷看過程。 
// 範例：ScannerStore(s);
void ScannerStore(Scanner *s) {
    s->iSave = s->i;
    s->posSave = s->pos;
    s->lineSave = s->line;
    s->tagSave = s->tag;
    strcpy(s->tokenSave, s->token);
}

// 功能：恢復 Scanner 的儲存狀態 
// 範例：ScannerRestore(s);
void ScannerRestore(Scanner *s) {
    s->i = s->iSave;
    s->pos = s->posSave;
    s->line = s->lineSave;
    s->tag = s->tagSave;
    strcpy(s->token, s->tokenSave);
}

// 功能：檢查下一個詞彙是否符合 tag 標記。
// 範例：if (ScannerIsNext(s, "+|-|*|/")) ScannerNext(s);
BOOL ScannerIsNext(Scanner *s, char *pTags) { // 檢查下一個詞彙的型態
	char tTags[MAX_LEN+1];
	sprintf(tTags, "|%s|", pTags);
	if (strPartOf(s->tag, tTags))
		return TRUE;
    else
		return FALSE;
}

// 功能：取得目前字元
// 範例：while (strMember(ch(s), DIGIT)) cnext(s);
char ch(Scanner *s) {
    return s->text[s->i];
}

// 功能：前進到下一個字元 
// 範例：while (strMember(ch(s), DIGIT)) cnext(s);
void cnext(Scanner *s) {
    s->i++;s->pos++;
}

#define OP "+-*/%<=>!&|"        // 運算符號字元集 (用來取得 +,-,*,/, ++, ...) 

// 功能：Scanner 詞彙分析階段的測試程式。
// 範例：ScannerTest("test.c1");
void ScannerNext(Scanner *s) { // 掃描下一個詞彙
	while (strMember(ch(s), SPACE)) { // 忽略空白
	    if (ch(s)=='\n') {
            s->line++;
            s->pos = 1;
        }
		cnext(s);
    }
	if (s->i >= s->len) { // 如果超過程式結尾 
	    s->tag = kEND; // 傳回 tag = kEND 
	    s->token[0] = '\0'; // 傳回 token = 空字串 
	    return;
    }
	char c = ch(s); // 取得下一個字元
	int begin = s->i; // 記住詞彙開始點
	if (c == '\"') { // 如果是 " 代表字串開頭
	    // 字串常數 : string = ".."
		cnext(s); // 跳過 "
		while (ch(s) != '\"') cnext(s); // 一直讀到下一個 " 符號為止。
		cnext(s); // 跳過 "
	} else if (strMember(c, OP)) { // 如果是OP(+-*/<=>!等符號)
  	    // 運算符號 : OP = ++, --, <=, >=, ...
		while (strMember(ch(s), OP)) cnext(s); // 一直讀到不是OP為止
	} else if (strMember(c, DIGIT)) { // 如果是數字
   	    // 數字常數 : number = 312, 77568, ... 
		while (strMember(ch(s), DIGIT)) cnext(s); // 一直讀到不是數字為止
		// 浮點常數 : float = 3.14, ... 
		if (ch(s) == '.') cnext(s); // 取得小數點 
		while (strMember(ch(s), DIGIT)) cnext(s); // 取得小數部分 
	} else if (strMember(c, ALPHA)) { // 如果是英文字母
	    // 基本詞彙 : token = int, sum, i, for, if, x1y2z, ....  
		while (strMember(ch(s), ALPHA) || strMember(ch(s), DIGIT)) 
            cnext(s); // 一直讀到不是英文字母 (或數字)為止
	} else // 其他符號，都解讀為單一字元 
		cnext(s); // 傳回單一字元
		
    // 字串掃描完了，設定 token 為(begin…textIdx) 之間的子字串
	strSubstr(s->token, s->text, begin, (s->i) - begin); 
	
	// 設定 token 的標記 tag
	s->tag = tokenToTag(s->token);
}

// 功能：Scanner 詞彙分析階段的測試程式。
// 範例：ScannerTest("test.c1");
char *tokenToTag(char *token) { // 判斷並取得 token的型態
    if (token[0] == '\"') // 如果以符號 " 開頭，則
		return CSTR; // 型態為 STRING
	else if (strMember(token[0], DIGIT)) {// 如果是數字開頭，則
		if (strMember('.', token))
			return CFLOAT;
		else
			return CINT;
	} else { // 否則 (像是 +,-,*,/,>,<,….)
        char *tag = NULL;
        // 若是 keyword (包含 關鍵字 if, for 與 +, ->, {, ++ 等合法符號
        // 則傳回查表結果 (字串指標)。
  int i;
	    for (i=0; gTokenList[i] != kEND; i++) {
            if (strEqual(token, gTokenList[i])) // 找到該 token，傳回字串指標。
               return gTokenList[i]; 
        }
	    if (strMember(token[0], ALPHA)) // 如果是英文字母開頭
		   return ID; // 則型態為 ID
        else
            ERROR();
  }
}
