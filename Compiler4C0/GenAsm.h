#include "GenQuaternary.h"
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


struct local_reg_var_struct{
    enum regs reg;
    char var_name[VAR_LEN];
};
struct local_reg_var_struct local_reg_var[LOCAL_REG_NUM];
enum regs reg_pool[LOCAL_REG_NUM]={edx,ecx,eax};
int reg_pool_x=3;
int reg_var_x=0;
int r_tmp_var_flag[R_TMP_VAR_NUM];
//申请临时寄存器
int sendback_reg(char dest[],char other1[],char other2[]){//返回置空表项
    int i;
    for(i=0;i<LOCAL_REG_NUM;i++){
        if(strcmp(dest,local_reg_var[i].var_name)!=0&&strcmp(other1,local_reg_var[i].var_name)!=0&&strcmp(other2,local_reg_var[i].var_name)!=0){
            reg_pool[reg_pool_x++]=local_reg_var[i].reg;
            return i;
        }
    }
}
int apply_reg(char dest[],char other1[],char other2[]){
    int i;
    if(reg_pool_x==0){
        reg_var_x=sendback_reg(dest,other1,other2);
    }
    local_reg_var[reg_var_x].reg=reg_pool[--reg_pool_x];
    strcpy(dest,local_reg_var[reg_var_x].var_name);
}

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
    opt_gen_asm_data(1);
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
        if(quat_table[qx].op==op_var_dcl){
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
    char* r_s="\tlocal\t@r_tmp_var_%03d\t\n"
    int i;
    int lc_data_num;
    struct opt_quat_struct* p;
    lc_data_num=opt_gen_asm_data(0,p);
    for(i=0;i<functs[funcx].block_num;i++){//如果已分配全局变量，则不需要分配
        p=functs[funcx].blocks[i];
        if(p->dest[0]=='&'){
            if(p->dest[1]=='r'){
                if(r_tmp_var_flag[atoi(p->dest+2)]==0){
                    fprintf(fasm,r_s,atoi(p->dest+2));
                    r_tmp_var_flag[atoi(p->dest+2)]=1;
                    lc_data_num++;
                }
            }else{
                if(tmp_var_flag[atoi(quat_table[i].dest+1)]==0){
                    fprintf(fasm,s,atoi(1+quat_table[i].dest));
                    tmp_var_flag[atoi(quat_table[i].dest+1)]=1;
                    lc_data_num++;
                }
            }
        }
    }
    return lc_data_num;
}
void opt_gen_instruction(struct opt_quat_struct* p){
    char dest[VAR_LEN];
    char src1[VAR_LEN];
    char src2[VAR_LEN];
    char* fmt0="\t%s\n";
    char* fmt1="\t%s\t%s\n";
    char* fmt2="\t%s\t%s,%s\n";
    opt_reconvert_name(dest,quat.dest);
    opt_reconvert_name(src1,quat.src1);
    opt_reconvert_name(src2,quat.src2);
    switch(p->op){
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
        if(p->src2[0]=='$'||p->src2[0]=='^'){
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
void opt_gen_asm_code(){

}
void opt_reconvert_name(){
}
#endif // GENASM_H_INCLUDED
