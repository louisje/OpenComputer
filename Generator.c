#include "Generator.h"

// �{�����;����D�n��ơC
void generate(Tree *tree, char *asmFile) {      // �N��R�� tree �ର�զX�y���� asmFile
  char nullVar[100]="";                                                             
  Generator *g = GenNew();                      // �}�ҲզX�y���ɥH�K��X              
  g->asmFile = fopen(asmFile, "w");                                                 
  printf("=====PCODE=====\n");                  // ���͵{���X                          
  GenCode(g, tree, nullVar);                    // ���͸�ƫŧi                        
	//  GenData(g);                                   // �����զX�y����
  fclose(g->asmFile);                           // ����O����                          
  GenFree(g);                                   // Ū�J�զX�y���ɨæL�X                
  char *asmText = fileToStr(asmFile);                                              
  printf("=====AsmFile:%s======\n", asmFile);                                       
  printf("%s\n", asmText);                      // ����O����                          
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
	Tree *type = node->childs->item[0],                                           // ���o�l�`�I
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
			sym = SymNew(id->value, type->value); // �N�ܼƥ[�J�Ÿ����C
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
				GenCode(g, exp, expVar);                                                //  ���j���� EXP
				GenPcode(g, "", "=", expVar, "", id->value);                            //  �����X�G�Ҧp = 0 sum
			}
			break;
		}
	}
}

void GenCond(Generator *g, Tree *exp, char *label0) {
    char expVar[100];
	GenCode(g, exp, expVar);                                                     // ���j���� COND
	GenPcode(g, "", "J==", expVar, "0", label0);							   // �����X�G�Ҧp CMP T1, 0, T2
}

