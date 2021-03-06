#include "Vm.h"

void VmTest() {
     execute("sum.obj1", 0); // irqTimes = 0 代表不中斷。 
}

void execute(char *objFile, int irqTimes) {       // 虛擬機器主函數
	printf("===Vm:run %s===\n", objFile);
	Vm *vm = VmNew(objFile); // 建立 M0 物件
	vm->irqTimes = irqTimes; // 每 irqTimes 個指令發生一次時間中斷
    vm->irqHandler = VmIrqHandler;
    vm->swiHandler = VmSwiHandler;
	VmRun(vm);	// 開始執行
	VmDump(vm); // 傾印暫存器
	VmFree(vm); // 釋放記憶體 
}

Vm* VmNew(char *objFile) {      // 建立 CPU0 物件                 
	Vm *vm=ObjNew(Vm, 1);                    //  分配 CPU0 物件空間 
	vm->m = fileToBytes(objFile, &vm->mSize); //  載入映像檔 objFile 到記憶體 m 中，預設記憶體大小為 64K=65536 
	vm->irqHandler = NULL;
	vm->swiHandler = NULL;
	vm->isTrace = TRUE;
	vm->count = 0;
	vm->opTable = OpTableNew();
	return vm;                                                                
}                                                                             

void VmFree(Vm *vm) {                      // 刪除 CPU0 物件                 
	OpTableFree(vm->opTable);
	memFree(vm->m);                           //  釋放 CPU0 的 memory 
	ObjFree(vm);                                 //  釋放 CPU0 物件                                         
}                                                                                         

int cmp(int a, int b) {
    int sw = 0;
	if (a > b) {                                 // > : SW(N=0, Z=0)
	  sw &= 0x3FFFFFFF;                          // N=0, Z=0
	} else if (a < b) {                          // < : SW(N=1, Z=0, ....)                                                
	  sw |= 0x80000000;                                  // N=1;
	  sw &= 0xBFFFFFFF;                                  // Z=0;
	} else {                                             // = : SW(N=0, Z=1)                      
	  sw &= 0x7FFFFFFF;                                  // N=0;
	  sw |= 0x40000000;                                  // Z=1;
	}
	return sw;
}

