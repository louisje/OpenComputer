#ifndef PCODE_H
#define PCODE_H

#include <stdio.h>
#include <string.h>
#include "Scanner.h"

void pcodeToAsm(FILE *asmFile, char* label, char* op, char* p1, char* p2, char* pTo);
void rewrite(FILE *asmFile, char* label, char* op, char* p1, char* p2, char* pTo);

#endif
