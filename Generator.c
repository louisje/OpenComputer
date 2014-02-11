#include "Generator.h"

// 程式產生器的主要函數。
void generate(Tree *tree, char *asmFile) {      // 將剖析樹 tree 轉為組合語言檔 asmFile
  char nullVar[100]="";                                                             
  Generator *g = GenNew();                      // 開啟組合語言檔以便輸出              
  g->asmFile = fopen(asmFile, "w");                                                 
  printf("=====PCODE=====\n");                  // 產生程式碼                          
  GenCode(g, tree, nullVar);                    // 產生資料宣告                        
	//  GenData(g);                                   // 關閉組合語言檔
  fclose(g->asmFile);                           // 釋放記憶體                          
  GenFree(g);                                   // 讀入組合語言檔並印出                
  char *asmText = fileToStr(asmFile);                                              
  printf("=====AsmFile:%s======\n", asmFile);                                       
  printf("%s\n", asmText);                      // 釋放記憶體                          
  freeMemory(asmText);
	ERROR();
}

Generator *GenNew() {
  Generator *g = ObjNew(Generator, 1);
  g->symTable = HashTableNew(127);
	g->scope = GLOBAL;
  return g;
}

void GenFree(Generator *g) {
	HashTable *table = g->symTable;
	int i,j;
	for (i=0; i<table->count; i++) {
		Array *hits = table->item[i];
		for (j=0; j<hits->count; j++) {
			Entry *e = hits->item[j];
			Symbol *s = (Symbol*) e->data;
			SymFree(s);
		}
	}
  HashTableFree(g->symTable);
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
		Symbol *sym;
		switch (scope) {
		case GLOBAL: 
			sym = SymNew(id->value, type->value); // 將變數加入符號表中。
			HashTablePut(g->symTable, id->value, sym);
			GenPcode(g, "", "global", type->value, "", id->value);
			break;
		case LOCAL:
			GenPcode(g, "", "local", type->value, "", id->value);
			break;
		case PARAM: 
			GenPcode(g, "", "param", type->value, "", id->value);
			break;
		case CODE: 
			if (var->childs->count > 1) {
				Tree *exp = var->childs->item[2];
				char expVar[100];
				GenCode(g, exp, expVar);                                                //  遞迴產生 EXP
				GenPcode(g, "", "=", expVar, "", id->value);                            //  中間碼：例如 = 0 sum
			}
			break;
		}
	}
}

void GenCond(Generator *g, Tree *exp, char *label0) {
    char expVar[100];
	GenCode(g, exp, expVar);                                                     // 遞迴產生 COND
	GenPcode(g, "", "J==", expVar, "0", label0);							   // 中間碼：例如 CMP T1, 0, T2
}

