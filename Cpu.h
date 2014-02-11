#ifndef CPU_H
#define CPU_H

#include "Lib.h"
#include "OpTable.h"

typedef struct {
	BYTE *m;
	int mSize;
	int r[16], ir;
	BYTE op, ra, rb, rc, cc;
	int  c5, c12, c16, c24, caddr, raddr;
	BYTE n, z, i, t, mode;
	FuncPtr1 irqHandler, swiHandler;
	BYTE isTrace;
	int count;
} Cpu;

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

#define IR (cpu->ir)
#define PC (cpu->r[15])
#define LR (cpu->r[14])
#define SP (cpu->r[13])
#define SW (cpu->r[12])
#define M  (cpu->m)
#define RA (cpu->ra)
#define RB (cpu->rb)
#define RC (cpu->rc)
#define OP (cpu->op)
#define C5 (cpu->c5)
#define C12 (cpu->c12)
#define C16 (cpu->c16)
#define C24 (cpu->c24)
#define N (cpu->n)
#define Z (cpu->z)
#define C (cpu->c)
#define V (cpu->v)
#define I (cpu->i)
#define T (cpu->t)
#define MODE (cpu->mode)
#define CADDR (cpu->caddr)
#define RADDR (cpu->raddr)
#define R (cpu->r)

void execute(char *objFile, int memSize); // 虛擬機器的主要函數 
Cpu* CpuNew(char *objFile, int memSize);
void CpuFree(Cpu *cpu);
void CpuRun(Cpu *cpu);
void CpuDump(Cpu *cpu);

#endif
