#include "Generator.h"

// 程式產生器的主要函數。
void generate(Tree *tree, SymTable *symTable, char *asmFile) {      // 將剖析樹 tree 轉為組合語言檔 asmFile
  char nullVar[100]="";                                                             
  Generator *g = GenNew();                      // 開啟組合語言檔以便輸出              
	g->symTable = symTable;
  g->asmFile = fopen(asmFile, "w");                                                 
	printf("=====PCODE=====\n");                  
	GenPcode(g, "", ".code", "", "", "");
	GenCode(g, tree, nullVar);                    // 產生程式碼 
	GenData(g);                                   // 關閉組合語言檔
  fclose(g->asmFile);                           // 釋放記憶體                          
  GenFree(g);                                   // 讀入組合語言檔並印出                
  char *asmText = fileToStr(asmFile);                                              
  printf("=====AsmFile:%s======\n", asmFile);                                       
  printf("%s\n", asmText);                      // 釋放記憶體                          
	memFree(asmText);
}

Generator *GenNew() {
  Generator *g = ObjNew(Generator, 1);
	g->scope = SCOPE_GLOBAL;
	g->pCodeStyle = CStyle;
  return g;
}

void GenFree(Generator *g) {
  ObjFree(g);
}

void GenDecl(Generator *g, Tree *node, SCOPE scope) {
	// DECL = TYPE VAR_LIST
	Tree *type = node->childs->item[0],                                           // 取得子節點
		*varList = node->childs->item[1];
	int i;
	// VAR_LIST = VAR (, VAR)*
	for (i=0; i<varList->childs->count; i+=2) {
		Tree *var = varList->childs->item[i];
		// VAR = ID (= EXP)?
		Tree *id = var->childs->item[0];
/*		Symbol *sym;
		switch (scope) {
		case GLOBAL: 
			sym = SymNew(id->token, type->token); // 將變數加入符號表中。
			HashTablePut(g->symTable, id->token, sym);
//			GenPcode(g, "", "global", type->token, "", id->token);
			break;
		case LOCAL:
			GenPcode(g, "", "local", type->token, "", id->token);
			break;
		case PARAM: 
			GenPcode(g, "", "param", type->token, "", id->token);
			break;
		case STRUCT: 
			GenPcode(g, "", "member", type->token, "", id->token);
		case CODE: */
			if (var->childs->count > 1) {
				Tree *exp = var->childs->item[2];
				char expVar[100];
				GenCode(g, exp, expVar);             
                ERROR();                                   //  遞迴產生 EXP
//				GenPcode(g, "", "=", expVar, "", id->token->str);                            //  中間碼：例如 = 0 sum
		}
/*			break;
		} */
	}
}

void GenCond(Generator *g, Tree *exp, char *label0) {
    char expVar[100];
	GenCode(g, exp, expVar);                                                     // 遞迴產生 COND
	GenPcode(g, "", "J==", expVar, "0", label0);							   // 中間碼：例如 CMP T1, 0, T2
}

void GenCall(Generator *g, Tree *path, Tree *expList, char *rzVar) {
	char pathVar[100];
	GenCode(g, path, pathVar);
	int i;
	for (i=0; i<expList->childs->count; i+=2) {
		Tree *exp = expList->childs->item[i];
		char expVar[100];
		GenCode(g, exp, expVar);
		GenPcode(g, "", "arg", expVar, "", "");
	}
	GenTempVar(g, rzVar);
	GenPcode(g, "", "call", pathVar, "", rzVar);
}

