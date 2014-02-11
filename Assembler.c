#include "Assembler.h"

void assemble(char *asmFile, char *objFile) {                   // ��Ķ�����D�n���
  printf("Assembler:asmFile=%s objFile=%s\n", asmFile,objFile); // ��J�զX�y���B��X�ت���
  printf("===============Assemble=============\n");
  char *text = fileToStr(asmFile);                             // Ū���ɮר� text �r�ꤤ
  Assembler *a = AsmNew();                                      
  AsmPass1(a, text);                                            // �Ĥ@���q�G�p���} 
  printf("===============SYMBOL TABLE=========\n");             
  HashTableEach(a->symTable, (FuncPtr1) AsmCodePrintln);        // �L�X�Ÿ���   
  AsmPass2(a);                                                  // �ĤG���q�G�غc�ت��X 
  AsmSaveObjFile(a, objFile);                                      
  AsmFree(a);                                                   // ��X�ت���   
	memFree(text);                                             // ����O����   
}                                                               

Assembler* AsmNew() {
  Assembler *a=ObjNew(Assembler, 1);
  a->codes = ArrayNew(10);
  a->symTable = HashTableNew(127);
  a->opTable = OpTableNew();
  return a;
}

void AsmFree(Assembler *a) {
  ArrayFree(a->codes, (FuncPtr1) AsmCodeFree);
  HashTableFree(a->symTable);
  OpTableFree();
  ObjFree(a);
}

void AsmPass1(Assembler *a, char *text) {             // �Ĥ@���q����Ķ           
  int i, address = 0, number;                                                 
  Array* lines = split(text, "\r\n", REMOVE_SPLITER); // �N�զX�y�����Φ��@��@��
  ArrayEach(lines, strPrintln);                       // �L�X�H�K�[��           
  printf("=================PASS1================\n");               
  for (i=0; i<lines->count; i++) {                    // ���C�@��                        
      strReplace(lines->item[i], ";", '\0');
      strReplace(lines->item[i], "\t", ' ');
      AsmCode *code = AsmCodeNew(lines->item[i]);     // �إ߫��O����
      if (code == NULL) continue;
      code->address = address;                        // �]�w�Ӧ檺��}      
      Op *op = NULL;
      if (code->op != NULL) {
        op = HashTableGet(opTable, code->op);           // �d�߹B��X            
        if (op != NULL) {                               // �p�G�d��
          code->opCode = op->code;                      //    �]�w�B��X
          code->type = op->type;                        //    �]�w���A
        }                                                  
      }
      if (code->label != NULL)                        // �p�G���аO�Ÿ�
        HashTablePut(a->symTable, code->label, code); //    �[�J�Ÿ���
      ArrayAdd(a->codes, code);                       //  �غc���O����}�C
      AsmCodePrintln(code);                           //    �L�X�[��
      code->size = AsmCodeSize(code);                 //  �p����O�j�p
      address += code->size;                          //  �p��U�@�ӫ��O��}
  }                                                                           
  ArrayFree(lines, strFree);                          // ����O����
}

void AsmPass2(Assembler *a) {                         // ��Ķ�����ĤG���q
  printf("=============PASS2==============\n");                       
  int i;                                                              
  for (i=0; i<a->codes->count; i++) {                 // ��C�@�ӫ��O    
    AsmCode *code = ArrayGet(a->codes, i);                                
    AsmTranslateCode(a, code);                        //   �i��s�X�ʧ@  
    AsmCodePrintln(code);
  }
}

void AsmTranslateCode(Assembler *a, AsmCode *code) {                       // ���O���s�X���           
  char cxCode[9]="00000000";
	code->objCode = memNew(code->size*2+1);
  memset(code->objCode, '\0', code->size*2+1);                              // �ت��X�� 0000�K
  switch (code->type) {                                                    // �ھګ��O���A             
    case 'J' :                                                             // �B�z J �����O              
      AsmCodeParse(code, a);
      sprintf(cxCode, "%8x", code->cx);
      sprintf(code->objCode, "%2x%s", code->opCode, &cxCode[2]);                 //   �s�X�ت��X(16�i��)      
      break;                                                                                        
    case 'L' :
      AsmCodeParse(code, a);
      sprintf(cxCode, "%8x", code->cx);
      sprintf(code->objCode, "%2x%x%x%s", code->opCode, code->r[0], code->r[1], &cxCode[4]);
      break;                                                               
    case 'A' :                                                             // �B�z A �����O         
      AsmCodeParse(code, a);
      sprintf(cxCode, "%8x", code->cx);                                                                   
      sprintf(code->objCode, "%2x%x%x%x%s", code->opCode,code->r[0],code->r[1],code->r[2],&cxCode[5]);   //   �s�X�ت��X(16�i��)  
      break;                                                                                        
    case 'D' : {                                                           // �B�z�O��ƫŧi                             
      // �ڭ̱N��ƫŧi  RESW, RESB, WORD, BYTE �]�����@�ث��O�A��κA�� D
      Array *tokens = code->tokens;
      char format4[]="%8x", format1[]="%2x", *format = format1;            
      switch (code->opCode) {                                              // �p�G�O RESW    
        case OP_RESW:                                                      //       �� RESB 
        case OP_RESB:
          memset(code->objCode, '0', code->size*2);                              // �ت��X�� 0000�K.         
          break;                                                           // �p�G�O WORD:            
        case OP_WORD:                                                                
          format = format4;                                                // �]�w��X�榡�� %8x 
        case OP_BYTE: {                                                    // �p�G�O BYTE : ��X�榡�� %2x
          char *objPtr = code->objCode;
          int i=0, si;
          for (i=code->argStart; i<tokens->count; i++) {                                 
              char *item = tokens->item[i];
              if (item[0] == '"') {
                for (si=1; item[si] != '"'; si++) {
                    sprintf(objPtr, "%2x", item[si]);
                    objPtr += 2;
                }
              } else if (isdigit(item[0]))
                sprintf(objPtr, format, atoi(item));
              else {
                AsmCode *itemCode = HashTableGet(a->symTable, item);
                sprintf(objPtr, format, itemCode->address);
              }
              strFree(item);
              objPtr += strlen(objPtr);
          }
          break;
        } // case OP_BYTE:
      } // switch
      break;
    } // case ' '
    default: 
      break;
  }
  strReplace(code->objCode, " ", '0');
  strToUpper(code->objCode);
}