void VmRun(Vm *vm) {                       // 虛擬機器的主要執行函數                   
	char buffer[200];
    Op *op;
	int tPC;
	PC = 0;                                        // 設定起始位址，準備開始執行                             
	LR = -1;                                                                                
	BOOL stop = FALSE;                                                                                  
	while (!stop) {      
		if (I==1 && vm->irqHandler!= NULL) vm->irqHandler(vm);
		R[0] = 0;                                                // R[0] 永遠為 0                            
		LoadInt32(IR, M, PC);                                    // 指令擷取，IR=[PC..PC+3]                                 
		tPC = PC;
		PC += 4;                                                 // 擷取完將 PC 加 4，指向下一個指令
		OP = bits(IR, 24, 31);                                   // 取得 op 欄位，IR[24..31]
		RA = bits(IR, 20, 23);                                   // 取得 ra 欄位，IR[20..23]    
		RB = bits(IR, 16, 19);                                   // 取得 rb 欄位，IR[16..19]
		RC = bits(IR, 12, 15);                                   // 取得 rc 欄位，IR[12..15]
		C5 = bits(IR,  0, 4);                                    // 取得 5  位元的 cx 
		C12= bits(IR, 0, 11);                                    // 取得 12 位元的 cx 
		C16= bits(IR, 0, 15);                                    // 取得 16 位元的 cx                         
		C24= bits(IR, 0, 23);                                    // 取得 24 位元的 cx                                         
		N  = bits(SW, 31, 31);
		Z  = bits(SW, 30, 30);
		I  = bits(SW, 7, 7);
		T  = bits(SW, 6, 6);
		MODE  = bits(SW, 0, 0);
		if (bits(IR, 11, 11)!=0) C12 |= 0xFFFFF000;              // 若 cx 為負數，則調整為2補數格式                                         
		if (bits(IR, 15, 15)!=0) C16 |= 0xFFFF0000;              // 若 cx 為負數，則調整為2補數格式                                        
		if (bits(IR, 23, 23)!=0) C24 |= 0xFF000000;              // 若 cx 為負數，則調整為2補數格式                                                                                 
		CADDR = R[RB]+C16;                                       // 取得位址[Rb+cx]                          
		RADDR = R[RB]+R[RC];                                     // 取得位址[Rb+Rc]                               
//		op = HashTableGet(vm->opTable, );
		switch (OP) {                                            // 根據op執行動作                           
	  case OP_LD : LoadInt32(R[RA], M, CADDR); break;        // 處理 LD 指令                            
	  case OP_ST : StoreInt32(R[RA], M, CADDR); break;       // 處理 ST 指令                            
	  case OP_LDB: LoadByte(R[RA], M, CADDR); break;         // 處理 LDB 指令                            
	  case OP_STB: StoreByte(R[RA], M, CADDR); break;        // 處理 STB 指令                            
	  case OP_LDR: LoadInt32(R[RA], M, RADDR); break;        // 處理 LDR 指令                           
	  case OP_STR: StoreInt32(R[RA], M, RADDR); break;       // 處理 STR 指令                           
	  case OP_LBR: LoadByte(R[RA], M, RADDR); break;         // 處理 LBR 指令                           
	  case OP_SBR: StoreByte(R[RA], M, RADDR); break;        // 處理 SBR 指令                           
	  case OP_LDI: R[RA] = C16; break;                       // 處理 LDI 指令                           
	  case OP_CMP: {                                         // 處理CMP指令，根據比較結果，設定 N,Z 旗標 
	      SW = cmp(R[RA], R[RB]);
//          debug("cmp(%d,%d): SW=%d\n", R[RA], R[RB], SW);
		  RA = 12;
		  break;                                                                                        
        } 
      case OP_MOV: R[RA] = R[RB]; break;                     // 處理MOV指令
	  case OP_ADD: R[RA] = R[RB] + R[RC]; break;             // 處理ADD指令                             
	  case OP_SUB: R[RA] = R[RB] - R[RC]; break;             // 處理SUB指令                             
	  case OP_MUL: R[RA] = R[RB] * R[RC]; break;             // 處理MUL指令                             
	  case OP_DIV: R[RA] = R[RB] / R[RC]; break;             // 處理DIV指令                             
	  case OP_MOD: R[RA] = R[RB] % R[RC]; break;             // 處理MOD指令                             
	  case OP_AND: R[RA] = R[RB] & R[RC]; break;             // 處理AND指令                             
	  case OP_OR:  R[RA] = R[RB] | R[RC]; break;             // 處理OR指令                              
	  case OP_XOR: R[RA] = R[RB] ^ R[RC]; break;             // 處理XOR指令              
      case OP_LAND:R[RA] = R[RB] && R[RC]; break;            // 處理LAND指令  
      case OP_LOR: R[RA] = R[RB] || R[RC]; break;            // 處理LOR指令  
      case OP_EQ:  R[RA] = R[RB] == R[RC]; break;            // 處理EQ指令  
      case OP_NE:  R[RA] = R[RB] != R[RC]; break;            // 處理NE指令  
      case OP_GT:  R[RA] = R[RB] > R[RC]; break;             // 處理GT指令  
      case OP_LT:  R[RA] = R[RB] < R[RC]; break;             // 處理LT指令  
      case OP_GE:  R[RA] = R[RB] >= R[RC]; break;            // 處理GE指令  
      case OP_LE:  R[RA] = R[RB] <= R[RC]; break;            // 處理LE指令  
	  case OP_ROL: R[RA] = ROL(R[RB],C5); break;             // 處理ROL指令                             
	  case OP_ROR: R[RA] = ROR(R[RB],C5); break;             // 處理ROR指令                             
	  case OP_SHL: R[RA] = SHL(R[RB],C5); break;             // 處理SHL指令                             
	  case OP_SHR: R[RA] = SHR(R[RB],C5); break;             // 處理SHR指令                             
	  case OP_JEQ: if (Z==1) PC += C24; break;               // 處理JEQ指令 Z=1
	  case OP_JNE: if (Z==0) PC += C24; break;               // 處理JNE指令 Z=0 
	  case OP_JLT: if (N==1&&Z==0) PC += C24; break;         // 處理JLT指令 NZ=10 
	  case OP_JGT: if (N==0&&Z==0) PC += C24; break;         // 處理JGT指令 NZ=00
	  case OP_JLE:                                           // 處理JLE指令 NZ=10 or 01
		  if ((N==1&&Z==0)||(N==0&&Z==1)) PC+=C24; break;
	  case OP_JGE:                                           // 處理JGE指令 NZ=00 or 01
		  if ((N==0&&Z==0)||(N==0&&Z==1)) PC+=C24; break;
	  case OP_JMP: PC+=C24; break;                           // 處理JMP指令                             
	  case OP_SWI: 
		  if (T==1 && vm->swiHandler != NULL) 
			  vm->swiHandler(vm); 
		  break;                   // 處理SWI指令                             
	  case OP_CALL:LR=PC; PC+=C24; break;                    // 處理JSUB指令                            
	  case OP_RET: if (LR<0) stop=TRUE; else PC=LR; break;   // 處理RET指令                             
	  case OP_PUSH:StoreInt32(R[RA], M, SP); SP+=4; break;   // 處理PUSH指令，注意：原為 SP-=4; 改為 SP+=4; 
	  case OP_POP: SP-=4; LoadInt32(R[RA], M, SP); break;    // 處理POP指令，注意：原為 SP+=4; 改為 SP-=4; 
	  case OP_PUSHB:StoreByte(R[RA], M, SP); SP++; break;    // 處理PUSHB指令，注意：原為 SP--; 改為 SP++;  
	  case OP_POPB: SP--; LoadByte(R[RA], M, SP); break;     // 處理POPB指令 ，注意：原為 SP++; 改為 SP--;                           
	  default: printf("Error:invalid op (%02x) ", OP);                                                
		}     
		if (vm->isTrace)
			printf("PC=%08x LR=%08x IR=%08x SW=%08x R[%02d]=0x%08x=%d count=%d\n", tPC, LR, IR, SW, RA, R[RA], R[RA], vm->count);
		strToUpper(buffer);
	}                                                                                                   
}                                                                                       

