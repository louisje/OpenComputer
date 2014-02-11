#ifndef OS_H
#define OS_H

#include "Cpu.h"

#define M0MemorySize 65536
#define Timer        ((int*) &(cpu0->m[65500]))
#define ResetAddress 0*4
#define UnexpAddress 1*4
#define SwiAddress   2*4
#define IrqAddress   3*4

void M0Interrupt(void *cpu0);
void M0Execute(char *objFile);

#endif