void GenCode(Generator *g, Tree *node, char *rzVar) {                          // 遞迴產生節點 node 的程式碼
/*	int i;
	char nullVar[100] = "", tempVar[100] = "";

  strcpy(rzVar, "");                                                                                               
	if (node == NULL) return;                                                // 遞迴終止條件。
                                                                                                                   
	//  debug("GenCode:NODE:type=%s value=%s\n", node->tag, node->token);
	Tree *child0 = NULL;
	if (node->childs != NULL) child0 = node->childs->item[0]; // 取得第0個子節點。

	if (strEqual(node->tag, "DECL")) {                                         // 處理 DECL
	    // DECL = TYPE VAR_LIST
		GenDecl(g, node, g->scope);
		if (g->scope != STRUCT)
		GenDecl(g, node, CODE);
	} else if (strEqual(node->tag, "TYPE")) {                                         // 處理 DECL
        // TYPE = (int | byte | char | float | STRUCT_TYPE) **  (** 代表 * 出現 0 次以上)
        ERROR();
	} else if (strEqual(node->tag, "METHOD")) {                                  // 處理 METHOD
		// METHOD = def ID ( DECL_LIST ) BLOCK
		Tree *id = node->childs->item[1],                                           // 取得子節點
			*declList  = node->childs->item[3],
			*block = node->childs->item[5];
		char methodLabel[100];
		sprintf(methodLabel, id->token);
		GenPcode(g, methodLabel, "method", "", "", "");                                          // 函數進入點，例如 _sum:
		g->scope = PARAM;
		GenCode(g, declList, nullVar);
		g->scope = LOCAL;
		GenCode(g, block, nullVar);
		GenPcode(g, "", "_method", "", "", "");                                          // 函數進入點，例如 _sum:
		g->scope = GLOBAL;
	} else if (strEqual(node->tag, "FOR")) {                                     // 處理 FOR 節點
		// FOR = for ( STMT ; EXP ; STMT) BASE
    char forBeginLabel[100], forEndLabel[100], condOp[100];                     
    Tree *stmt1 = node->childs->item[2],                                        // 取得子節點                         
			*exp  = node->childs->item[4],
         *stmt2 = node->childs->item[6],                                                                           
         *base  = node->childs->item[8];
    GenCode(g, stmt1, nullVar);                                                 // 遞迴產生 STMT                             
    int tempForCount = g->forCount++;                                           // 設定FOR迴圈的                 
    sprintf(forBeginLabel, "FOR%d", tempForCount);                              //   進入標記                                
    sprintf(forEndLabel, "_FOR%d", tempForCount);                               //   離開標記            
    GenPcode(g, forBeginLabel, "", "", "", "");                                 // 中間碼：例如 FOR1:    
		GenCond(g, exp, forEndLabel);
    GenCode(g, base, nullVar);                                                 // 遞迴產生 BLOCK
    GenCode(g, stmt2, nullVar);                                                 // 遞迴產生 STMT        
		GenPcode(g, "", "goto", "", "", forBeginLabel);                                // 中間碼：例如J FOR1
    GenPcode(g, forEndLabel, "", "", "", "");                                   // 中間碼：例如 _FOR1    
	} else if (strEqual(node->tag, "IF")) {                                     // 處理 FOR 節點
		// if ( EXP ) BASE ( else BASE )?
		char elseLabel[100];
		Tree *exp  = node->childs->item[2],                                        // 取得子節點
         *base1 = node->childs->item[4],
         *base2 = node->childs->item[7];
		int tempIfCount = g->ifCount++;
		sprintf(elseLabel, "ELSE%d", tempIfCount);
		GenCond(g, exp, elseLabel);
    GenCode(g, base1, nullVar);                                                 // 遞迴產生 BASE1
    GenPcode(g, elseLabel, "", "", "", "");                                     // 產生 ELSE 標記，例如 ELSE1:
    GenCode(g, base2, nullVar);                                                 // 遞迴產生 STMT
	} else if (strEqual(node->tag, "WHILE")) {                                     // 處理 FOR 節點
		// while (EXP) BASE
		char wendLabel[100];
		Tree *exp  = node->childs->item[2],                                        // 取得子節點
         *base  = node->childs->item[4];
    int tempWhileCount = g->whileCount++;                                       // 設定 WHILE 迴圈的
    sprintf(wendLabel, "_WHILE%d", tempWhileCount);                         //   離開標記
		GenCond(g, exp, wendLabel);
    GenCode(g, base, nullVar);                                                  // 遞迴產生 BASE
    GenPcode(g, wendLabel, "", "", "", "");                                     // 產生 _WHILE 標記，例如 _WHILE1:
	} else if (strEqual(node->tag, "STRUCT")) {                                     // 處理 FOR 節點
		// ** STRUCT = struct ID { (DECL;)* }
		Tree *id = node->childs->item[1];
		GenPcode(g, id->token, "STRUCT", "", "", "");
		g->scope = STRUCT;
		for (i=3; i<node->childs->count-1; i+=2) {
            Tree *decl = node->childs->item[i];
            GenCode(g, decl, nullVar);
        }
		GenPcode(g, "", "_STRUCT", "", "", "");
	} else if (strEqual(node->tag, "PATH")) {                                     // 處理 FOR 節點
		// ** PATH = ID ((.|->|[ EXP_LIST ]) ID)*
		ERROR();
	} else if (strEqual(node->tag, "STMT")) {                                    // 處理 STMT 節點
	    // ** STMT = return EXP | DECL | PATH = EXP | PATH (EXP_LIST) | PATH OP1
    // STMT = return EXP | DECL | ID (EXP_LIST) | ID = EXP | ID OP1
		if (strEqual(child0->tag, "return")) { // return EXP                           // 處理 return 指令
      Tree *exp = node->childs->item[1];
	  GenCode(g, exp, rzVar);
			GenPcode(g, "", "return", "", "", rzVar);                                    // 中間碼： 例如 RET sum
		} else if (strEqual(child0->tag, "DECL")) { // DECL				          // 產生 DECL 的程式碼
			Tree *decl = child0;
			GenCode(g, decl, nullVar);
		} else if (strEqual(child0->tag, "PATH")) { // ID : 可能是 PATH = EXP | PATH (EXP_LIST) | PATH OP1
			Tree *path = node->childs->item[0]; // 取得子節點
			char pathVar[100];
			GenCode(g, path, pathVar);
	  if (node->childs->count <=1)
	    ERROR();
      Tree *op = node->childs->item[1];                                                                   
			if (strEqual(op->tag, "(")) {                                            // 處理 id(EXP_LIST)
				Tree *expList = node->childs->item[2];
				GenCall(g, path, expList, rzVar);
			} if (strEqual(op->tag, "=")) {                                          // 處理 id= EXP
				// STMT 是 ID = EXP
        Tree *exp = node->childs->item[2];                                                                
        char expVar[100];                                                          
        GenCode(g, exp, expVar);                                                //  遞迴產生 EXP            
				GenPcode(g, "", "=", expVar, "", pathVar);                            //  中間碼：例如 = 0 sum
        strcpy(rzVar, expVar);                                                  //  傳回 EXP 的變數，例如 T0  
			} else if (strPartOf(op->tag, SET_OP1)) { // STMT 是 ID OP1                             					// 處理 id++ 或 id--
            	GenPcode(g, "", op->tag, pathVar, "", pathVar); //  中間碼：例如 ++ i   i
      } else
		ERROR();
    }                                                                           
	} else if (strEqual(node->tag, "EXP")) {                                  // 處理 EXP
		// EXP = TERM (OP2 TERM)?
    char var1[100], var2[100];
		Tree *term1 = node->childs->item[0];                                           //   取得子節點
		GenCode(g, term1, var1);
		if (node->childs->count > 1) {
			Tree *op = node->childs->item[1];
			Tree *term2 = node->childs->item[2];
			GenCode(g, term2, var2);
			GenTempVar(g, rzVar);
			GenPcode(g, "", op->token, var1, var2, rzVar);
		} else 
	    strcpy(rzVar, var1);
	} else if (strEqual(node->tag, "TERM")) { // 處理 TERM
		// TERM = TERM = ( EXP (OP2 EXP)? ) | ITEM
		if (node->childs->count == 1) { // ITEM
			Tree *item = node->childs->item[0];
			GenCode(g, child0, rzVar);
		} else if (node->childs->count >= 3) { // ( EXP )
			char var1[100], var2[100];
			Tree *exp1 = node->childs->item[1];
	    Tree *op = node->childs->item[2];
			GenCode(g, exp1, var1);
			if (node->childs->count == 5) { // EXP OP2 EXP
  	    Tree *exp2 = node->childs->item[3];
	    GenCode(g, exp2, var2);
	    GenTempVar(g, rzVar);
				GenPcode(g, "", op->token, var1, var2, rzVar);
			} else
				strcpy(rzVar, var1);
	  }
		else ERROR();
	} else if (strEqual(node->tag, "ITEM")) { // 處理 ITEM
		// ** ITEM = INT | FLOAT | STRING | PATH OP1? | PATH (EXP_LIST?)
		Tree *child0 = node->childs->item[0];
		if (strPartOf(child0->tag, "int|float|string"))
			GenCode(g, child0, rzVar);
		else {
			// ERROR();
			Tree *path = child0;
			char pathVar[100];
			GenCode(g, path, pathVar);
			Tree *op = node->childs->item[1];
			if (node->childs->count == 1)
			    strcpy(rzVar, pathVar);
			else if (node->childs->count == 2) {
            	GenPcode(g, "", op->tag, pathVar, "", pathVar); //  中間碼：例如 ++ i   i
            } else if (node->childs->count == 4) {
				Tree *expList = node->childs->item[2];
	   		    GenCall(g, path, expList, rzVar);
				}
			}
	} else if (strPartOf(node->tag, "integer|float")) {                            // 處理 number, id 節點
		// 遇到變數或常數，傳回其 value 名稱。
		strcpy(rzVar, node->token);                                                 // 直接傳回 id 或 number
	} else if (strPartOf(node->tag, "string")) {                            // 處理 number, id 節點
    // 遇到變數或常數，傳回其 value 名稱。                                                         
		GenTempVar(g, rzVar);
  } else if (node->childs != NULL) {                                            // 其他情況           
    // 其他狀況，若有子代則遞回處理                                                                
    int i;                                                                                         
    for (i=0; i<node->childs->count; i++)                                       // 遞迴處理所有子節點
      GenCode(g, node->childs->item[i], nullVar);
  }
	//  debug("_GenCode:NODE:type=%s value=%s\n", node->tag, node->token); */
}

