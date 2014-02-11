#include "Assembler.h"

void assemble(char *asmFile, char *objFile) {                   // 組譯器的主要函數
  printf("Assembler:asmFile=%s objFile=%s\n", asmFile,objFile); // 輸入組合語言、輸出目的檔
  printf("===============Assemble=============\n");
  char *text = fileToStr(asmFile);                             // 讀取檔案到 text 字串中
  Assembler *a = AsmNew();                                      
  AsmPass1(a, text);                                            // 第一階段：計算位址 
  printf("===============SYMBOL TABLE=========\n");             
  HashTableEach(a->symTable, (FuncPtr1) AsmCodePrintln);        // 印出符號表   
  AsmPass2(a);                                                  // 第二階段：建構目的碼 
  AsmSaveObjFile(a, objFile);                                      
  AsmFree(a);                                                   // 輸出目的檔   
	memFree(text);                                             // 釋放記憶體   
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

void AsmPass1(Assembler *a, char *text) {             // 第一階段的組譯           
  int i, address = 0, number;                                                 
  Array* lines = split(text, "\r\n", REMOVE_SPLITER); // 將組合語言分割成一行一行
  ArrayEach(lines, strPrintln);                       // 印出以便觀察           
  printf("=================PASS1================\n");               
  for (i=0; i<lines->count; i++) {                    // 對於每一行                        
      strReplace(lines->item[i], ";", '\0');
      strReplace(lines->item[i], "\t", ' ');
      AsmCode *code = AsmCodeNew(lines->item[i]);     // 建立指令物件
      if (code == NULL) continue;
      code->address = address;                        // 設定該行的位址      
      Op *op = NULL;
      if (code->op != NULL) {
        op = HashTableGet(opTable, code->op);           // 查詢運算碼            
        if (op != NULL) {                               // 如果查到
          code->opCode = op->code;                      //    設定運算碼
          code->type = op->type;                        //    設定型態
        }                                                  
      }
      if (code->label != NULL)                        // 如果有標記符號
        HashTablePut(a->symTable, code->label, code); //    加入符號表中
      ArrayAdd(a->codes, code);                       //  建構指令物件陣列
      AsmCodePrintln(code);                           //    印出觀察
      code->size = AsmCodeSize(code);                 //  計算指令大小
      address += code->size;                          //  計算下一個指令位址
  }                                                                           
  ArrayFree(lines, strFree);                          // 釋放記憶體
}

void AsmPass2(Assembler *a) {                         // 組譯器的第二階段
  printf("=============PASS2==============\n");                       
  int i;                                                              
  for (i=0; i<a->codes->count; i++) {                 // 對每一個指令    
    AsmCode *code = ArrayGet(a->codes, i);                                
    AsmTranslateCode(a, code);                        //   進行編碼動作  
    AsmCodePrintln(code);
  }
}

void AsmTranslateCode(Assembler *a, AsmCode *code) {                       // 指令的編碼函數           
  char cxCode[9]="00000000";
	code->objCode = memNew(code->size*2+1);
  memset(code->objCode, '\0', code->size*2+1);                              // 目的碼為 0000…
  switch (code->type) {                                                    // 根據指令型態             
    case 'J' :                                                             // 處理 J 型指令              
      AsmCodeParse(code, a);
      sprintf(cxCode, "%8x", code->cx);
      sprintf(code->objCode, "%2x%s", code->opCode, &cxCode[2]);                 //   編出目的碼(16進位)      
      break;                                                                                        
    case 'L' :
      AsmCodeParse(code, a);
      sprintf(cxCode, "%8x", code->cx);
      sprintf(code->objCode, "%2x%x%x%s", code->opCode, code->r[0], code->r[1], &cxCode[4]);
      break;                                                               
    case 'A' :                                                             // 處理 A 型指令         
      AsmCodeParse(code, a);
      sprintf(cxCode, "%8x", code->cx);                                                                   
      sprintf(code->objCode, "%2x%x%x%x%s", code->opCode,code->r[0],code->r[1],code->r[2],&cxCode[5]);   //   編出目的碼(16進位)  
      break;                                                                                        
    case 'D' : {                                                           // 處理是資料宣告                             
      // 我們將資料宣告  RESW, RESB, WORD, BYTE 也視為一種指令，其形態為 D
      Array *tokens = code->tokens;
      char format4[]="%8x", format1[]="%2x", *format = format1;            
      switch (code->opCode) {                                              // 如果是 RESW    
        case OP_RESW:                                                      //       或 RESB 
        case OP_RESB:
          memset(code->objCode, '0', code->size*2);                              // 目的碼為 0000….         
          break;                                                           // 如果是 WORD:            
        case OP_WORD:                                                                
          format = format4;                                                // 設定輸出格式為 %8x 
        case OP_BYTE: {                                                    // 如果是 BYTE : 輸出格式為 %2x
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
    if (sscanf(code->arg[i], "R%d", &code->r[rIdx])>0)           // 可能是暫存器，例如 R1 
      rIdx++;
    else if (sscanf(code->arg[i], "%d", &code->cx) == 0) {       // 可能是常數，例如 37 
      AsmCode *labelCode = HashTableGet(a->symTable, code->arg[i]);       // 可能是符號
      if (labelCode != NULL) {
        code->cx = labelCode->address - code->address - 4;             // 計算 cx 欄位，相對於 PC 編碼 
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

int AsmCodeSize(AsmCode *code) {                    // 計算指令的大小     
  Array *tokens = code->tokens;
  char  *arg0 = AsmCodeArg(code, 0);
  switch (code->opCode) {                           // 根據運算碼 op
    case OP_RESW :                                  //  如果是RESW       
      return 4 * atoi(arg0);                  //   大小為 4*保留量  
    case OP_RESB :                                  // 如果是RESB        
      return atoi(arg0);                      //   大小為 1*保留量  
    case OP_WORD :                                  // 如果是WORD        
      return 4 * (strCountChar(arg0, ",")+1); //   大小為 4*參數個數
    case OP_BYTE : {                                 // 如果是BYTE        
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
    case OP_NULL :                                  // 如果只是標記                      
      return 0;                                     //   大小為 0        
    default :                                       // 其他情形 (指令)   
      return 4;                                     //   大小為 4
  }                                                   
}

char *AsmCodeArg(AsmCode *code, int i) {
  int argIdx = code->argStart + i;
  if (argIdx < code->tokens->count)
    return code->tokens->item[argIdx];
  return NULL;
}
