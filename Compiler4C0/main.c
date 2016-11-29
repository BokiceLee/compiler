#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<ctype.h>
#include "Global.h"
#include "Error.h"
#include "GenAsm.h"
#include "LexicalAnalysis.h"
#include "Optimization.h"
#include "SymbolManagement.h"
#include "GenQuaternary.h"
#include "GrammarAnalysis.h"

int main()
{
    setInputOutput();
    ch=' ';
    printf("compile result:\n");
    getNextSym();
    program();
    outputquat();
    gen_asm();
    finish_compile();
    return 0;
}
