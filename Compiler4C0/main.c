#include<stdio.h>
#include<stdlib.h>
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
int main(){
    setInputOutput();
    ch=' ';
    printf("compile result:\n");
    getNextSym();
    program();
    printf("�﷨��������\n");
    printf("�Ƿ�����Ż�?1:��,0:��");
    scanf("%d",&is_optimize);
    if(is_optimize){
        optimize();
        opt_outputquat();
        opt_gen_asm();
    }else{
        outputquat();
        gen_asm();
    }
    finish_compile();
    return 0;
}