void AsmSaveObjFile(Assembler *a, char *objFile) {
  printf("==========Save to ObjFile:%s==========\n", objFile);
  FILE *file = fopen(objFile, "wb");
  int i;
  for (i=0; i<a->codes->count; i++) {
    AsmCode *code = a->codes->item[i];
    char *objPtr = code->objCode;
    while (*objPtr != '\0') {
      int x;
      sscanf(objPtr, "%2x", &x);
      assert(x >= 0 && x < 256);
      BYTE b = (BYTE) x;
      fwrite(&b, sizeof(BYTE), 1, file);
      objPtr += 2;
      char bstr[3];
      sprintf(bstr, "%2x", b);
      strReplace(bstr, " ", '0');
      strToUpper(bstr);
      printf("%s", bstr);
    }
  }
  printf("\n");
  fclose(file);
}

int AsmCodePrintln(AsmCode *code) {
  printf("%4x %-40s %c %2x %s\n", code->address, code->line, code->type, code->opCode, code->objCode);
}

AsmCode* AsmCodeNew(char *line) {
  AsmCode* code = ObjNew(AsmCode,1);
  Array* tokens = split(line, " \t+[],", REMOVE_SPLITER);
  if (tokens->count == 0) { ArrayFree(tokens, strFree); return NULL; }
	code->line = strNew(line);
  strTrim(code->line, code->line, SPACE);
  code->tokens = tokens;
  int tokenIdx = 0;
  if (strTail(tokens->item[0], ":")) {
      code->label = ArrayGet(tokens, tokenIdx++);
      strTrim(code->label, code->label, ":");
  } else
      code->label = NULL;
  code->op = ArrayGet(tokens, tokenIdx++);
  code->opCode = OP_NULL;
  code->argStart = tokenIdx;
  code->arg[0] = ArrayGet(tokens, tokenIdx++);
  code->arg[1] = ArrayGet(tokens, tokenIdx++);
  code->arg[2] = ArrayGet(tokens, tokenIdx++);
	//  AsmCodePrintln(code);
  return code;
}

void AsmCodeParse(AsmCode *code, Assembler *a) {
  int i, rIdx=0;
  for (i=0; i<3; i++) {
    if (code->arg[i] == NULL) 
       break;
    if (sscanf(code->arg[i], "R%d", &code->r[rIdx])>0)           // �i��O�Ȧs���A�Ҧp R1 
      rIdx++;
    else if (sscanf(code->arg[i], "%d", &code->cx) == 0) {       // �i��O�`�ơA�Ҧp 37 
      AsmCode *labelCode = HashTableGet(a->symTable, code->arg[i]);       // �i��O�Ÿ�
      if (labelCode != NULL) {
        code->cx = labelCode->address - code->address - 4;             // �p�� cx ���A�۹�� PC �s�X 
        code->r[rIdx++] = 15;                                          // rb = PC = R15
      }
    }
  }
  code->rCount = rIdx;
}

void AsmCodeFree(AsmCode *code) {
	memFree(code->line);
  ArrayFree(code->tokens, strFree);
	memFree(code->objCode);
	memFree(code);
}

int AsmCodeSize(AsmCode *code) {                    // �p����O���j�p     
  Array *tokens = code->tokens;
  char  *arg0 = AsmCodeArg(code, 0);
  switch (code->opCode) {                           // �ھڹB��X op
    case OP_RESW :                                  //  �p�G�ORESW       
      return 4 * atoi(arg0);                  //   �j�p�� 4*�O�d�q  
    case OP_RESB :                                  // �p�G�ORESB        
      return atoi(arg0);                      //   �j�p�� 1*�O�d�q  
    case OP_WORD :                                  // �p�G�OWORD        
      return 4 * (strCountChar(arg0, ",")+1); //   �j�p�� 4*�ѼƭӼ�
    case OP_BYTE : {                                 // �p�G�OBYTE        
      int i, si, size=0;
      for (i=code->argStart; i<tokens->count; i++) {
        char *item = tokens->item[i];
        if (item[0] == '"')
          for (si=1; item[si] != '"'; si++)
              size += 1;
        else
          size += 1;
      }
      return size;
    }
    case OP_NULL :                                  // �p�G�u�O�аO                      
      return 0;                                     //   �j�p�� 0        
    default :                                       // ��L���� (���O)   
      return 4;                                     //   �j�p�� 4
  }                                                   
}

char *AsmCodeArg(AsmCode *code, int i) {
  int argIdx = code->argStart + i;
  if (argIdx < code->tokens->count)
    return code->tokens->item[argIdx];
  return NULL;
}