void VmDump(Vm *vm) {                      // 印出所有暫存器                           
	printf("\n===CPU0 dump registers===\n");                                                            
	printf("IR =0x%08x=%d\n", IR, IR);             // 印出 IR                                                                           
	int i;                                                                   
	for (i=0; i<16; i++)                           // 印出 R0..R15  
		printf("R[%02d]=0x%08x=%d\n", i, R[i], R[i]);
}

void VmSwiHandler(Vm *vm) {
	if (OP != OP_SWI) return;
	if (C24 == 0) { system("pause"); }
	if (C24 == 1) { vm->isTrace = TRUE; printf("SWI(1):TRACE enable\n"); }
	if (C24 == 2) { vm->isTrace = FALSE; printf("SWI(2):TRACE disable\n"); }
	if (C24 == 3) { printf("SWI(3):%s\n", &M[R[9]]); }
	if (C24 == 4) { 
		printf("SWI(4):Integer(R9=%d)\n", R[9]); 
		system("pause");
	}
	if (C24 == 5) { vm->count = 0; }
}

void VmIrqHandler(Vm *vm) {
	vm->count ++;
	if (vm->irqTimes > 0) {
	   if (vm->count % vm->irqTimes == 0) { // 每 irqTimes 個指令發生一次時間中斷 
		  LR = PC;
		  PC = IrqAddress;
	   } 
    }
}

