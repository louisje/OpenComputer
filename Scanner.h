// 本軟體採用公共領域授權 (Public Domain License)，您可以任意使用本軟體及其原始碼。 
// 但作者對於任何因本軟體所產生的損害（包含他人修改後所造成的損害），不負任何責任。
// 作者：陳鍾誠 2011.7.4

#ifndef SCANNER_H
#define SCANNER_H

#include "Lib.h"

typedef struct {        // 掃描器的物件結構              
  char *text;           //   輸入的程式 (text) 
	int len;               //   程式的總長度
	// 注意：以下的 xSave 都是在 ScannerStore() 與 ScannerRestore() 時使用的備份。 
	int i, iSave;          //   目前詞彙的位置 
	int line, lineSave;    //   目前詞彙的行號 
	int pos, posSave;      //   目前詞彙的起始點 
	char *tag, *tagSave;   //   詞彙的標記 
	char token[100], tokenSave[100]; // 目前的詞彙 
} Scanner;

void ScannerTest(char *fileName);   // Scanner 詞彙分析階段的測試程式。 
Scanner* ScannerNew(char *pText);   // 建立新的詞彙分析 Scanner 物件 
void ScannerFree(Scanner *s);       // 釋放 Scanner 物件 
void ScannerStore(Scanner *s);      // 儲存 Scanner 的目前狀態 
void ScannerRestore(Scanner *s);    // 恢復 Scanner 的儲存狀態
BOOL ScannerIsNext(Scanner *s, char *pTags); // 檢查下一個詞彙是否符合 tag 標記。 
void ScannerNext(Scanner *s);       // 取得下一個詞彙 (token) 
char ch(Scanner *s);                // 取得目前字元 
void cnext(Scanner *s);             // 前進到下一個字元 
char *tokenToTag(char *token);      // 對詞彙 (token) 進行標記 (tag)

// 宣告 Token 變數，包含關鍵字 if, for, 運算子 ++, / 與 非終端項目 IF, FOR... 
#define DEF(var, str) extern char var[];
#include "Token.h"
#undef DEF

#endif
