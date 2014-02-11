#ifndef CPU_H
#define CPU_H

#include "Lib.h"
#include "OpTable.h"

typedef struct Vm {
	BYTE *m;
	int mSize;
	int r[16], ir;
	BYTE op, ra, rb, rc, cc;
	int  c5, c12, c16, c24, caddr, raddr;
	BYTE n, z, i, t, mode;
	void (*irqHandler)(struct Vm *vm);
	void (*swiHandler)(struct Vm *vm);
	BYTE isTrace;
	int count, irqTimes;
	OpTable *opTable;
} Vm;

#define bits(i, from, to) ((UINT32) i << (31-to) >> (31-to+from)) // 取得 from 到 to 之間的位元                   
#define ROR(i, k) (((UINT32)i>>k)|(bits(i,32-k, 31)<<(32-k)))// 向右旋轉k位元                                           
#define ROL(i, k) (((UINT32)i<<k)|(bits(i,0,k-1)<<(32-k)))   // 向左旋轉k位元                                             
#define SHR(i, k) ((UINT32)i>>k)                             // 向右移位k位元                
#define SHL(i, k) ((UINT32)i<<k)                             // 向左移位k位元                
#define bytesToInt32(p) (INT32)(p[0]<<24|p[1]<<16|p[2]<<8|p[3])// 4 byte轉 int
#define bytesToInt16(p) (INT16)(p[0]<<8|p[1])                // 2 byte轉 INT16               
#define int32ToBytes(i, bp) { bp[0]=i>>24; bp[1]=i>>16; bp[2]=i>>8; bp[3]=i;} // int轉為4 byte                                             
#define StoreInt32(i, m, addr) { BYTE *p=&m[addr]; int32ToBytes(i, p); } // i=m[addr…addr+3]                                         
#define LoadInt32(i, m, addr) { BYTE *p=&m[addr]; i=bytesToInt32(p); } // m[addr..addr+3]=i                                         
#define StoreByte(b, m, addr) { m[addr] = (BYTE) b; }        // m[addr]=b                                                 
#define LoadByte(b, m, addr) { b = m[addr]; }                // b=m[addr]                    

#define IR (vm->ir)
#define PC (vm->r[15])
#define LR (vm->r[14])
#define SP (vm->r[13])
#define SW (vm->r[12])
#define M  (vm->m)
#define RA (vm->ra)
#define RB (vm->rb)
#define RC (vm->rc)
#define OP (vm->op)
#define C5 (vm->c5)
#define C12 (vm->c12)
#define C16 (vm->c16)
#define C24 (vm->c24)
#define N (vm->n)
#define Z (vm->z)
#define C (vm->c)
#define V (vm->v)
#define I (vm->i)
#define T (vm->t)
#define MODE (vm->mode)
#define CADDR (vm->caddr)
#define RADDR (vm->raddr)
#define R (vm->r)

// ============ 位址設定  ===============
#define M0MemorySize 65536
#define Timer        ((int*) &(cpu0->m[65500]))
#define ResetAddress 0*4
#define UnexpAddress 1*4
#define SwiAddress   2*4
#define IrqAddress   3*4


void VmTest();
void execute(char *objFile, int irqTimes); // 虛擬機器的主要函數 
Vm* VmNew(char *objFile);
void VmFree(Vm *vm);
void VmIrqHandler(Vm *vm);
void VmSwiHandler(Vm *vm);
void VmRun(Vm *vm);
void VmDump(Vm *vm);

#endif
