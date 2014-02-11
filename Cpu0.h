#ifndef CPU0_H
#define CPU0_H

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
} Cpu0;

#define bits(i, from, to) ((UINT32) i << (31-to) >> (31-to+from)) // ���o from �� to �������줸                   
#define ROR(i, k) (((UINT32)i>>k)|(bits(i,32-k, 31)<<(32-k)))// �V�k����k�줸                                           
#define ROL(i, k) (((UINT32)i<<k)|(bits(i,0,k-1)<<(32-k)))   // �V������k�줸                                             
#define SHR(i, k) ((UINT32)i>>k)                             // �V�k����k�줸                
#define SHL(i, k) ((UINT32)i<<k)                             // �V������k�줸                
#define bytesToInt32(p) (INT32)(p[0]<<24|p[1]<<16|p[2]<<8|p[3])// 4 byte�� int
#define bytesToInt16(p) (INT16)(p[0]<<8|p[1])                // 2 byte�� INT16               
#define int32ToBytes(i, bp) { bp[0]=i>>24; bp[1]=i>>16; bp[2]=i>>8; bp[3]=i;} // int�ର4 byte                                             
#define StoreInt32(i, m, addr) { BYTE *p=&m[addr]; int32ToBytes(i, p); } // i=m[addr�Kaddr+3]                                         
#define LoadInt32(i, m, addr) { BYTE *p=&m[addr]; i=bytesToInt32(p); } // m[addr..addr+3]=i                                         
#define StoreByte(b, m, addr) { m[addr] = (BYTE) b; }        // m[addr]=b                                                 
#define LoadByte(b, m, addr) { b = m[addr]; }                // b=m[addr]                    

#define IR (cpu0->ir)
#define PC (cpu0->r[15])
#define LR (cpu0->r[14])
#define SP (cpu0->r[13])
#define SW (cpu0->r[12])
#define M  (cpu0->m)
#define RA (cpu0->ra)
#define RB (cpu0->rb)
#define RC (cpu0->rc)
#define OP (cpu0->op)
#define C5 (cpu0->c5)
#define C12 (cpu0->c12)
#define C16 (cpu0->c16)
#define C24 (cpu0->c24)
#define N (cpu0->n)
#define Z (cpu0->z)
#define C (cpu0->c)
#define V (cpu0->v)
#define I (cpu0->i)
#define T (cpu0->t)
#define MODE (cpu0->mode)
#define CADDR (cpu0->caddr)
#define RADDR (cpu0->raddr)
#define R (cpu0->r)
                                                                                          
void execute(char *objFile, int memSize); // �����������D�n��� 
Cpu0* Cpu0New(char *objFile, int memSize);
void Cpu0Free(Cpu0 *cpu0);
void Cpu0Run(Cpu0 *cpu0);
void Cpu0Dump(Cpu0 *cpu0);

#endif
