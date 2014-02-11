#include "Vm.h"

void VmTest() {
     execute("sum.obj1", 0); // irqTimes = 0 �N�����_�C 
}

void execute(char *objFile, int irqTimes) {       // ���������D���
	printf("===Vm:run %s===\n", objFile);
	Vm *vm = VmNew(objFile); // �إ� M0 ����
	vm->irqTimes = irqTimes; // �C irqTimes �ӫ��O�o�ͤ@���ɶ����_
    vm->irqHandler = VmIrqHandler;
    vm->swiHandler = VmSwiHandler;
	VmRun(vm);	// �}�l����
	VmDump(vm); // �ɦL�Ȧs��
	VmFree(vm); // ����O���� 
}

Vm* VmNew(char *objFile) {      // �إ� CPU0 ����                 
	Vm *vm=ObjNew(Vm, 1);                    //  ���t CPU0 ����Ŷ� 
	vm->m = fileToBytes(objFile, &vm->mSize); //  ���J�M���� objFile ��O���� m ���A�w�]�O����j�p�� 64K=65536 
	vm->irqHandler = NULL;
	vm->swiHandler = NULL;
	vm->isTrace = TRUE;
	vm->count = 0;
	vm->opTable = OpTableNew();
	return vm;                                                                
}                                                                             

