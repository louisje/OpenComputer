#include "Asm0.h"

// 將 pcode 轉為組合語言的函數
void PcodeToAsm(FILE *asmFile, char* label, char* op, char* p1, char* p2, char* pTo) {
	if (strlen(label) > 0) // 如果有標記
		rewrite(asmFile, label, "", "", "", ""); //  輸出標記
		
    fprintf(asmFile, "// ");
    pcodeNoLabel(asmFile, op, p1, p2, pTo);
    
	if (strEqual(op, "=")) { // 處理 pTo = p1 
		rewrite(asmFile, "", "LD", "R1", p1, "");  // LDI R1, 0
		rewrite(asmFile, "", "ST", "R1", pTo, ""); // ST R1, sum
	} else if (strEqual(op, "CMP")) { // CMP p1, p2, pTo
		rewrite(asmFile, "", "LD", "R1", p1, "");       // LD R1, p1
		rewrite(asmFile, "", "LD", "R2", p2, "");       // LD R2, p2
		rewrite(asmFile, "", "CMP", "R1", "R2", "");    // CMP R1, R2 => SW
		rewrite(asmFile, "", "SHL", "R3", "R11", "30"); // R3 = SW >> 30 ; 取得 N,Z 兩個狀態位元 
		rewrite(asmFile, "", "ST",  "R3", pTo, "");     // ST R3, pTo
	} else if (strEqual(op, "goto")) { // 處理 goto pTo
		rewrite(asmFile, "", "JMP", pTo, "", ""); // JMP p1
	} else if (strPartOf(op, "|J==|J!=|J>|J<|J>=|J<=|")) { // if p1 op p2 goto pTo
		rewrite(asmFile, "", "LD", "R1", p1, ""); // R1=p1
		rewrite(asmFile, "", "LD", "R2", p2, ""); // R2=p2
		rewrite(asmFile, "", "CMP", "R1", "R2", ""); // cmp p1, p2
		if (strEqual(op, "J==")) rewrite(asmFile, "", "JEQ", pTo, "", ""); // if (p1==p2) goto pTo
		if (strEqual(op, "J!=")) rewrite(asmFile, "", "JNE", pTo, "", ""); // if (p1!=p2) goto pTo
		if (strEqual(op, "J>")) rewrite(asmFile, "", "JGT", pTo, "", "");  // if (p1>p2)  goto pTo
		if (strEqual(op, "J<")) rewrite(asmFile, "", "JLT", pTo, "", "");  // if (p1<p2)  goto pTo
		if (strEqual(op, "J>=")) rewrite(asmFile, "", "JGE", pTo, "", ""); // if (p1>=p2) goto pTo
		if (strEqual(op, "J<=")) rewrite(asmFile, "", "JLE", pTo, "", ""); // if (p1<=p2) goto pTo
	} else if (strEqual(op, "call")) { // 處理 call p1
		rewrite(asmFile, "", "CALL", p1, "", "");
		rewrite(asmFile, "", "LD", pTo, "R1", "");
	} else if (strEqual(op, "arg")) { // 處理 arg p1
		rewrite(asmFile, "", "LD", "R1", p1, "");  // R1 = p1
		rewrite(asmFile, "", "PUSH", "R1", "", "");// push R1
	} else if (strEqual(op, "return")) { // return p1
		rewrite(asmFile, "", "LD", "R1", pTo, ""); // R1 = sum
		rewrite(asmFile, "", "RET", "", "", "");   // RET
	} else if (strPartOf(op, "|+|-|*|/|%|^|&|") || strEqual(op, "|")) { // 處理 p1 OP2 p2
		rewrite(asmFile, "", "LD", "R1", p1, ""); // R1 = p1
		rewrite(asmFile, "", "LD", "R2", p2, ""); // R2 = p2
		// 根據 op 設定運算指令
		if (strEqual(op, "+")) rewrite(asmFile, "", "ADD","R3", "R1", "R2");
		if (strEqual(op, "-")) rewrite(asmFile, "", "SUB","R3", "R1", "R2");
		if (strEqual(op, "*")) rewrite(asmFile, "", "MUL","R3", "R1", "R2");
		if (strEqual(op, "/")) rewrite(asmFile, "", "DIV","R3", "R1", "R2");
		if (strEqual(op, "&")) rewrite(asmFile, "", "AND","R3", "R1", "R2");
		if (strEqual(op, "|")) rewrite(asmFile, "", "OR", "R3", "R1", "R2");
		if (strEqual(op, "^")) rewrite(asmFile, "", "XOR","R3", "R1", "R2");
		if (strEqual(op, "%")) { // 處理 pTo=p1%p2 時 pTo = p1-(p1/p2)*p2
			rewrite(asmFile, "", "DIV", "R3", "R1", "R2"); // R3 = p1/p2
			rewrite(asmFile, "", "MUL", "R4", "R3", "R2"); // R4 = R3*p2 = (p1/p2)*p2
			rewrite(asmFile, "", "SUB", "R3", "R1", "R4"); // R3 = R1-R4 = p1-(p1/p2)*p2
		}
		rewrite(asmFile, "", "ST", "R3", pTo, ""); // ST R3, pTo
	} else if (strPartOf(op, "|<<|>>|")) { // 處理 pTo=p1 op p2 ; op is <<|>>
		rewrite(asmFile, "", "LD", "R1", p1, ""); // R1 = p1
		if (strEqual(op, ">>")) rewrite(asmFile, "", "SHL","R3", "R1", p2); // R3 = R1 << p2
		if (strEqual(op, "<<")) rewrite(asmFile, "", "SHR","R3", "R1", p2); // R3 = R1 >> p2
		rewrite(asmFile, "", "ST", "R3", pTo, ""); // pTo = R3
	} else if (strPartOf(op, "|==|!=|<=|>=|<|>|")) { // 處理 pTo=p1 op p2 ; op 是比較運算
		rewrite(asmFile, "", "LD", "R1", p1, ""); // R1=p1
		rewrite(asmFile, "", "LD", "R2", p2, ""); // R2=p2
		rewrite(asmFile, "", "LD", "R3", "0", "");// R3=0, 如果沒有執行 1:，則結果值為 0。 
		rewrite(asmFile, "", "CMP","R1", "R2", ""); // CMP p1, p2
		if (strEqual(op, "==")) rewrite(asmFile, "", "JEQ", "1f", "", ""); // if (p1==p2) R3=1
		if (strEqual(op, "!=")) rewrite(asmFile, "", "JNE", "1f", "", ""); // if (p1!=p2) R3=1
		if (strEqual(op, "<"))  rewrite(asmFile, "", "JLT", "1f", "", ""); // if (p1<p2)  R3=1
		if (strEqual(op, ">"))  rewrite(asmFile, "", "JGT", "1f", "", ""); // if (p1>p2)  R3=1
		if (strEqual(op, "<=")) rewrite(asmFile, "", "JLE", "1f", "", ""); // if (p1<=p2) R3=1
		if (strEqual(op, ">=")) rewrite(asmFile, "", "JGE", "1f", "", ""); // if (p1>=p2) R3=1
		rewrite(asmFile, "", "JMP", "2f", "", "");   // goto 2: (R3 = 0)
		rewrite(asmFile, "1:", "LD", "R3", "1", ""); // R3 = 1
		rewrite(asmFile, "2:", "ST", "R3", pTo, ""); // pTo = R3
	} else if (strEqual(op, "&&") || strEqual(op, "||")) { // 處理 pTo=p1 op p2 ; op 是邏輯運算
		rewrite(asmFile, "", "LD", "R1", p1, "");  // R1 = p1
		rewrite(asmFile, "", "LD", "R2", p2, "");  // R2 = p2
		rewrite(asmFile, "", "LD", "R3", "0", ""); // R3 = 0
        rewrite(asmFile, "", "CMP", "R1", "R0", ""); // R1 cmp R2
		if (strEqual(op, "&&")) { // pTo = p1 && p2;
            rewrite(asmFile, "", "JEQ", "2f", "", ""); // if !p1 pTo=0
            rewrite(asmFile, "", "CMP", "R2", "R0", ""); 
            rewrite(asmFile, "", "JEQ", "1f", "", ""); // if (p1&&p2) pTo=1
        }
		if (strEqual(op, "||")) { // pTo = p1 || p2;
            rewrite(asmFile, "", "JNE", "1f", "", ""); // if (p1) pTo=1
            rewrite(asmFile, "", "CMP", "R2", "R0", "");
            rewrite(asmFile, "", "JNE", "1f", "", ""); // if (!p1||p2) pTo=1
        }
		rewrite(asmFile, "1:", "LD", "R3", "1", "");   // R3=1
		rewrite(asmFile, "2:", "ST", "R3", pTo, "");   // pTo=R3
	} else if (strPartOf(op, "|method|")) { // 處理 method, _method
	    fprintf(asmFile, "method_enter()\n");
	} else if (strPartOf(op, "|_method|")) { // 處理 global type var ; 全域變數 
	    fprintf(asmFile, "method_exit()\n");
	} else if (strPartOf(op, "|global|")) { // 處理 global type var ; 全域變數 
	    char varLabel[100];
	    sprintf(varLabel, "%s:", pTo);
	    char *type = p1;
	    char *var = pTo;
	    if (strEqual(p1, "int"))
		  rewrite(asmFile, varLabel, "RESW", "1", "", ""); // 宣告全域變數 
		else
		  ERROR();
	} else if (strPartOf(op, "|param|")) { // 處理 param
	} else if (strPartOf(op, "|local|")) { // 處理 local
    }
}

void rewrite(FILE *asmFile, char* label, char* op, char* p1, char* p2, char* pTo) {      // 輸出組合語言指令
	char *realOp = op;
	if (strEqual(op, "LD"))
		if (isdigit(p2[0]))                                                                    // 如果指令是 LD，而且
			realOp = "LDI";                                                                      //   p2 為常數
	fprintf(asmFile, "%-8s %-4s %-4s %-4s %-4s\n", label, realOp, p1, p2, pTo);              //    改用 LDI 指令
}                                                                                          // 輸出組合語言指令
