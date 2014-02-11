#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "Cpu0.h"

typedef struct {                                    // ��Ķ������                     
  Array *codes;                                     //   ���O�����C                 
  HashTable *symTable;                              //   �Ÿ���                       
  HashTable *opTable;                               //   ���O��                       
} Assembler;                                                                       
                                                                                   
typedef struct {                                    // ���O����                       
  char *line;
  int  address, opCode, size;                       //   �]�t��}�B�B��X�B           
  char *label, *op, type;                           //   �Ŷ��j�p�Bop, �B�аO�B       
  Array *tokens;
  int  argStart;
  char *objCode; 
  char *arg[3];                                     //   �ѼơB���A�B�ت��X           
  int  r[3];
  int  rCount;
  int  cx;
} AsmCode;                                          //   �����                       
                                                                                   
void assemble(char *asmFile, char *objFile);        // ��Ķ�����D�{��              
                                                                                   
Assembler* AsmNew();                                // ��Ķ�����غc���               
void AsmFree(Assembler *a);                         // ��Ķ�����Ѻc���               
                                                                                   
void AsmPass1(Assembler *a, char *text);            // ��Ķ�����Ĥ@���q               
void AsmPass2(Assembler *a);                        // ��Ķ�����ĤG���q               
void AsmSaveObjFile(Assembler *a, char *objFile);   // �x�s�ت���                     
void AsmTranslateCode(Assembler *a, AsmCode *code); // �N���O�ର�ت��X               
                                                                                   
AsmCode* AsmCodeNew(char *line);                                                   
void AsmCodeParse(AsmCode *code, Assembler *a);
void AsmCodeFree(AsmCode *code);                    // ���O���󪺫غc���             
int AsmCodePrintln(AsmCode *code);                  // ���O���󪺸Ѻc���             
char *AsmCodeArg(AsmCode *code, int i);
// ���O���󪺦C�L���             
#endif
