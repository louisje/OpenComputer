#include "Generator.h"

// �{�����;����D�n��ơC
void generate(Tree *tree, SymTable *symTable, char *asmFile) {      // �N��R�� tree �ର�զX�y���� asmFile
  char nullVar[100]="";                                                             
  Generator *g = GenNew();                      // �}�ҲզX�y���ɥH�K��X              
	g->symTable = symTable;
  g->asmFile = fopen(asmFile, "w");                                                 
	printf("=====PCODE=====\n");                  
	GenPcode(g, "", ".code", "", "", "");
	GenCode(g, tree, nullVar);                    // ���͵{���X 
	GenData(g);                                   // �����զX�y����
  fclose(g->asmFile);                           // ����O����                          
  GenFree(g);                                   // Ū�J�զX�y���ɨæL�X                
  char *asmText = fileToStr(asmFile);                                              
  printf("=====AsmFile:%s======\n", asmFile);                                       
  printf("%s\n", asmText);                      // ����O����                          
	memFree(asmText);
}

Generator *GenNew() {
  Generator *g = ObjNew(Generator, 1);
//	g->scope = SCOPE_GLOBAL;
	g->pCodeStyle = CStyle;
  return g;
}

void GenFree(Generator *g) {
  ObjFree(g);
}

void GenDecl(Generator *g, Tree *node) {
	// DECL = TYPE VAR_LIST
	Tree *type = node->childs->item[0],                                           // ���o�l�`�I
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
			sym = SymNew(id->token, type->token); // �N�ܼƥ[�J�Ÿ����C
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
                ERROR();                                   //  ���j���� EXP
//				GenPcode(g, "", "=", expVar, "", id->token->str);                            //  �����X�G�Ҧp = 0 sum
		}
/*			break;
		} */
	}
}