void GenData(Generator* g) {
    Array* symArray = HashTableToArray(g->symTable);
    int i=0;
    GenPcode(g, "", ".data", "", "", "");
    for (i=0; i<symArray->count; i++) {
        Symbol *symbol = symArray->item[i];
        GenPcode(g, "", "global", "", "", symbol->name);
    }
}

// 輸出pcode後再轉為組合語言
void GenPcode(Generator* g, char* label, char* op, char* p1, char* p2, char* pTo) { 
	pcode(stdout, label, op, p1, p2, pTo, g->pCodeStyle); // 印出 pcode
	fprintf(g->asmFile, "; ");
	pcode(g->asmFile, label, op, p1, p2, pTo, g->pCodeStyle); // 印出 pcode
//	pcodeToAsm(g->asmFile, label, op, p1, p2, pTo, g->pCodeStyle); // 將 pcode 轉為組合語言
}

void fprintLabel(FILE *file, char *label) {
    char tlabel[100];
        sprintf(tlabel, "%s:", label);
    if (strlen(label) > 0)
        fprintf(file, "%-8s", tlabel);
    else
        fprintf(file, "%-8s", "");
}                                                                                                        
                                                                                                                            
void pcode(FILE *file, char* label, char* op, char* p1, char* p2, char* pTo, int style) {
	fprintLabel(file, label);
	if (style == CStyle)
  	  pcodeCStyle(file, op, p1, p2, pTo);
  	else
  	  pcodeAsmStyle(file, op, p1, p2, pTo);
	fprintf(file, "\n");
}                                                                                                  
                                                                                                                            
