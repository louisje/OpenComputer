#include "OS.h"

void OSSwiHandler(void *pCpu) {
	Cpu *cpu = pCpu;
	if (OP != OP_SWI) return;
	if (C24 == 0) { system("pause"); }
	if (C24 == 1) { cpu->isTrace = TRUE; printf("SWI(1):TRACE enable\n"); }
	if (C24 == 2) { cpu->isTrace = FALSE; printf("SWI(2):TRACE disable\n"); }
	if (C24 == 3) { printf("SWI(3):%s\n", &M[R[9]]); }
	if (C24 == 4) { 
		printf("SWI(4):Integer(R9=%d)\n", R[9]); 
		system("pause");
	}
	if (C24 == 5) { cpu->count = 0; }
}

void OSIrqHandler(void *pCpu) {
	Cpu *cpu = pCpu;
	cpu->count ++;
	if (cpu->count % 10 == 0) { // 每 10 個指令發生一次時間中斷 
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


void OSexecute(char *objFile) {               	// 虛擬機器主函數
	printf("===M0:run %s===\n", objFile);
	Cpu *cpu = CpuNew(objFile, M0MemorySize);	// 建立 M0 物件
	cpu->irqHandler = OSIrqHandler;
	cpu->swiHandler = OSSwiHandler;
	CpuRun(cpu);                 				// 開始執行
	CpuDump(cpu);                             	// 傾印暫存器
	CpuFree(cpu);                             	// 釋放記憶體 
}
