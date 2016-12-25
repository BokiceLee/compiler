#include "GenQuaternary.h"
#include "Optimization.h"
#ifndef GENASM_H_INCLUDED
#define GENASM_H_INCLUDED
#define INSTRU_LEN 40
#define TMP_VAR_NUM 1000
#define R_TMP_VAR_NUM 1000
#define LOCAL_REG_NUM 3
int qx=0;
int tmp_var_flag[TMP_VAR_NUM];
void gen_asm();
void gen_asm_head();
int gen_asm_data(int is_glocal);
int gen_asm_local_data();
void gen_instruction(struct quat_struct quat,int var_flag,int is_main);
void gen_asm_code();
void reconvert_name(char res[],char src[]);

void gen_asm(){
    int i=0;
    fasm=fopen(fasm_name,"w");
    gen_asm_head();
    for(i=0;i<TMP_VAR_NUM;i++){
        tmp_var_flag[i]=0;
    }
    gen_asm_data(1);
    gen_asm_code();
}
void gen_asm_head(){
    int i;
    fprintf(fasm,".386\n");
    fprintf(fasm,".model flat,stdcall\n");
    fprintf(fasm,"option casemap : none\n");
    fprintf(fasm,"include C:\\masm32\\include\\windows.inc\n");
    fprintf(fasm,"include C:\\masm32\\include\\kernel32.inc\n");
    fprintf(fasm,"includelib C:\\masm32\\lib\\kernel32.lib\n");
    fprintf(fasm,"include C:\\masm32\\include\\user32.inc\n");
    fprintf(fasm,"includelib C:\\masm32\\lib\\user32.lib\n");
    fprintf(fasm,"include C:\\masm32\\include\\msvcrt.inc\n");
    fprintf(fasm,"includelib C:\\masm32\\lib\\msvcrt.lib\n");
    fprintf(fasm,".data\n");
    for(i=0;i<=string_table_index;i++){
        if(string_tab[i][0]=='\0'){
            fprintf(fasm,"\t_STR_%03d\tBYTE\t0\n",i);
        }else{
            fprintf(fasm,"\t_STR_%03d\tDB\t\"%s\",0\n",i,string_tab[i]);
        }
    }
    fprintf(fasm,"\t_fmt_out_s\tDB\t'%%s',0\n");
    fprintf(fasm,"\t_fmt_out_c\tDB\t'%%c',0\n");
    fprintf(fasm,"\t_fmt_out_i\tDB\t'%%d',0\n");
    fprintf(fasm,"\t_fmt_in_c\tDB\t'%%c',0\n");
    fprintf(fasm,"\t_fmt_in_i\tDB\t'%%d',0\n");//读数字后面有空格隔开
}
int gen_asm_data(int is_global){
    char* s[]={"\tlocal\t%s\n","\t_%s\tDD\t0\n"};
    char* dup_s[]={"\tlocal\t%s[%d]\n","\t_%s\tDD\t%d\tDUP(0)\n"};
    int lc_data_num=0;
    while(quat_table[qx].op==op_var_dcl||quat_table[qx].op==op_array_dcl){
        if(quat_table[qx].op==op_var_dcl){
            fprintf(fasm,s[is_global],quat_table[qx].dest);
            lc_data_num++;
        }else{
            fprintf(fasm,dup_s[is_global],quat_table[qx].dest,atoi(quat_table[qx].src1+1));
            lc_data_num++;
        }
        qx++;
    }
    return lc_data_num;
}
int gen_asm_local_data(){
    char* s="\tlocal\t@_tmp_var_%03d\t\n";
    int i;
    int lc_data_num;
    lc_data_num=gen_asm_data(0);
    for(i=qx;quat_table[i].op!=op_efunc && quat_table[i].op!=op_emain;i++){
        if(quat_table[i].dest[0]=='&'){
            if(tmp_var_flag[atoi(quat_table[i].dest+1)]==0){
                fprintf(fasm,s,atoi(1+quat_table[i].dest));
                tmp_var_flag[atoi(quat_table[i].dest+1)]=1;
                lc_data_num++;
            }
        }
    }
    return lc_data_num;
}
void gen_instruction(struct quat_struct quat,int var_flag,int is_main){
    char dest[VAR_LEN];
    char src1[VAR_LEN];
    char src2[VAR_LEN];
    char* fmt0="\t%s\n";
    char* fmt1="\t%s\t%s\n";
    char* fmt2="\t%s\t%s,%s\n";
    reconvert_name(dest,quat.dest);
    reconvert_name(src1,quat.src1);
    reconvert_name(src2,quat.src2);
    switch(quat.op){
    case op_add:
        fprintf(fasm,fmt2,"mov","eax",src1);
        fprintf(fasm,fmt2,"add","eax",src2);
        fprintf(fasm,fmt2,"mov",dest,"eax");
        break;
    case op_sub:
        fprintf(fasm,fmt2,"mov","eax",src1);
        fprintf(fasm,fmt2,"sub","eax",src2);
        fprintf(fasm,fmt2,"mov",dest,"eax");
        break;
    case op_mul:
        fprintf(fasm,fmt2,"mov","eax",src1);
        fprintf(fasm,fmt2,"imul","eax",src2);
        fprintf(fasm,fmt2,"mov",dest,"eax");
        break;
    case op_idiv:
        fprintf(fasm,fmt2,"mov","eax",src1);
        fprintf(fasm,fmt0,"cdq");
        if(quat_table[qx].src2[0]=='$'||quat_table[qx].src2[0]=='^'){
            fprintf(fasm,fmt1,"push","ebx");
            fprintf(fasm,fmt2,"mov","ebx",src2);
            fprintf(fasm,fmt1,"idiv","ebx");
            fprintf(fasm,fmt1,"pop","ebx");
        }else{
            fprintf(fasm,fmt1,"idiv",src2);
        }
        fprintf(fasm,fmt2,"mov",dest,"eax");
        break;
    case op_mov:
        if(quat_table[qx].src1[0]=='$'&&quat_table[qx].dest[0]!='^'){
            fprintf(fasm,fmt2,"mov",dest,src1);
        }else{
            fprintf(fasm,fmt2,"mov","eax",src1);
            fprintf(fasm,fmt2,"mov",dest,"eax");
        }
        break;
    case op_arr_assign:
        fprintf(fasm,"\t%s\t%s,[%s]\n","lea","eax",dest);
        fprintf(fasm,fmt2,"mov","ebx",src1);
        fprintf(fasm,fmt2,"mov","ecx",src2);
        fprintf(fasm,"\t%s\t[%s+4*%s],%s\n","mov","eax","ebx","ecx");
//        fprintf(fasm,fmt2,"mov","dest",src1);
//        fprintf(fasm,fmt2,"imul","ebx","4");
//        fprintf(fasm,"\t%s\tdword ptr [%s],%s","mov","eax",src2);
        break;
    case op_ret_void:
        fprintf(fasm,"\t%s\t%s\n","pop","esi");
        fprintf(fasm,"\t%s\t%s\n","pop","edi");
        fprintf(fasm,"\t%s\t%s\n","pop","ebx");//被调用者恢复现场
        if(var_flag==0){
            fprintf(fasm,"\t%s\t%s,%s\n","mov","esp","ebp");
            fprintf(fasm,"\t%s\t%s\n","pop","ebp");
        }
        if(is_main){
            fprintf(fasm,"\tinvoke ExitProcess,0\n");
        }else{
            fprintf(fasm,fmt0,"ret");
        }
        break;
    case op_ret_value:
        fprintf(fasm,"\t%s\t%s\n","pop","esi");
        fprintf(fasm,"\t%s\t%s\n","pop","edi");
        fprintf(fasm,"\t%s\t%s\n","pop","ebx");//被调用者恢复现场
        fprintf(fasm,fmt2,"mov","eax",dest);
        if(var_flag==0){
            fprintf(fasm,"\t%s\t%s,%s\n","mov","esp","ebp");
            fprintf(fasm,"\t%s\t%s\n","pop","ebp");
        }
        fprintf(fasm,fmt0,"ret");
        break;
    case op_beq:
        fprintf(fasm,fmt2,"mov","eax",src1);
        fprintf(fasm,fmt2,"cmp","eax",src2);
        fprintf(fasm,fmt1,"je",dest);
        break;
    case op_bne:
        fprintf(fasm,fmt2,"mov","eax",src1);
        fprintf(fasm,fmt2,"cmp","eax",src2);
        fprintf(fasm,fmt1,"jne",dest);
        break;
    case op_ble:
        fprintf(fasm,fmt2,"mov","eax",src1);
        fprintf(fasm,fmt2,"cmp","eax",src2);
        fprintf(fasm,fmt1,"jle",dest);
        break;
    case op_bls:
        fprintf(fasm,fmt2,"mov","eax",src1);
        fprintf(fasm,fmt2,"cmp","eax",src2);
        fprintf(fasm,fmt1,"jl",dest);
        break;
    case op_bgt:
        fprintf(fasm,fmt2,"mov","eax",src1);
        fprintf(fasm,fmt2,"cmp","eax",src2);
        fprintf(fasm,fmt1,"jg",dest);
        break;
    case op_bge:
        fprintf(fasm,fmt2,"mov","eax",src1);
        fprintf(fasm,fmt2,"cmp","eax",src2);
        fprintf(fasm,fmt1,"jge",dest);
        break;
    case op_jump:
        fprintf(fasm,fmt1,"jmp",dest);
        break;
    case op_prints:
        if(dest[0]=='@'){
            fprintf(fasm,"\t%s\t%s,%s %s,%s %s\n","invoke","crt_printf","OFFSET","_fmt_out_s","OFFSET",dest);
        }else{
            fprintf(fasm,"\t%s\t%s,%s %s,%s %s\n","invoke","crt_printf","OFFSET","_fmt_out_s","OFFSET",dest);
        }
        break;
    case op_printi:
        if(quat_table[qx].dest[0]=='^'){
            fprintf(fasm,fmt1,"push","eax");
            fprintf(fasm,fmt2,"mov","eax",dest);
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_printf","OFFSET","_fmt_out_i","eax");
            fprintf(fasm,fmt1,"pop","eax");
        }else{
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_printf","OFFSET","_fmt_out_i",dest);
        }
        break;
    case op_printc:
        if(quat_table[qx].dest[0]=='^'){
            fprintf(fasm,fmt1,"push","eax");
            fprintf(fasm,fmt2,"mov","eax",dest);
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_printf","OFFSET","_fmt_out_c","eax");
            fprintf(fasm,fmt1,"pop","eax");
        }else{
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_printf","OFFSET","_fmt_out_c",dest);
        }
        break;
    case op_scanfc:
        if(dest[0]=='@'){
            fprintf(fasm,"\t%s\t%s,%s %s,%s %s\n","invoke","crt_scanf","OFFSET","_fmt_in_c","ADDR",dest);
        }else{
            fprintf(fasm,"\t%s\t%s,%s %s,%s %s\n","invoke","crt_scanf","OFFSET","_fmt_in_c","OFFSET",dest);
        }
        break;
    case op_scanfi:
        if(dest[0]=='@'){
            fprintf(fasm,"\t%s\t%s,%s %s,%s %s\n","invoke","crt_scanf","OFFSET","_fmt_in_i","ADDR",dest);
        }else{
            fprintf(fasm,"\t%s\t%s,%s %s,%s %s\n","invoke","crt_scanf","OFFSET","_fmt_in_i","OFFSET",dest);
        }
        break;
    case op_set_label:
        fprintf(fasm,"%s:\n",dest);
        break;
    case op_load_ret:
        fprintf(fasm,fmt2,"mov",dest,"eax");
        break;
    case op_arr_get:
        fprintf(fasm,"\t%s\t%s,[%s]\n","lea","eax",src1);
        fprintf(fasm,fmt2,"mov","ebx",src2);
        fprintf(fasm,"\t%s\t%s,[%s+4*%s]\n","mov","eax","eax","ebx");
        fprintf(fasm,fmt2,"mov",dest,"eax");
//        fprintf(fasm,fmt2,"imul","eax","4");
//        fprintf(fasm,fmt2,"add","eax","ebx");
//        fprintf(fasm,"\t%s\t%s,dword ptr [%s]","mov",dest,"eax");
        break;
    default:
        fprintf(fasm,"不合法指令\n");
    }
}
void gen_asm_code(){
    int para_count=0;
    char ce_name[VAR_LEN];
    int var_flag=0;
    int is_main;
    fprintf(fasm,"%s",".code\n");
    while(quat_table[qx].op==op_func||quat_table[qx].op==op_main){
        if(quat_table[qx].op==op_main){
            fprintf(fasm,"main PROC\n");
            qx++;
            is_main=1;
        }else{
            reconvert_name(ce_name,quat_table[qx].dest);
            fprintf(fasm,"%s PROC\n",ce_name);
            qx++;
            is_main=0;
        }
        if(gen_asm_local_data()>0){
            var_flag=1;
        }else{
            fprintf(fasm,"\t%s\t%s\n","push","ebp");
            fprintf(fasm,"\t%s\t%s,%s\n","mov","ebp","esp");
            var_flag=0;
        }
        fprintf(fasm,"\t%s\t%s\n","push","ebx");
        fprintf(fasm,"\t%s\t%s\n","push","edi");
        fprintf(fasm,"\t%s\t%s\n","push","esi");//被调用者保护现场
        while(quat_table[qx].op!=op_efunc&&quat_table[qx].op!=op_emain){
            if(quat_table[qx].op!=op_para && quat_table[qx].op!=op_call){
                gen_instruction(quat_table[qx],var_flag,is_main);
                qx++;
            }else{
//                fprintf(fasm,"\t%s\t%s\n","push","eax");
                fprintf(fasm,"\t%s\t%s\n","push","ecx");
                fprintf(fasm,"\t%s\t%s\n","push","edx");//调用者保护现场
                para_count=0;
                while(quat_table[qx].op==op_para){//参数压栈
                    reconvert_name(ce_name,quat_table[qx].dest);
                    fprintf(fasm,"\t%s\t%s\n","push",ce_name);
                    para_count++;
                    qx++;
                }//可能无参
                if(quat_table[qx].op==op_call){//传参后面正常来说是函数调用
                    reconvert_name(ce_name,quat_table[qx].dest);
                    fprintf(fasm,"\t%s\t%s\n","call",ce_name);
                    qx++;
                    if(para_count){//弹参数
                        fprintf(fasm,"\t%s\t%s,%d\n","add","esp",4*para_count);
                    }
                }
                fprintf(fasm,"\t%s\t%s\n","pop","edx");
                fprintf(fasm,"\t%s\t%s\n","pop","ecx");
//                fprintf(fasm,"\t%s\t%s\n","pop","eax");//调用者恢复现场
            }
        }
        if(quat_table[qx].op==op_emain){
            fprintf(fasm,"main ENDP\n");
            fprintf(fasm,"END main\n");
            qx++;
        }else{
            reconvert_name(ce_name,quat_table[qx].dest);
            fprintf(fasm,"%s ENDP\n",ce_name);
            qx++;
        }
        if(qx>=quat_index){
            break;
        }
    }
}
void reconvert_name(char res[],char src[]){
    switch(src[0]){
    case '&':
        sprintf(res,"@_tmp_var_%s",src+1);
        return;
    case '*':
        sprintf(res,"_STR_%03d",atoi(src+1));
        return;
    case '$':
        sprintf(res,"%d",atoi(src+1));
        return;
    case '#':
        sprintf(res,"%d",src[1]);
        return;
    case '^':
        sprintf(res,"[ebp+%d]",4+4*atoi(src+1));
        return;
    default:
        if(src[0]!='@'){
            sprintf(res,"_%s",src);
        }else{
            sprintf(res,"%s",src);
        }
        return;
    }
}