void VmFree(Vm *vm) {                      // �R�� CPU0 ����                 
	OpTableFree(vm->opTable);
	memFree(vm->m);                           //  ���� CPU0 �� memory 
	ObjFree(vm);                                 //  ���� CPU0 ����                                         
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

void VmRun(Vm *vm) {                       // �����������D�n������                   
	char buffer[200];
    Op *op;
	int tPC;
	PC = 0;                                        // �]�w�_�l��}�A�ǳƶ}�l����                             
	LR = -1;                                                                                
	BOOL stop = FALSE;                                                                                  
	while (!stop) {      
		if (I==1 && vm->irqHandler!= NULL) vm->irqHandler(vm);
		R[0] = 0;                                                // R[0] �û��� 0                            
		LoadInt32(IR, M, PC);                                    // ���O�^���AIR=[PC..PC+3]                                 
		tPC = PC;
		PC += 4;                                                 // �^�����N PC �[ 4�A���V�U�@�ӫ��O
		OP = bits(IR, 24, 31);                                   // ���o op ���AIR[24..31]
		RA = bits(IR, 20, 23);                                   // ���o ra ���AIR[20..23]    
		RB = bits(IR, 16, 19);                                   // ���o rb ���AIR[16..19]
		RC = bits(IR, 12, 15);                                   // ���o rc ���AIR[12..15]
		C5 = bits(IR,  0, 4);                                    // ���o 5  �줸�� cx 
		C12= bits(IR, 0, 11);                                    // ���o 12 �줸�� cx 
		C16= bits(IR, 0, 15);                                    // ���o 16 �줸�� cx                         
		C24= bits(IR, 0, 23);                                    // ���o 24 �줸�� cx                                         
		N  = bits(SW, 31, 31);
		Z  = bits(SW, 30, 30);
		I  = bits(SW, 7, 7);
		T  = bits(SW, 6, 6);
		MODE  = bits(SW, 0, 0);
		if (bits(IR, 11, 11)!=0) C12 |= 0xFFFFF000;              // �Y cx ���t�ơA�h�վ㬰2�ɼƮ榡                                         
		if (bits(IR, 15, 15)!=0) C16 |= 0xFFFF0000;              // �Y cx ���t�ơA�h�վ㬰2�ɼƮ榡                                        
		if (bits(IR, 23, 23)!=0) C24 |= 0xFF000000;              // �Y cx ���t�ơA�h�վ㬰2�ɼƮ榡                                                                                 
		CADDR = R[RB]+C16;                                       // ���o��}[Rb+cx]                          
		RADDR = R[RB]+R[RC];                                     // ���o��}[Rb+Rc]                               
//		op = HashTableGet(vm->opTable, );
		switch (OP) {                                            // �ھ�op����ʧ@                           
	  case OP_LD : LoadInt32(R[RA], M, CADDR); break;        // �B�z LD ���O                            
	  case OP_ST : StoreInt32(R[RA], M, CADDR); break;       // �B�z ST ���O                            
	  case OP_LDB: LoadByte(R[RA], M, CADDR); break;         // �B�z LDB ���O                            
	  case OP_STB: StoreByte(R[RA], M, CADDR); break;        // �B�z STB ���O                            
	  case OP_LDR: LoadInt32(R[RA], M, RADDR); break;        // �B�z LDR ���O                           
	  case OP_STR: StoreInt32(R[RA], M, RADDR); break;       // �B�z STR ���O                           
	  case OP_LBR: LoadByte(R[RA], M, RADDR); break;         // �B�z LBR ���O                           
	  case OP_SBR: StoreByte(R[RA], M, RADDR); break;        // �B�z SBR ���O                           
	  case OP_LDI: R[RA] = C16; break;                       // �B�z LDI ���O                           
	  case OP_CMP: {                                         // �B�zCMP���O�A�ھڤ�����G�A�]�w N,Z �X�� 
	      SW = cmp(R[RA], R[RB]);
//          debug("cmp(%d,%d): SW=%d\n", R[RA], R[RB], SW);
		  RA = 12;
		  break;                                                                                        
        } 
      case OP_MOV: R[RA] = R[RB]; break;                     // �B�zMOV���O
	  case OP_ADD: R[RA] = R[RB] + R[RC]; break;             // �B�zADD���O                             
	  case OP_SUB: R[RA] = R[RB] - R[RC]; break;             // �B�zSUB���O                             
	  case OP_MUL: R[RA] = R[RB] * R[RC]; break;             // �B�zMUL���O                             
	  case OP_DIV: R[RA] = R[RB] / R[RC]; break;             // �B�zDIV���O                             
	  case OP_MOD: R[RA] = R[RB] % R[RC]; break;             // �B�zMOD���O                             
	  case OP_AND: R[RA] = R[RB] & R[RC]; break;             // �B�zAND���O                             
	  case OP_OR:  R[RA] = R[RB] | R[RC]; break;             // �B�zOR���O                              
	  case OP_XOR: R[RA] = R[RB] ^ R[RC]; break;             // �B�zXOR���O              
      case OP_LAND:R[RA] = R[RB] && R[RC]; break;            // �B�zLAND���O  
      case OP_LOR: R[RA] = R[RB] || R[RC]; break;            // �B�zLOR���O  
      case OP_EQ:  R[RA] = R[RB] == R[RC]; break;            // �B�zEQ���O  
      case OP_NE:  R[RA] = R[RB] != R[RC]; break;            // �B�zNE���O  
      case OP_GT:  R[RA] = R[RB] > R[RC]; break;             // �B�zGT���O  
      case OP_LT:  R[RA] = R[RB] < R[RC]; break;             // �B�zLT���O  
      case OP_GE:  R[RA] = R[RB] >= R[RC]; break;            // �B�zGE���O  
      case OP_LE:  R[RA] = R[RB] <= R[RC]; break;            // �B�zLE���O  
	  case OP_ROL: R[RA] = ROL(R[RB],C5); break;             // �B�zROL���O                             
	  case OP_ROR: R[RA] = ROR(R[RB],C5); break;             // �B�zROR���O                             
	  case OP_SHL: R[RA] = SHL(R[RB],C5); break;             // �B�zSHL���O                             
	  case OP_SHR: R[RA] = SHR(R[RB],C5); break;             // �B�zSHR���O                             
	  case OP_JEQ: if (Z==1) PC += C24; break;               // �B�zJEQ���O Z=1
	  case OP_JNE: if (Z==0) PC += C24; break;               // �B�zJNE���O Z=0 
	  case OP_JLT: if (N==1&&Z==0) PC += C24; break;         // �B�zJLT���O NZ=10 
	  case OP_JGT: if (N==0&&Z==0) PC += C24; break;         // �B�zJGT���O NZ=00
	  case OP_JLE:                                           // �B�zJLE���O NZ=10 or 01
		  if ((N==1&&Z==0)||(N==0&&Z==1)) PC+=C24; break;
	  case OP_JGE:                                           // �B�zJGE���O NZ=00 or 01
		  if ((N==0&&Z==0)||(N==0&&Z==1)) PC+=C24; break;
	  case OP_JMP: PC+=C24; break;                           // �B�zJMP���O                             
	  case OP_SWI: 
		  if (T==1 && vm->swiHandler != NULL) 
			  vm->swiHandler(vm); 
		  break;                   // �B�zSWI���O                             
	  case OP_CALL:LR=PC; PC+=C24; break;                    // �B�zJSUB���O                            
	  case OP_RET: if (LR<0) stop=TRUE; else PC=LR; break;   // �B�zRET���O                             
	  case OP_PUSH:StoreInt32(R[RA], M, SP); SP+=4; break;   // �B�zPUSH���O�A�`�N�G�쬰 SP-=4; �אּ SP+=4; 
	  case OP_POP: SP-=4; LoadInt32(R[RA], M, SP); break;    // �B�zPOP���O�A�`�N�G�쬰 SP+=4; �אּ SP-=4; 
	  case OP_PUSHB:StoreByte(R[RA], M, SP); SP++; break;    // �B�zPUSHB���O�A�`�N�G�쬰 SP--; �אּ SP++;  
	  case OP_POPB: SP--; LoadByte(R[RA], M, SP); break;     // �B�zPOPB���O �A�`�N�G�쬰 SP++; �אּ SP--;                           
	  default: printf("Error:invalid op (%02x) ", OP);                                                
		}     
		if (vm->isTrace)
			printf("PC=%08x LR=%08x IR=%08x SW=%08x R[%02d]=0x%08x=%d count=%d\n", tPC, LR, IR, SW, RA, R[RA], R[RA], vm->count);
		strToUpper(buffer);
	}                                                                                                   
}                                                                                       

void VmDump(Vm *vm) {                      // �L�X�Ҧ��Ȧs��                           
	printf("\n===CPU0 dump registers===\n");                                                            
	printf("IR =0x%08x=%d\n", IR, IR);             // �L�X IR                                                                           
	int i;                                                                   
	for (i=0; i<16; i++)                           // �L�X R0..R15  
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
	   if (vm->count % vm->irqTimes == 0) { // �C irqTimes �ӫ��O�o�ͤ@���ɶ����_ 
		  LR = PC;
		  PC = IrqAddress;
	   } 
    }
}