void GenCode(Generator *g, Tree *node, char *rzVar) {                          // 遞迴產生節點 node 的程式碼
	int i;
	char nullVar[100] = "", tempVar[100] = "";

  strcpy(rzVar, "");                                                                                               
	if (node == NULL) return;                                                // 遞迴終止條件。
                                                                                                                   
	//  debug("GenCode:NODE:type=%s value=%s\n", node->type, node->value);
	Tree *child0 = NULL;
	if (node->childs != NULL) child0 = node->childs->item[0]; // 取得第0個子節點。

	if (strEqual(node->type, "DECL")) {                                         // 處理 DECL
		GenDecl(g, node, g->scope);
		GenDecl(g, node, CODE);
	} else if (strEqual(node->type, "METHOD")) {                                  // 處理 METHOD
		// METHOD = def ID ( DECL_LIST ) BLOCK
		Tree *id = node->childs->item[1],                                           // 取得子節點
			*declList  = node->childs->item[3],
			*block = node->childs->item[5];
		char methodLabel[100];
		sprintf(methodLabel, id->value);
		GenPcode(g, methodLabel, "method", "", "", "");                                          // 函數進入點，例如 _sum:
		g->scope = PARAM;
		GenCode(g, declList, nullVar);
		g->scope = LOCAL;
		GenCode(g, block, nullVar);
		GenPcode(g, "", "_method", "", "", "");                                          // 函數進入點，例如 _sum:
		g->scope = GLOBAL;
  } else if (strEqual(node->type, "FOR")) {                                     // 處理 FOR 節點
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
  } else if (strEqual(node->type, "IF")) {                                     // 處理 FOR 節點
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
  } else if (strEqual(node->type, "WHILE")) {                                     // 處理 FOR 節點
		// while (EXP) BASE
		char wendLabel[100];
		Tree *exp  = node->childs->item[2],                                        // 取得子節點
         *base  = node->childs->item[4];
    int tempWhileCount = g->whileCount++;                                       // 設定 WHILE 迴圈的
    sprintf(wendLabel, "_WHILE%d", tempWhileCount);                         //   離開標記
		GenCond(g, exp, wendLabel);
    GenCode(g, base, nullVar);                                                  // 遞迴產生 BASE
    GenPcode(g, wendLabel, "", "", "", "");                                     // 產生 _WHILE 標記，例如 _WHILE1:
  } else if (strEqual(node->type, "STMT")) {                                    // 處理 STMT 節點          
    // STMT = return EXP | DECL | ID (EXP_LIST) | ID = EXP | ID OP1
		if (strEqual(child0->type, "return")) { // return EXP                           // 處理 return 指令
      Tree *exp = node->childs->item[1];
	  GenCode(g, exp, rzVar);
			GenPcode(g, "", "return", "", "", rzVar);                                    // 中間碼： 例如 RET sum
		} else if (strEqual(child0->type, "DECL")) { // DECL				// 產生 DECL 的程式碼
			Tree *decl = child0;
			GenCode(g, decl, nullVar);
		} else if (strEqual(child0->type, ID)) { // ID : 可能是 ID (EXP_LIST) | ID = EXP | ID OP1
      Tree *id = node->childs->item[0];                                         //   取得子節點              
	  if (node->childs->count <=1)
	    ERROR();
      Tree *op = node->childs->item[1];                                                                   
      if (strEqual(op->type, "(")) {                                            // 處理 id(EXP_LIST)
				Tree *expList = node->childs->item[2];
				GenPcode(g, "", "call", id->value, "", nullVar);
	  } if (strEqual(op->type, "=")) {                                          // 處理 id= EXP
				// STMT 是 ID = EXP
        Tree *exp = node->childs->item[2];                                                                
        char expVar[100];                                                          
        GenCode(g, exp, expVar);                                                //  遞迴產生 EXP            
        GenPcode(g, "", "=", expVar, "", id->value);                            //  中間碼：例如 = 0 sum     
        strcpy(rzVar, expVar);                                                  //  傳回 EXP 的變數，例如 T0  
      } else if (strPartOf(op->type, OP1)) { // STMT 是 ID OP1                             					// 處理 id++ 或 id--
        char addsub[2] = " ";
		addsub[0] = op->type[0];
        GenPcode(g, "", addsub, id->value, "1", id->value);                     //  中間碼：例如 + i, 1, i
        strcpy(rzVar, id->value);                                               //  傳回id，例如 i           
      } else
		ERROR();
    }                                                                           
  } else if (strEqual(node->type, "EXP")) {                                  // 處理 EXP
		// EXP = TERM (OP2 TERM)?
    char var1[100], var2[100];
		Tree *term1 = node->childs->item[0];                                           //   取得子節點
		GenCode(g, term1, var1);
		if (node->childs->count > 1) {
			Tree *op = node->childs->item[1];
			Tree *term2 = node->childs->item[2];
			GenCode(g, term2, var2);
			GenTempVar(g, rzVar);
			GenPcode(g, "", op->value, var1, var2, rzVar);
		} else 
	    strcpy(rzVar, var1);
	} else if (strEqual(node->type, "TERM")) { // 處理 TERM
		// TERM = TERM = ( EXP (OP2 EXP)? ) | ITEM
		if (node->childs->count == 1) { // ITEM// ( EXP OP2 EXP )
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
	    GenPcode(g, "", op->value, var1, var2, rzVar);
			} else
				strcpy(rzVar, var1);
	  }
		else ERROR();
  } else if (strEqual(node->type, "ITEM")) { // 處理 ITEM
    // ITEM = INT | FLOAT | STRING | ID | ID(EXP_LIST?)
		Tree *child0 = node->childs->item[0];
	if (node->childs->count == 1) // INT | FLOAT | STRING | ID
			GenCode(g, child0, rzVar);
		else{
			// ERROR();
			Tree *id = child0;
			if (node->childs->count == 4) {
				Tree *expList = node->childs->item[2];
				for (i=0; i<expList->childs->count; i+=2) {
					Tree *exp = expList->childs->item[i];
					char expVar[100];
					GenCode(g, exp, expVar);
					GenPcode(g, "", "arg", expVar, "", "");
				}
				GenTempVar(g, rzVar);
				GenPcode(g, "", "call", id->value, "", rzVar);
			}
		}
  } else if (strPartOf(node->type, "|integer|float|id|")) {                            // 處理 number, id 節點
    // 遇到變數或常數，傳回其 value 名稱。                                                         
    strcpy(rzVar, node->value);                                                 // 直接傳回 id 或 number
  } else if (node->childs != NULL) {                                            // 其他情況           
    // 其他狀況，若有子代則遞回處理                                                                
    int i;                                                                                         
    for (i=0; i<node->childs->count; i++)                                       // 遞迴處理所有子節點
      GenCode(g, node->childs->item[i], nullVar);
  }
	//  debug("_GenCode:NODE:type=%s value=%s\n", node->type, node->value);
}