int r_tmp_var_flag[R_TMP_VAR_NUM];
//申请临时寄存器


void opt_gen_asm(){
    int i=0;
    fasm=fopen(fasm_name,"w");
    opt_gen_asm_head();
    for(i=0;i<TMP_VAR_NUM;i++){
        tmp_var_flag[i]=0;
    }
    for(i=0;i<R_TMP_VAR_NUM;i++){
        r_tmp_var_flag[i]=0;
    }
    opt_gen_asm_data(1,data);
    opt_gen_asm_code();
}
void opt_gen_asm_head(){
    gen_asm_head();
}
int opt_gen_asm_data(int is_global,struct opt_quat_struct* p){
    char* s[]={"\tlocal\t%s\n","\t_%s\tDD\t0\n"};
    char* dup_s[]={"\tlocal\t%s[%d]\n","\t_%s\tDD\t%d\tDUP(0)\n"};
    int lc_data_num=0;
    while(1){
        if(p==NULL||(p->op!=op_var_dcl&&p->op!=op_array_dcl)){
           break;
        }
        if(p->op==op_var_dcl){
            fprintf(fasm,s[is_global],p->dest);
            lc_data_num++;
        }else{
            fprintf(fasm,dup_s[is_global],p->dest,atoi(p->src1+1));
            lc_data_num++;
        }
        p=p->next;
    }
    return lc_data_num;
}
int opt_gen_asm_local_data(int funcx){
    char* s="\tlocal\t@_tmp_var_%03d\t\n";
    char* r_s="\tlocal\t@r_tmp_var_%03d\t\n";
    int i;
    int lc_data_num;
    struct opt_quat_struct* p;
    p=functs[funcx].blocks[0].block_begin;
    p=p->next;//第一个基本块的第二个指令开始
    lc_data_num=opt_gen_asm_data(0,p);
    for(i=0;i<functs[funcx].block_num;i++){
        p=functs[funcx].blocks[i].block_begin;
        while(p!=NULL){
            if(p->dest[0]=='&'){
                if(p->dest[1]=='r'){
                    if(r_tmp_var_flag[atoi(p->dest+2)]==0){
                        fprintf(fasm,r_s,atoi(p->dest+2));
                        r_tmp_var_flag[atoi(p->dest+2)]=1;
                        lc_data_num++;
                    }
                }else{
                    if(tmp_var_flag[atoi(p->dest+1)]==0){
                        fprintf(fasm,s,atoi(1+p->dest));
                        tmp_var_flag[atoi(p->dest+1)]=1;
                        lc_data_num++;
                    }
                }
            }
            p=p->next;
        }
    }
    return lc_data_num;
}
char global_reg_name[3][5]={"ebx","edi","esi"};
int var_is_para(char vvv[]){
    return vvv[0]=='[';
}
int var_is_mem(char vvv[]){
    return vvv[0]=='_'||vvv[0]=='@';
}
int var_is_int(char vvv[]){
    int i;
    if(vvv[0]=='\0'){
        return 0;
    }
    for(i=0;vvv[i]!='\0';i++){
        if(isdigit(vvv[i])==0){
            return 0;
        }
    }
    return 1;
}
int var_is_reg(char vvv[]){
    int i;
    for(i=0;i<3;i++){
        if(strcmp(global_reg_name[i],vvv)==0){
            return 1;
        }
    }
    return 0;
}

