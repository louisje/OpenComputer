#include "Machine0.h"

void M0SwiHandler(void *cpu) {
  Cpu0 *cpu0 = cpu;
  if (OP != OP_SWI) return;
  if (C24 == 0) { system("pause"); }
  if (C24 == 1) { cpu0->isTrace = TRUE; printf("SWI(1):TRACE enable\n"); }
  if (C24 == 2) { cpu0->isTrace = FALSE; printf("SWI(2):TRACE disable\n"); }
  if (C24 == 3) { printf("SWI(3):%s\n", &M[R[9]]); }
  if (C24 == 4) { 
          printf("SWI(4):Integer(R9=%d)\n", R[9]); 
          system("pause");
  }
  if (C24 == 5) { cpu0->count = 0; }
}

void M0IrqHandler(void *cpu) {
  Cpu0 *cpu0 = cpu;
  cpu0->count ++;
  if (cpu0->count % 10 == 0) { // �C 100 �ӫ��O�o�ͤ@���ɶ����_ 
//    printf("stepCount=%d\n", stepCount);
//    printf("before IRQ:PC=%x LR=%x\n", PC, LR);
    LR = PC;
    PC = IrqAddress;
//    printf("after  IRQ:PC=%x LR=%x\n", PC, LR);
//    printf("=============\n");
  }
//  if (PC == 0x3C)
//    printf("PC=0x3C\n");
//     system("pause");
//  if (stepCount % 300 == 0)
//    system("pause");
}


void M0execute(char *objFile) {               	// ���������D���
  printf("===M0:run %s===\n", objFile);
  Cpu0 *cpu0 = Cpu0New(objFile, M0MemorySize);	// �إ� M0 ����
  cpu0->irqHandler = M0IrqHandler;
  cpu0->swiHandler = M0SwiHandler;
  Cpu0Run(cpu0);                 				// �}�l����
  Cpu0Dump(cpu0);                             	// �ɦL�Ȧs��
  Cpu0Free(cpu0);                             	// ����O���� 
}
