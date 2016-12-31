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
    printf("语法分析结束\n");
    printf("是否进行优化?1:是,0:否");
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