void opt_sub_reconvert(char res[],char src[]){
    switch(src[0]){
    case '&'://临时变量
        if(src[1]=='r'){
            sprintf(res,"@r_tmp_var_%s",src+2);
        }else{
            sprintf(res,"@_tmp_var_%s",src+1);
        }
        return;
    case '*'://字符串
        sprintf(res,"_STR_%03d",atoi(src+1));
        return;
    case '$'://整数
        sprintf(res,"%d",atoi(src+1));
        return;
    case '#'://字符
        sprintf(res,"%d",src[1]);
        return;
    case '^'://参数
        sprintf(res,"[ebp+%d]",4+4*atoi(src+1));
        return;
    default:
        if(var_is_reg(src)){
            sprintf(res,"%s",src);
        }else if(src[0]!='@'){//全局变量
            sprintf(res,"_%s",src);
        }else{//局部变量
            sprintf(res,"%s",src);
        }
        return;
    }
}
void opt_reconvert_name(int funcx,struct opt_quat_struct* p,char dest[],char src1[],char src2[]){
    int i;
    char lc_dest[VAR_LEN];
    char lc_src1[VAR_LEN];
    char lc_src2[VAR_LEN];
    int dest_flag=0;
    int src1_flag=0;
    int src2_flag=0;
    for(i=0;i<functs[funcx].var_reg_len;i++){
        if(strcmp(p->dest,functs[funcx].var_reg[i].var)==0){
            strcpy(lc_dest,global_reg_name[functs[funcx].var_reg[i].reg]);
            dest_flag=1;
        }
        if(strcmp(p->src1,functs[funcx].var_reg[i].var)==0){
            strcpy(lc_src1,global_reg_name[functs[funcx].var_reg[i].reg]);
            src1_flag=1;
        }
        if(strcmp(p->src2,functs[funcx].var_reg[i].var)==0){
            strcpy(lc_src2,global_reg_name[functs[funcx].var_reg[i].reg]);
            src2_flag=1;
        }
    }
    if(dest_flag==1){
        opt_sub_reconvert(dest,lc_dest);
    }else{
        opt_sub_reconvert(dest,p->dest);
    }
    if(src1_flag==1){
        opt_sub_reconvert(src1,lc_src1);
    }else{
        opt_sub_reconvert(src1,p->src1);
    }
    if(src2_flag==1){
        opt_sub_reconvert(src2,lc_src2);
    }else{
        opt_sub_reconvert(src2,p->src2);
    }
}