void GenCond(Generator *g, Tree *exp, char *label0) {
    char expVar[100];
	GenCode(g, exp, expVar);                                                     // ���j���� COND
	GenPcode(g, "", "J==", expVar, "0", label0);							   // �����X�G�Ҧp CMP T1, 0, T2
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

void GenCode(Generator *g, Tree *node, char *rzVar) {                          // ���j���͸`�I node ���{���X
	int i;
	char nullVar[100] = "", tempVar[100] = "";

  strcpy(rzVar, "");                                                                                               
	if (node == NULL) return;                                                // ���j�פ����C
                                                                                                                   
	//  debug("GenCode:NODE:type=%s value=%s\n", node->tag, node->token);
	Tree *child0 = NULL;
	if (node->childs != NULL) child0 = node->childs->item[0]; // ���o��0�Ӥl�`�I�C

	if (node->tag == METHOD) { // �B�z METHOD
		// METHOD = ETYPE ** ID(PARAM_LIST?) BLOCK
		Tree *id = node->childs->item[1],                                           // ���o�l�`�I
			*declList  = node->childs->item[3],
			*block = node->childs->item[5];
		char methodLabel[100];
		sprintf(methodLabel, token(id));
		GenPcode(g, methodLabel, "method", "", "", "");                                          // ��ƶi�J�I�A�Ҧp _sum:
//		g->scope = PARAM;
		GenCode(g, declList, nullVar);
//		g->scope = LOCAL;
		GenCode(g, block, nullVar);
		GenPcode(g, "", "_method", "", "", "");                                          // ��ƶi�J�I�A�Ҧp _sum:
//		g->scope = GLOBAL;
	} else if (node->tag == FOR) {                                     // �B�z FOR �`�I
		// FOR = for (STMT ; EXP ; STMT) BASE
    char forBeginLabel[100], forEndLabel[100], condOp[100];                     
    Tree *stmt1 = node->childs->item[2],                                        // ���o�l�`�I                         
			*exp  = node->childs->item[4],
         *stmt2 = node->childs->item[6],                                                                           
         *base  = node->childs->item[8];
    GenCode(g, stmt1, nullVar);                                                 // ���j���� STMT                             
    int tempForCount = g->forCount++;                                           // �]�wFOR�j�骺                 
    sprintf(forBeginLabel, "FOR%d", tempForCount);                              //   �i�J�аO                                
    sprintf(forEndLabel, "_FOR%d", tempForCount);                               //   ���}�аO            
    GenPcode(g, forBeginLabel, "", "", "", "");                                 // �����X�G�Ҧp FOR1:    
		GenCond(g, exp, forEndLabel);
    GenCode(g, base, nullVar);                                                 // ���j���� BLOCK
    GenCode(g, stmt2, nullVar);                                                 // ���j���� STMT        
		GenPcode(g, "", "goto", "", "", forBeginLabel);                                // �����X�G�ҦpJ FOR1
    GenPcode(g, forEndLabel, "", "", "", "");                                   // �����X�G�Ҧp _FOR1    
	} else if (node->tag == IF) {                                     // �B�z IF �`�I
		// IF = if (EXP) BASE (else BASE)?
		char elseLabel[100];
		Tree *exp  = node->childs->item[2],                                        // ���o�l�`�I
         *base1 = node->childs->item[4],
         *base2 = node->childs->item[7];
		int tempIfCount = g->ifCount++;
		sprintf(elseLabel, "ELSE%d", tempIfCount);
		GenCond(g, exp, elseLabel);
    GenCode(g, base1, nullVar);                                                 // ���j���� BASE1
    GenPcode(g, elseLabel, "", "", "", "");                                     // ���� ELSE �аO�A�Ҧp ELSE1:
    GenCode(g, base2, nullVar);                                                 // ���j���� STMT
	} else if (node->tag == WHILE) {                                     // �B�z WHILE �`�I
		// WHILE = while (EXP) BASE
		char wendLabel[100];
		Tree *exp  = node->childs->item[2],                                        // ���o�l�`�I
         *base  = node->childs->item[4];
    int tempWhileCount = g->whileCount++;                                       // �]�w WHILE �j�骺
    sprintf(wendLabel, "_WHILE%d", tempWhileCount);                         //   ���}�аO
		GenCond(g, exp, wendLabel);
    GenCode(g, base, nullVar);                                                  // ���j���� BASE
    GenPcode(g, wendLabel, "", "", "", "");                                     // ���� _WHILE �аO�A�Ҧp _WHILE1:
	} else if (node->tag == STRUCT) {                                     // �B�z STRUCT �`�I
		// STRUCT = struct ID { DECL_LIST ; }
		Tree *id = node->childs->item[1];
		GenPcode(g, token(id), "STRUCT", "", "", "");
//		g->scope = STRUCT;
		for (i=3; i<node->childs->count-1; i+=2) {
            Tree *decl = node->childs->item[i];
            GenCode(g, decl, nullVar);
        }
		GenPcode(g, "", "_STRUCT", "", "", "");
	} else if (node->tag == STMT) {                                    // �B�z STMT �`�I
	    // STMT = return EXP | DECL | PATH (EXP_LIST) | PATH = EXP | PATH OP1
		// ** STMT = return EXP | DECL | ID (EXP_LIST) | ID = EXP | ID OP1
		if (child0->tag == kRETURN) { // return EXP                           // �B�z return ���O
      Tree *exp = node->childs->item[1];
	  GenCode(g, exp, rzVar);
			GenPcode(g, "", "return", "", "", rzVar);                                    // �����X�G �Ҧp RET sum
		} else if (child0->tag == DECL) { // DECL				          // ���� DECL ���{���X
			Tree *decl = child0;
			GenCode(g, decl, nullVar);
		} else if (child0->tag = PATH) { // ID : �i��O PATH = EXP | PATH (EXP_LIST) | PATH OP1
			Tree *path = node->childs->item[0]; // ���o�l�`�I
			char pathVar[100];
			GenCode(g, path, pathVar);
	  if (node->childs->count <=1)
	    ERROR();
      Tree *op = node->childs->item[1];                                                                   
			if (op->tag == sLPAREN /* ( */) {                                             // �B�z id(EXP_LIST)
				Tree *expList = node->childs->item[2];
				GenCall(g, path, expList, rzVar);
			} if (op->tag == sASSIGN /* = */) {                                          // �B�z id= EXP
				// STMT �O ID = EXP
        Tree *exp = node->childs->item[2];                                                                
        char expVar[100];                                                          
        GenCode(g, exp, expVar);                                                //  ���j���� EXP            
				GenPcode(g, "", "=", expVar, "", pathVar);                            //  �����X�G�Ҧp = 0 sum
        strcpy(rzVar, expVar);                                                  //  �Ǧ^ EXP ���ܼơA�Ҧp T0  
			} else if (op->tag == sDPLUS /* ++ */ || op->tag == sDMINUS /* -- */) { // STMT �O ID OP1                             					// �B�z id++ �� id--
            	GenPcode(g, "", op->tag, pathVar, "", pathVar); //  �����X�G�Ҧp ++ i   i
      } else
		ERROR();
    }                                                                           
	} else if (node->tag == EXP) {                                  // �B�z EXP
		// EXP = TERM (OP2 TERM)?
    char var1[100], var2[100];
		Tree *term1 = node->childs->item[0];                                           //   ���o�l�`�I
		GenCode(g, term1, var1);
		if (node->childs->count > 1) {
			Tree *op = node->childs->item[1];
			Tree *term2 = node->childs->item[2];
			GenCode(g, term2, var2);
			GenTempVar(g, rzVar);
			GenPcode(g, "", token(op), var1, var2, rzVar);
		} else 
	    strcpy(rzVar, var1);
	} else if (node->tag == TERM) { // �B�z TERM
	    // TERM = ( EXP (OP2 EXP)? ) | CINT | CFLOAT | CSTR | PATH
		// ** TERM = TERM = ( EXP (OP2 EXP)? ) | ITEM
		SemTerm *sem = node->sem;
		if (node->childs->count == 1) { // ITEM
			Tree *item = node->childs->item[0];
			GenCode(g, child0, rzVar);
		} else if (sem->subTag == EXP) { // ( EXP )
			char var1[100], var2[100];
			Tree *exp1 = node->childs->item[1];
	    Tree *op = node->childs->item[2];
			GenCode(g, exp1, var1);
			if (node->childs->count == 5) { // EXP OP2 EXP
  	    Tree *exp2 = node->childs->item[3];
	    GenCode(g, exp2, var2);
	    GenTempVar(g, rzVar);
				GenPcode(g, "", token(op), var1, var2, rzVar);
			} else
				strcpy(rzVar, var1);
	  }
	} else if (node->tag == PATH) { // �B�z PATH
        // PATH = ATOM ((.|->) ATOM)*
	} else if (node->tag == ATOM) { // �B�z ATOM
	    // ATOM = ID (([ EXP ])* |( EXP_LIST? ))
	} else if (node->tag == VAR) { // �B�z VAR
        // VAR = ** ID ([ integer ])* (= EXP)?
	} else if (node->tag == DECL) { // �B�z DECL
	    // DECL = ETYPE VAR_LIST
		GenDecl(g, node);
/*		if (g->scope != STRUCT)
		  GenDecl(g, node, CODE); */
	} else if (node->tag == PARAM) { // �B�z PARAM
	    // PARAM = ETYPE VAR
	} else if (node->tag == CINT || node->tag == CFLOAT) {                            // �B�z��Ʊ`�� 
		// �J���ܼƩα`�ơA�Ǧ^�� value �W�١C
		strcpy(rzVar, token(node));                                                 // �����Ǧ^ id �� number
	} else if (node->tag == CSTR) {                            // �B�z�r��`��
    // �J���ܼƩα`�ơA�Ǧ^�� value �W�١C                                                         
		GenTempVar(g, rzVar);
  } else if (node->childs != NULL) {                                            // ��L���p           
    // ��L���p�A�Y���l�N�h���^�B�z                                                                
    int i;                                                                                         
    for (i=0; i<node->childs->count; i++)                                       // ���j�B�z�Ҧ��l�`�I
      GenCode(g, node->childs->item[i], nullVar);
  }
	//  debug("_GenCode:NODE:type=%s value=%s\n", node->tag, node->token);
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

// ��Xpcode��A�ର�զX�y��
void GenPcode(Generator* g, char* label, char* op, char* p1, char* p2, char* pTo) { 
	pcode(stdout, label, op, p1, p2, pTo, g->pCodeStyle); // �L�X pcode
	fprintf(g->asmFile, "; ");
	pcode(g->asmFile, label, op, p1, p2, pTo, g->pCodeStyle); // �L�X pcode
//	pcodeToAsm(g->asmFile, label, op, p1, p2, pTo, g->pCodeStyle); // �N pcode �ର�զX�y��
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