void pcodeAsmStyle(FILE *file, char* op, char* p1, char* p2, char* pTo) {
    fprintf(file, "%-6s %-6s %-6s %-6s", op, p1, p2, pTo);
}

void pcodeCStyle(FILE *file, char* op, char* p1, char* p2, char* pTo) {
	if (strPartOf(op, "|global|local|arg|param|return|goto|method|_method|"))
  	    fprintf(file, "%s %s %s %s", op, p1, p2, pTo);
	else if (strPartOf(op, "|.data|.code|"))
  	    fprintf(file, "%s", op);
	else if (strPartOf(op, "|call|"))
  	    fprintf(file, "%s = %s(...)", pTo, p1);
  	else if (strPartOf(op, "|J==|J!=|J>|J<|J>=|J<=|"))
        fprintf(file, "if %s %s %s goto %s", p1, &op[1], p2, pTo);
	else if (strEqual(op, "="))
  	    fprintf(file, "%s = %s", pTo, p1);
	else if (strPartOf(op, "||.data|.code|"))
  	    fprintf(file, "%s %s %s %s", pTo, p1, op, p2);
	else
  	    fprintf(file, "%s = %s %s %s", pTo, p1, op, p2);
}
                                                                                                        
void GenTempVar(Generator *g, char *tempVar) {                                                          
  sprintf(tempVar, "T%d", g->varCount++);
}

int typeSize(char *type) {
    if (strPartOf(type, "|int|float|"))
        return 4;
    if (strPartOf(type, "|byte|"))
        return 1;
    ERROR();
    return 0;
}