// 輸出pcode後再轉為組合語言
void GenPcode(Generator* g, char* label, char* op, char* p1, char* p2, char* pTo) { 
    char tlabel[100] = "";
  if (strlen(label)>0)                                                                                                      
        sprintf(tlabel, "%s:", label);
	pcode(stdout, tlabel, op, p1, p2, pTo); // 印出 pcode
	PcodeToAsm(g->asmFile, tlabel, op, p1, p2, pTo); // 將 pcode 轉為組合語言
}                                                                                                        
                                                                                                                            
void pcode(FILE *file, char* label, char* op, char* p1, char* p2, char* pTo) {
	fprintf(file, "%-8s", label);
	pcodeNoLabel(file, op, p1, p2, pTo);
}                                                                                                  
                                                                                                                            
void pcodeNoLabel(FILE *file, char* op, char* p1, char* p2, char* pTo) {		
	if (strPartOf(op, "|global|local|arg|param|return|goto|method|_method|"))
  	    fprintf(file, "%s %s %s %s\n", op, p1, p2, pTo);
	else if (strPartOf(op, "|call|"))
  	    fprintf(file, "%s = %s(...)\n", pTo, p1);
  	else if (strPartOf(op, "|J==|J!=|J>|J<|J>=|J<=|"))
        fprintf(file, "if %s %s %s goto %s\n", p1, &op[1], p2, pTo);
	else if (strEqual(op, "="))
  	    fprintf(file, "%s = %s\n", pTo, p1);
	else if (strEqual(op, ""))
  	    fprintf(file, "%s %s %s %s\n", pTo, p1, op, p2);
	else
  	    fprintf(file, "%s = %s %s %s\n", pTo, p1, op, p2);
}
                                                                                                        
void GenTempVar(Generator *g, char *tempVar) {                                                          
  sprintf(tempVar, "T%d", g->varCount++);
}

/*
void negateOp(char *condOp, char *negOp) {
if (strEqual(condOp, "==")) strcpy(negOp, "!=");
if (strEqual(condOp, "!=")) strcpy(negOp, "==");
if (strEqual(condOp, ">=")) strcpy(negOp, "<");
if (strEqual(condOp, "<=")) strcpy(negOp, ">");
if (strEqual(condOp, ">")) strcpy(negOp, "<=");
if (strEqual(condOp, "<")) strcpy(negOp, ">=");
}
*/