void GenCode(Generator *g, Tree *node, char *rzVar) {                          // ���j���͸`�I node ���{���X
	int i;
	char nullVar[100] = "", tempVar[100] = "";

  strcpy(rzVar, "");                                                                                               
	if (node == NULL) return;                                                // ���j�פ����C
                                                                                                                   
	//  debug("GenCode:NODE:type=%s value=%s\n", node->type, node->value);
	Tree *child0 = NULL;
	if (node->childs != NULL) child0 = node->childs->item[0]; // ���o��0�Ӥl�`�I�C

	if (strEqual(node->type, "DECL")) {                                         // �B�z DECL
		GenDecl(g, node, g->scope);
		GenDecl(g, node, CODE);
	} else if (strEqual(node->type, "METHOD")) {                                  // �B�z METHOD
		// METHOD = def ID ( DECL_LIST ) BLOCK
		Tree *id = node->childs->item[1],                                           // ���o�l�`�I
			*declList  = node->childs->item[3],
			*block = node->childs->item[5];
		char methodLabel[100];
		sprintf(methodLabel, id->value);
		GenPcode(g, methodLabel, "method", "", "", "");                                          // ��ƶi�J�I�A�Ҧp _sum:
		g->scope = PARAM;
		GenCode(g, declList, nullVar);
		g->scope = LOCAL;
		GenCode(g, block, nullVar);
		GenPcode(g, "", "_method", "", "", "");                                          // ��ƶi�J�I�A�Ҧp _sum:
		g->scope = GLOBAL;
  } else if (strEqual(node->type, "FOR")) {                                     // �B�z FOR �`�I
		// FOR = for ( STMT ; EXP ; STMT) BASE
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
  } else if (strEqual(node->type, "IF")) {                                     // �B�z FOR �`�I
		// if ( EXP ) BASE ( else BASE )?
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
  } else if (strEqual(node->type, "WHILE")) {                                     // �B�z FOR �`�I
		// while (EXP) BASE
		char wendLabel[100];
		Tree *exp  = node->childs->item[2],                                        // ���o�l�`�I
         *base  = node->childs->item[4];
    int tempWhileCount = g->whileCount++;                                       // �]�w WHILE �j�骺
    sprintf(wendLabel, "_WHILE%d", tempWhileCount);                         //   ���}�аO
		GenCond(g, exp, wendLabel);
    GenCode(g, base, nullVar);                                                  // ���j���� BASE
    GenPcode(g, wendLabel, "", "", "", "");                                     // ���� _WHILE �аO�A�Ҧp _WHILE1:
  } else if (strEqual(node->type, "STMT")) {                                    // �B�z STMT �`�I          
    // STMT = return EXP | DECL | ID (EXP_LIST) | ID = EXP | ID OP1
		if (strEqual(child0->type, "return")) { // return EXP                           // �B�z return ���O
      Tree *exp = node->childs->item[1];
	  GenCode(g, exp, rzVar);
			GenPcode(g, "", "return", "", "", rzVar);                                    // �����X�G �Ҧp RET sum
		} else if (strEqual(child0->type, "DECL")) { // DECL				// ���� DECL ���{���X
			Tree *decl = child0;
			GenCode(g, decl, nullVar);
		} else if (strEqual(child0->type, ID)) { // ID : �i��O ID (EXP_LIST) | ID = EXP | ID OP1
      Tree *id = node->childs->item[0];                                         //   ���o�l�`�I              
	  if (node->childs->count <=1)
	    ERROR();
      Tree *op = node->childs->item[1];                                                                   
      if (strEqual(op->type, "(")) {                                            // �B�z id(EXP_LIST)
				Tree *expList = node->childs->item[2];
				GenPcode(g, "", "call", id->value, "", nullVar);
	  } if (strEqual(op->type, "=")) {                                          // �B�z id= EXP
				// STMT �O ID = EXP
        Tree *exp = node->childs->item[2];                                                                
        char expVar[100];                                                          
        GenCode(g, exp, expVar);                                                //  ���j���� EXP            
        GenPcode(g, "", "=", expVar, "", id->value);                            //  �����X�G�Ҧp = 0 sum     
        strcpy(rzVar, expVar);                                                  //  �Ǧ^ EXP ���ܼơA�Ҧp T0  
      } else if (strPartOf(op->type, OP1)) { // STMT �O ID OP1                             					// �B�z id++ �� id--
        char addsub[2] = " ";
		addsub[0] = op->type[0];
        GenPcode(g, "", addsub, id->value, "1", id->value);                     //  �����X�G�Ҧp + i, 1, i
        strcpy(rzVar, id->value);                                               //  �Ǧ^id�A�Ҧp i           
      } else
		ERROR();
    }                                                                           
  } else if (strEqual(node->type, "EXP")) {                                  // �B�z EXP
		// EXP = TERM (OP2 TERM)?
    char var1[100], var2[100];
		Tree *term1 = node->childs->item[0];                                           //   ���o�l�`�I
		GenCode(g, term1, var1);
		if (node->childs->count > 1) {
			Tree *op = node->childs->item[1];
			Tree *term2 = node->childs->item[2];
			GenCode(g, term2, var2);
			GenTempVar(g, rzVar);
			GenPcode(g, "", op->value, var1, var2, rzVar);
		} else 
	    strcpy(rzVar, var1);
	} else if (strEqual(node->type, "TERM")) { // �B�z TERM
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
  } else if (strEqual(node->type, "ITEM")) { // �B�z ITEM
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
  } else if (strPartOf(node->type, "|integer|float|id|")) {                            // �B�z number, id �`�I
    // �J���ܼƩα`�ơA�Ǧ^�� value �W�١C                                                         
    strcpy(rzVar, node->value);                                                 // �����Ǧ^ id �� number
  } else if (node->childs != NULL) {                                            // ��L���p           
    // ��L���p�A�Y���l�N�h���^�B�z                                                                
    int i;                                                                                         
    for (i=0; i<node->childs->count; i++)                                       // ���j�B�z�Ҧ��l�`�I
      GenCode(g, node->childs->item[i], nullVar);
  }
	//  debug("_GenCode:NODE:type=%s value=%s\n", node->type, node->value);
}

// ��Xpcode��A�ର�զX�y��
void GenPcode(Generator* g, char* label, char* op, char* p1, char* p2, char* pTo) { 
    char tlabel[100] = "";
  if (strlen(label)>0)                                                                                                      
        sprintf(tlabel, "%s:", label);
	pcode(stdout, tlabel, op, p1, p2, pTo); // �L�X pcode
	PcodeToAsm(g->asmFile, tlabel, op, p1, p2, pTo); // �N pcode �ର�զX�y��
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