void opt_gen_instruction(int funcx,struct opt_quat_struct* p,int var_flag,int is_main){
    char dest[VAR_LEN];
    char src1[VAR_LEN];
    char src2[VAR_LEN];
    char* fmt0="\t%s\n";
    char* fmt1="\t%s\t%s\n";
    char* fmt2="\t%s\t%s,%s\n";
    char ce_name[VAR_LEN];
    opt_reconvert_name(funcx,p,dest,src1,src2);//能转换的全转换
    switch(p->op){
    case op_add:
        if(strcmp(dest,src1)==0){
            if(var_is_para(dest)){
                fprintf(fasm,fmt2,"mov","eax",src1);
                fprintf(fasm,fmt2,"add","eax",src2);
                fprintf(fasm,fmt2,"mov",dest,"eax");
            }else if(var_is_reg(dest)){
                fprintf(fasm,fmt2,"add",dest,src2);
            }else if(var_is_mem(dest)){
                if(var_is_mem(src2)||var_is_reg(src2)){
                    fprintf(fasm,fmt2,"add",dest,src2);
                }else{
                    fprintf(fasm,fmt2,"mov","eax",src1);
                    fprintf(fasm,fmt2,"add","eax",src2);
                    fprintf(fasm,fmt2,"mov",dest,"eax");
                }
            }else{
                printf("未考虑\n");
            }
        }else{
            fprintf(fasm,fmt2,"mov","eax",src1);
            fprintf(fasm,fmt2,"add","eax",src2);
            fprintf(fasm,fmt2,"mov",dest,"eax");
        }
        break;
    case op_sub:
        if(strcmp(dest,src1)==0){
            if(var_is_para(dest)){
                fprintf(fasm,fmt2,"mov","eax",src1);
                fprintf(fasm,fmt2,"sub","eax",src2);
                fprintf(fasm,fmt2,"mov",dest,"eax");
            }else if(var_is_reg(dest)){
                fprintf(fasm,fmt2,"sub",dest,src2);
            }else if(var_is_mem(dest)){
                if(var_is_mem(src2)||var_is_reg(src2)){
                    fprintf(fasm,fmt2,"sub",dest,src2);
                }else{
                    fprintf(fasm,fmt2,"mov","eax",src1);
                    fprintf(fasm,fmt2,"sub","eax",src2);
                    fprintf(fasm,fmt2,"mov",dest,"eax");
                }
            }else{
                printf("未考虑\n");
            }
        }else{
            fprintf(fasm,fmt2,"mov","eax",src1);
            fprintf(fasm,fmt2,"sub","eax",src2);
            fprintf(fasm,fmt2,"mov",dest,"eax");
        }
        break;
    case op_mul:
        if(strcmp(dest,src1)==0){
            if(var_is_para(dest)){
                fprintf(fasm,fmt2,"mov","eax",src1);
                fprintf(fasm,fmt2,"imul","eax",src2);
                fprintf(fasm,fmt2,"mov",dest,"eax");
            }else if(var_is_reg(dest)){
                fprintf(fasm,fmt2,"add",dest,src2);
            }else if(var_is_mem(dest)){
                if(var_is_mem(src2)||var_is_reg(src2)){
                    fprintf(fasm,fmt2,"imul",dest,src2);
                }else{
                    fprintf(fasm,fmt2,"mov","eax",src1);
                    fprintf(fasm,fmt2,"imul","eax",src2);
                    fprintf(fasm,fmt2,"mov",dest,"eax");
                }
            }else{
                printf("未考虑\n");
            }
        }else{
            fprintf(fasm,fmt2,"mov","eax",src1);
            fprintf(fasm,fmt2,"imul","eax",src2);
            fprintf(fasm,fmt2,"mov",dest,"eax");
        }
        break;
    case op_idiv://被除数只能是mem或ebx
        fprintf(fasm,fmt2,"mov","eax",src1);
        fprintf(fasm,fmt0,"cdq");
        if(var_is_para(src2)||var_is_int(src2)){
            fprintf(fasm,fmt2,"mov","ecx",src2);
            fprintf(fasm,fmt1,"idiv","ecx");
        }else if(var_is_mem(src2)||var_is_reg(src2)){
            fprintf(fasm,fmt1,"idiv",src2);
        }
        fprintf(fasm,fmt2,"mov",dest,"eax");
        break;
    case op_mov:
        if(
           ( var_is_mem(dest) || var_is_para(dest) )
           &&
           ( var_is_mem(src1) || var_is_para(src1) ) ){
           fprintf(fasm,fmt2,"mov","eax",src1);
           fprintf(fasm,fmt2,"mov",dest,"eax");
        }else{
            fprintf(fasm,fmt2,"mov",dest,src1);
        }
        break;
    case op_arr_assign:
        fprintf(fasm,"\t%s\t%s,[%s]\n","lea","eax",dest);
        if(var_is_reg(src2)){
            if(var_is_reg(src1)||var_is_int(src1)||var_is_mem(src1)){
                fprintf(fasm,"\t%s\t[%s+4*%s],%s\n","mov","eax",src1,src2);
            }else{
                fprintf(fasm,fmt2,"mov","ecx",src1);
                fprintf(fasm,"\t%s\t[%s+4*%s],%s\n","mov","eax","ecx",src2);
            }
        }else{
            fprintf(fasm,fmt2,"mov","ecx",src1);
            fprintf(fasm,fmt2,"mov","edx",src2);
            fprintf(fasm,"\t%s\t[%s+4*%s],%s\n","mov","eax","ecx","edx");
        }
        break;
    case op_ret_void:
        fprintf(fasm,"\t%s\t%s\n","pop","esi");
        fprintf(fasm,"\t%s\t%s\n","pop","edi");
        fprintf(fasm,"\t%s\t%s\n","pop","ebx");//被调用者恢复现场
        if(var_flag==0){
            fprintf(fasm,"\t%s\t%s,%s\n","mov","esp","ebp");
            fprintf(fasm,"\t%s\t%s\n","pop","ebp");
        }
        if(is_main){
            fprintf(fasm,"\tinvoke ExitProcess,0\n");
        }else{
            fprintf(fasm,fmt0,"ret");
        }
        break;
    case op_ret_value:
        fprintf(fasm,"\t%s\t%s\n","pop","esi");
        fprintf(fasm,"\t%s\t%s\n","pop","edi");
        fprintf(fasm,"\t%s\t%s\n","pop","ebx");//被调用者恢复现场
        fprintf(fasm,fmt2,"mov","eax",dest);
        if(var_flag==0){
            fprintf(fasm,"\t%s\t%s,%s\n","mov","esp","ebp");
            fprintf(fasm,"\t%s\t%s\n","pop","ebp");
        }
        fprintf(fasm,fmt0,"ret");
        break;
    case op_beq:
        if((var_is_mem(src1)||var_is_para(src1))&&(var_is_mem(src2)||var_is_para(src2))){
            fprintf(fasm,fmt2,"mov","eax",src1);
            fprintf(fasm,fmt2,"cmp","eax",src2);
            fprintf(fasm,fmt1,"je",dest);
        }else if(var_is_int(src1)&&var_is_int(src2)){
            fprintf(fasm,fmt2,"mov","eax",src1);
            fprintf(fasm,fmt2,"cmp","eax",src2);
            fprintf(fasm,fmt1,"je",dest);
        }else{
            fprintf(fasm,fmt2,"cmp",src1,src2);
            fprintf(fasm,fmt1,"je",dest);
        }
        break;
    case op_bne:
        if((var_is_mem(src1)||var_is_para(src1))&&(var_is_mem(src2)||var_is_para(src2))){
            fprintf(fasm,fmt2,"mov","eax",src1);
            fprintf(fasm,fmt2,"cmp","eax",src2);
            fprintf(fasm,fmt1,"jne",dest);
        }else if(var_is_int(src1)&&var_is_int(src2)){
            fprintf(fasm,fmt2,"mov","eax",src1);
            fprintf(fasm,fmt2,"cmp","eax",src2);
            fprintf(fasm,fmt1,"jne",dest);
        }else{
            fprintf(fasm,fmt2,"cmp",src1,src2);
            fprintf(fasm,fmt1,"jne",dest);
        }
        break;
    case op_ble:
        if((var_is_mem(src1)||var_is_para(src1))&&(var_is_mem(src2)||var_is_para(src2))){
            fprintf(fasm,fmt2,"mov","eax",src1);
            fprintf(fasm,fmt2,"cmp","eax",src2);
            fprintf(fasm,fmt1,"jle",dest);
        }else if(var_is_int(src1)&&var_is_int(src2)){
            fprintf(fasm,fmt2,"mov","eax",src1);
            fprintf(fasm,fmt2,"cmp","eax",src2);
            fprintf(fasm,fmt1,"jle",dest);
        }else{
            fprintf(fasm,fmt2,"cmp",src1,src2);
            fprintf(fasm,fmt1,"jle",dest);
        }
        break;
    case op_bls:
        if((var_is_mem(src1)||var_is_para(src1))&&(var_is_mem(src2)||var_is_para(src2))){
            fprintf(fasm,fmt2,"mov","eax",src1);
            fprintf(fasm,fmt2,"cmp","eax",src2);
            fprintf(fasm,fmt1,"jl",dest);
        }else if(var_is_int(src1)&&var_is_int(src2)){
            fprintf(fasm,fmt2,"mov","eax",src1);
            fprintf(fasm,fmt2,"cmp","eax",src2);
            fprintf(fasm,fmt1,"jl",dest);
        }else{
            fprintf(fasm,fmt2,"cmp",src1,src2);
            fprintf(fasm,fmt1,"jl",dest);
        }
        break;
    case op_bgt:
        if((var_is_mem(src1)||var_is_para(src1))&&(var_is_mem(src2)||var_is_para(src2))){
            fprintf(fasm,fmt2,"mov","eax",src1);
            fprintf(fasm,fmt2,"cmp","eax",src2);
            fprintf(fasm,fmt1,"jg",dest);
        }else if(var_is_int(src1)&&var_is_int(src2)){
            fprintf(fasm,fmt2,"mov","eax",src1);
            fprintf(fasm,fmt2,"cmp","eax",src2);
            fprintf(fasm,fmt1,"jg",dest);
        }else{
            fprintf(fasm,fmt2,"cmp",src1,src2);
            fprintf(fasm,fmt1,"jg",dest);
        }
        break;
    case op_bge:
        if((var_is_mem(src1)||var_is_para(src1))&&(var_is_mem(src2)||var_is_para(src2))){
            fprintf(fasm,fmt2,"mov","eax",src1);
            fprintf(fasm,fmt2,"cmp","eax",src2);
            fprintf(fasm,fmt1,"jge",dest);
        }else if(var_is_int(src1)&&var_is_int(src2)){
            fprintf(fasm,fmt2,"mov","eax",src1);
            fprintf(fasm,fmt2,"cmp","eax",src2);
            fprintf(fasm,fmt1,"jge",dest);
        }else{
            fprintf(fasm,fmt2,"cmp",src1,src2);
            fprintf(fasm,fmt1,"jge",dest);
        }
        break;
    case op_jump:
        fprintf(fasm,fmt1,"jmp",dest);
        break;
    case op_prints:
        if(dest[0]=='@'){
            fprintf(fasm,"\t%s\t%s,%s %s,%s %s\n","invoke","crt_printf","OFFSET","_fmt_out_s","OFFSET",dest);
        }else{
            fprintf(fasm,"\t%s\t%s,%s %s,%s %s\n","invoke","crt_printf","OFFSET","_fmt_out_s","OFFSET",dest);
        }
        break;
    case op_printi:
        if(var_is_para(dest)){
            fprintf(fasm,fmt2,"mov","eax",dest);
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_printf","OFFSET","_fmt_out_i","eax");
        }else{
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_printf","OFFSET","_fmt_out_i",dest);
        }
        break;
    case op_printc:
        if(var_is_para(dest)){
            fprintf(fasm,fmt2,"mov","eax",dest);
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_printf","OFFSET","_fmt_out_c","eax");
        }else{
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_printf","OFFSET","_fmt_out_c",dest);
        }
        break;
    case op_scanfc:
        if(var_is_reg(dest)){//获取寄存器对应的变量名称
            reconvert_name(ce_name,p->dest);
            fprintf(fasm,"\t%s\t%s,%s %s,%s %s\n","invoke","crt_scanf","OFFSET","_fmt_in_c","ADDR",ce_name);
            fprintf(fasm,fmt2,"mov",dest,ce_name);
        }else if(dest[0]=='@'){
            fprintf(fasm,"\t%s\t%s,%s %s,%s %s\n","invoke","crt_scanf","OFFSET","_fmt_in_c","ADDR",dest);
        }else if(dest[0]=='_'){
            fprintf(fasm,"\t%s\t%s,%s %s,%s %s\n","invoke","crt_scanf","OFFSET","_fmt_in_c","OFFSET",dest);
        }else{
            printf("未考虑");
        }
        break;
    case op_scanfi:
        if(var_is_reg(dest)){//获取寄存器对应的变量名称
            reconvert_name(ce_name,p->dest);
            fprintf(fasm,"\t%s\t%s,%s %s,%s %s\n","invoke","crt_scanf","OFFSET","_fmt_in_i","ADDR",ce_name);
            fprintf(fasm,fmt2,"mov",dest,ce_name);
        }else if(dest[0]=='@'){
            fprintf(fasm,"\t%s\t%s,%s %s,%s %s\n","invoke","crt_scanf","OFFSET","_fmt_in_i","ADDR",dest);
        }else if(dest[0]=='_'){
            fprintf(fasm,"\t%s\t%s,%s %s,%s %s\n","invoke","crt_scanf","OFFSET","_fmt_in_i","OFFSET",dest);
        }else{
            printf("未考虑");
        }
        break;
    case op_set_label:
        fprintf(fasm,"%s:\n",dest);
        break;
    case op_load_ret:
        fprintf(fasm,fmt2,"mov",dest,"eax");
        break;
    case op_arr_get:
        fprintf(fasm,"\t%s\t%s,[%s]\n","lea","eax",src1);
        if(var_is_reg(src2)){
            if(var_is_reg(dest)){
                fprintf(fasm,"\t%s\t%s,[%s+4*%s]\n","mov",dest,"eax",src2);
            }else{
                fprintf(fasm,"\t%s\t%s,[%s+4*%s]\n","mov","eax","eax",src2);
                fprintf(fasm,fmt2,dest,"eax");
            }
        }else{
            fprintf(fasm,fmt2,"mov","ecx",src2);
            if(var_is_reg(dest)){
                fprintf(fasm,"\t%s\t%s,[%s+4*%s]\n","mov",dest,"eax","ecx");
            }else{
                fprintf(fasm,"\t%s\t%s,[%s+4*%s]\n","mov","eax","eax","ecx");
                fprintf(fasm,fmt2,"mov",dest,"eax");
            }
        }
        break;
    default:
        fprintf(fasm,"不合法指令\n");
    }
}
void opt_gen_asm_code(){
    int para_count=0;
    char ce_name[VAR_LEN];
    int var_flag=0;
    int is_main;
    int i,j;
    struct opt_quat_struct* p;
    fprintf(fasm,"%s",".code\n");
    for(i=0;i<g_funct_num;i++){
        for(j=0;j<functs[i].block_num;j++){
            p=functs[i].blocks[j].block_begin;
            if(j==0&&(p->op==op_func||p->op==op_main)){
                if(p->op==op_main){
                    fprintf(fasm,"main PROC\n");
                    p=p->next;
                    is_main=1;
                }else{
                    reconvert_name(ce_name,p->dest);
                    fprintf(fasm,"%s PROC\n",ce_name);
                    p=p->next;
                    is_main=0;
                }
                if(opt_gen_asm_local_data(i)>0){
                    var_flag=1;
                }else{
                    fprintf(fasm,"\t%s\t%s\n","push","ebp");
                    fprintf(fasm,"\t%s\t%s,%s\n","mov","ebp","esp");
                    var_flag=0;
                }
                fprintf(fasm,"\t%s\t%s\n","push","ebx");
                fprintf(fasm,"\t%s\t%s\n","push","edi");
                fprintf(fasm,"\t%s\t%s\n","push","esi");//被调用者保护现场
                p=p->next;
                while(p!=NULL&&(p->op==op_var_dcl||p->op==op_array_dcl)){
                    p=p->next;
                }
            }
            while(p!=NULL){
                switch(p->op){
                case op_emain:
                    fprintf(fasm,"main ENDP\n");
                    fprintf(fasm,"END main\n");
                    p=p->next;
                    break;
                case op_efunc:
                    reconvert_name(ce_name,p->dest);
                    fprintf(fasm,"%s ENDP\n",ce_name);
                    p=p->next;
                    break;
                case op_para:
                case op_call:
                    para_count=0;
                    while(p->op==op_para){//参数压栈
                        reconvert_name(ce_name,p->dest);//TODO ，参数可能是寄存器值
                        fprintf(fasm,"\t%s\t%s\n","push",ce_name);
                        para_count++;
                        p=p->next;
                    }//可能无参
                    if(p->op==op_call){//传参后面正常来说是函数调用
                        reconvert_name(ce_name,p->dest);
                        fprintf(fasm,"\t%s\t%s\n","call",ce_name);
                        if(para_count){//弹参数
                            fprintf(fasm,"\t%s\t%s,%d\n","add","esp",4*para_count);
                        }
                        p=p->next;
                    }
                    break;
                default:
                    opt_gen_instruction(i,p,var_flag,is_main);
                    p=p->next;
                }
            }
        }
    }
}

#endif // GENASM_H_INCLUDED
