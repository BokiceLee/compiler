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


struct local_reg_var_struct{
    enum regs reg;
    char var_name[VAR_LEN];
    int valid;
};
struct local_reg_var_struct local_reg_var[LOCAL_REG_NUM];
enum regs reg_pool[LOCAL_REG_NUM]={edx,ecx,eax};
int reg_pool_x=3;
int reg_var_x=0;
int r_tmp_var_flag[R_TMP_VAR_NUM];
char* reg_name[3]={"ebx","edi","esi","eax","ecx","edx"};
enum quat_types{all_same_reg,all_same_var};
//申请临时寄存器
void sendback_reg(char tmp_reg_name[]){//将不在other中出现的临时寄存器返回
    int i;
    for(i=0;i<LOCAL_REG_NUM;i++){
        if(local_reg_var[i].valid==1){
            if(strcmp(reg_name[local_reg_var[i].reg],tmp_reg_name)==0){
                fprintf(fasm,"\t%s\t%s,%s\n","mov",local_reg_var[i].var_name,reg_name[local_reg_var[i].reg]);
                local_reg_var[i].valid=0;
                reg_pool[reg_pool_x++]=local_reg_var[i].reg;
                return;
            }
        }
    }
}
int apply_reg(char tmp_reg[],char dest[],char other1[],char other2[]){
    int i;
    if(reg_pool_x==0){
        for(i=0;i<LOCAL_REG_NUM;i++){
            if(strcmp(other1,reg_name[local_reg_var[i].reg])!=0&&strcmp(other2,reg_name[local_reg_var[i].reg])!=0){
                sendback_reg(reg_name[local_reg_var[i].reg]);
                break;
            }
        }
    }
    local_reg_var[reg_var_x].reg=reg_pool[--reg_pool_x];
    local_reg_var[reg_var_x].valid=1;
    strcpy(tmp_reg,reg_name[local_reg_var[reg_var_x].reg]);

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
    char* r_s="\tlocal\t@r_tmp_var_%03d\t\n";
    int i;
    int lc_data_num;
    struct opt_quat_struct* p;
    lc_data_num=opt_gen_asm_data(0,p);
    for(i=0;i<functs[funcx].block_num;i++){
        p=functs[funcx].blocks[i].block_begin;
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
int is_reg(char rrr[]){
    int i;
    for(i=0;i<6;i++){
        if(strcmp(rrr,reg_name[i])==0){
            return 1;
        }
    }
    return 0;
}
int is_global_reg(char vvv[]){
    int i;
    for(i=0;i<3;i++){
        if(strcmp(vvv,reg_name[i])==0){
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
        if(is_reg(src)){
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
    for(i=0;i<functs[funcx].var_reg_len;i++){
        if(strcmp(p->dest,functs[funcx].var_reg[i].var)==0){
            strcpy(lc_dest,reg_name[functs[funcx].var_reg[i].reg]);
        }
        if(strcmp(p->src1,functs[funcx].var_reg[i].var)==0){
            strcpy(lc_src1,reg_name[functs[funcx].var_reg[i].reg]);
        }
        if(strcmp(p->src2,functs[funcx].var_reg[i].var)==0){
            strcpy(lc_src2,reg_name[functs[funcx].var_reg[i].reg]);
        }
    }
    for(i=0;i<3;i++){
        if(local_reg_var[i].valid==1){
            if(strcmp(p->dest,local_reg_var[i].var_name)==0){
                strcpy(lc_dest,reg_name[local_reg_var[i].reg+3]);
            }
            if(strcmp(p->src1,local_reg_var[i].var_name)==0){
                strcpy(lc_src1,reg_name[local_reg_var[i].reg+3]);
            }
            if(strcmp(p->dest,local_reg_var[i].var_name)==0){
                strcpy(lc_src2,reg_name[local_reg_var[i].reg+3]);
            }
        }
    }
    opt_sub_reconvert(dest,lc_dest);
    opt_sub_reconvert(src1,lc_src1);
    opt_sub_reconvert(src2,lc_src2);

}

void opt_gen_instruction(int funcx,struct opt_quat_struct* p,int var_flag,int is_main){
    char dest[VAR_LEN];
    char src1[VAR_LEN];
    char src2[VAR_LEN];
    char* fmt0="\t%s\n";
    char* fmt1="\t%s\t%s\n";
    char* fmt2="\t%s\t%s,%s\n";
    char tmp_reg[LOCAL_REG_NUM][3];
    opt_reconvert_name(funcx,p,dest,src1,src2);//能转换的全转换
    switch(p->op){
    case op_add:
        if(strcmp(dest,src1)==0){
            if(is_reg(src1)){
                fprintf(fasm,fmt2,"add",dest,src2);
            }else if(n_var(src2)){//不是内存值
                fprintf(fasm,fmt2,"add",dest,src2);
            }else{
                apply_reg(tmp_reg,src2,dest,src1);//位置0为替换结果，位置1为替换目标，申请一个未在其他位置出现的临时寄存器
                fprintf(fasm,fmt2,"add",dest,tmp_reg);
            }
        }else{
            apply_reg(tmp_reg,src1,dest,src2);
            fprintf(fasm,fmt2,"mov",tmp_reg,src1);
            fprintf(fasm,fmt2,"add",tmp_reg,src2);
            fprintf(fasm,fmt2,"mov",dest,tmp_reg);
        }
        break;
    case op_sub:
        if(strcmp(dest,src1)==0){
            if(is_reg(src1)){
                fprintf(fasm,fmt2,"sub",dest,src2);
            }else if(n_var(src2)){
                fprintf(fasm,fmt2,"sub",dest,src2);
            }else{
                apply_reg(src2,src2,dest,src1);
                fprintf(fasm,fmt2,"sub",dest,src2);
            }
        }else{
            apply_reg(tmp_reg,src1,dest,src2);
            fprintf(fasm,fmt2,"mov",tmp_reg,src1);
            fprintf(fasm,fmt2,"sub",tmp_reg,src2);
            fprintf(fasm,fmt2,"mov",dest,tmp_reg);
        }
        break;
    case op_mul:
        if(strcmp(dest,src1)==0){
            if(is_reg(src1)){
                fprintf(fasm,fmt2,"imul",dest,src2);
            }else if(n_var(src2)){
                fprintf(fasm,fmt2,"imul",dest,src2);
            }else{
                apply_reg(src2,src2,dest,src1);
                fprintf(fasm,fmt2,"imul",dest,src2);
            }
        }else{
            apply_reg(tmp_reg,src1,dest,src2);
            fprintf(fasm,fmt2,"mov",tmp_reg,src1);
            fprintf(fasm,fmt2,"imul",tmp_reg,src2);
            fprintf(fasm,fmt2,"mov",dest,tmp_reg);
        }
        break;
    case op_idiv:
        sendback_reg("edx");
        if(strcmp(src1,"eax")!=0){
            sendback_reg("eax");
            fprintf(fasm,fmt2,"mov","eax",src1);
        }
        fprintf(fasm,fmt0,"cdq");
        if(p->src2[0]=='$'||p->src2[0]=='^'){
            if(is_reg(src2)){
                strcpy(tmp_reg,src2);
            }else{
                apply_reg(tmp_reg,src2,dest,src1);
                fprintf(fasm,fmt2,"mov",tmp_reg,src2);
                fprintf(fasm,fmt1,"idiv",tmp_reg);
                sendback_reg(tmp_reg);
            }
        }else{
            fprintf(fasm,fmt1,"idiv",src2);
        }
        fprintf(fasm,fmt2,"mov",dest,tmp_reg);
        break;
    case op_mov:
        if(p->dest[0]!='$'&&p->dest[0]!='#'&&!is_reg(dest)){
            if(p->src1[0]!='$'&&p->src1[0]!='#'&&!is_reg(src1)){
                apply_reg(tmp_reg,src1,dest,"");//如无实际意义，结束时应退回
                fprintf(fasm,fmt2,"mov",tmp_reg,src1);
                fprintf(fasm,fmt2,"mov",dest,tmp_reg);
                break;
            }
        }
//        if(quat_table[qx].src1[0]=='$'&&quat_table[qx].dest[0]!='^'){
        fprintf(fasm,fmt2,"mov",dest,src1);
        break;
    case op_arr_assign:
        apply_reg(tmp_reg,"",src1,src2);
        fprintf(fasm,"\t%s\t%s,[%s]\n","lea",tmp_reg,dest);
        if(is_reg(src1)){
            strcpy(tmp_reg[1],src1);
        }else{
            apply_reg(tmp_reg[1],src1,src2,"");
            fprintf(fasm,fmt2,"mov",tmp_reg[1],src1);
        }
        if(is_reg(src2)){
            strcpy(tmp_reg[1],src2);
        }else{
            apply_reg(tmp_reg[2],src2,src1,"");
            fprintf(fasm,fmt2,"mov",tmp_reg[2],src2);
        }
        fprintf(fasm,"\t%s\t[%s+4*%s],%s\n","mov",tmp_reg[0],tmp_reg[1],tmp_reg[2]);
        sendback_reg(tmp_reg[0]);
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
        if(strcmp(dest,"eax")==0){
            ;
        }else{
            sendback_reg("eax");
            fprintf(fasm,fmt2,"mov","eax",dest);
        }
        if(var_flag==0){
            fprintf(fasm,"\t%s\t%s,%s\n","mov","esp","ebp");
            fprintf(fasm,"\t%s\t%s\n","pop","ebp");
        }
        fprintf(fasm,fmt0,"ret");
        break;
    case op_beq:
        if(!is_reg(src1)&&!is_reg(src2)){
            apply_reg(tmp_reg[0],src1,dest,src2);
            fprintf(fasm,fmt2,"mov",tmp_reg[0],src1);
            fprintf(fasm,fmt2,"cmp",tmp_reg[0],src2);
            fprintf(fasm,fmt1,"je",dest);
            break;
        }
        fprintf(fasm,fmt2,"cmp",src1,src2);
        fprintf(fasm,fmt1,"je",dest);
        break;
    case op_bne:
        if(!is_reg(src1)&&!is_reg(src2)){
            apply_reg(tmp_reg[0],src1,dest,src2);
            fprintf(fasm,fmt2,"mov",tmp_reg[0],src1);
            fprintf(fasm,fmt2,"cmp",tmp_reg[0],src2);
            fprintf(fasm,fmt1,"jne",dest);
            break;
        }
        fprintf(fasm,fmt2,"cmp",src1,src2);
        fprintf(fasm,fmt1,"jne",dest);
        break;
    case op_ble:
        if(!is_reg(src1)&&!is_reg(src2)){
            apply_reg(tmp_reg[0],src1,dest,src2);
            fprintf(fasm,fmt2,"mov",tmp_reg[0],src1);
            fprintf(fasm,fmt2,"cmp",tmp_reg[0],src2);
            fprintf(fasm,fmt1,"jle",dest);
            break;
        }
        fprintf(fasm,fmt2,"cmp",src1,src2);
        fprintf(fasm,fmt1,"jle",dest);
        break;
    case op_bls:
        if(!is_reg(src1)&&!is_reg(src2)){
            apply_reg(tmp_reg[0],src1,dest,src2);
            fprintf(fasm,fmt2,"mov",tmp_reg[0],src1);
            fprintf(fasm,fmt2,"cmp",tmp_reg[0],src2);
            fprintf(fasm,fmt1,"jle",dest);
            break;
        }
        fprintf(fasm,fmt2,"cmp",src1,src2);
        fprintf(fasm,fmt1,"jle",dest);
        break;
    case op_bgt:
        if(!is_reg(src1)&&!is_reg(src2)){
            apply_reg(tmp_reg[0],src1,dest,src2);
            fprintf(fasm,fmt2,"mov",tmp_reg[0],src1);
            fprintf(fasm,fmt2,"cmp",tmp_reg[0],src2);
            fprintf(fasm,fmt1,"jg",dest);
            break;
        }
        fprintf(fasm,fmt2,"cmp",src1,src2);
        fprintf(fasm,fmt1,"jg",dest);
        break;
    case op_bge:
        if(!is_reg(src1)&&!is_reg(src2)){
            apply_reg(tmp_reg[0],src1,dest,src2);
            fprintf(fasm,fmt2,"mov",tmp_reg[0],src1);
            fprintf(fasm,fmt2,"cmp",tmp_reg[0],src2);
            fprintf(fasm,fmt1,"jge",dest);
            break;
        }
        fprintf(fasm,fmt2,"cmp",src1,src2);
        fprintf(fasm,fmt1,"jge",dest);
        break;
    case op_jump:
        fprintf(fasm,fmt1,"jmp",dest);
        break;
    case op_prints:
        fprintf(fasm,fmt1,"push","eax");
        fprintf(fasm,fmt1,"push","ecx");
        fprintf(fasm,fmt1,"push","edx");
        if(dest[0]=='@'){
            fprintf(fasm,"\t%s\t%s,%s %s,%s %s\n","invoke","crt_printf","OFFSET","_fmt_out_s","OFFSET",dest);
        }else{
            fprintf(fasm,"\t%s\t%s,%s %s,%s %s\n","invoke","crt_printf","OFFSET","_fmt_out_s","OFFSET",dest);
        }
        fprintf(fasm,fmt1,"pop","edx");
        fprintf(fasm,fmt1,"pop","ecx");
        fprintf(fasm,fmt1,"pop","eax");
        break;
    case op_printi:
        if(p->dest[0]=='^'){
            apply_reg(tmp_reg[0],"","","");
            fprintf(fasm,fmt2,"mov",tmp_reg[0],dest);
            fprintf(fasm,fmt1,"push","eax");
            fprintf(fasm,fmt1,"push","ecx");
            fprintf(fasm,fmt1,"push","edx");
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_printf","OFFSET","_fmt_out_i",tmp_reg[0]);
            fprintf(fasm,fmt1,"pop","edx");
            fprintf(fasm,fmt1,"pop","ecx");
            fprintf(fasm,fmt1,"pop","eax");
        }else{
            fprintf(fasm,fmt1,"push","eax");
            fprintf(fasm,fmt1,"push","ecx");
            fprintf(fasm,fmt1,"push","edx");
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_printf","OFFSET","_fmt_out_i",dest);
            fprintf(fasm,fmt1,"pop","edx");
            fprintf(fasm,fmt1,"pop","ecx");
            fprintf(fasm,fmt1,"pop","eax");
        }
        break;
    case op_printc:
        if(p->dest[0]=='^'){
            apply_reg(tmp_reg[0],"","","");
            fprintf(fasm,fmt2,"mov",tmp_reg[0],dest);
            fprintf(fasm,fmt1,"push","eax");
            fprintf(fasm,fmt1,"push","ecx");
            fprintf(fasm,fmt1,"push","edx");
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_printf","OFFSET","_fmt_out_c",tmp_reg[0]);
            fprintf(fasm,fmt1,"pop","edx");
            fprintf(fasm,fmt1,"pop","ecx");
            fprintf(fasm,fmt1,"pop","eax");
        }else{
            fprintf(fasm,fmt1,"push","eax");
            fprintf(fasm,fmt1,"push","ecx");
            fprintf(fasm,fmt1,"push","edx");
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_printf","OFFSET","_fmt_out_c",dest);
            fprintf(fasm,fmt1,"pop","edx");
            fprintf(fasm,fmt1,"pop","ecx");
            fprintf(fasm,fmt1,"pop","eax");
        }
        break;
    case op_scanfc:
        if(dest[0]=='@'){
            if(is_reg(dest)&&!is_global_reg(dest)){
                sendback_reg(dest);
            }
            fprintf(fasm,"\t%s\t%s,%s %s,%s %s\n","invoke","crt_scanf","OFFSET","_fmt_in_c","ADDR",p->dest);
            if(is_global_reg(dest)){
                fprintf(fasm,fmt2,"mov",dest,p->dest);
            }
        }else{
            if(is_reg(dest)&&!is_global_reg(dest)){
                sendback_reg(dest);
            }
            fprintf(fasm,"\t%s\t%s,%s %s,%s %s\n","invoke","crt_scanf","OFFSET","_fmt_in_c","OFFSET",p->dest);
        }
        break;
    case op_scanfi:
        if(dest[0]=='@'){
            if(is_reg(dest)&&!is_global_reg(dest)){
                sendback_reg(dest);
            }
            fprintf(fasm,"\t%s\t%s,%s %s,%s %s\n","invoke","crt_scanf","OFFSET","_fmt_in_i","ADDR",p->dest);
            if(is_global_reg(dest)){
                fprintf(fasm,fmt2,"mov",dest,p->dest);
            }
        }else{
            if(is_reg(dest)&&!is_global_reg(dest)){
                sendback_reg(dest);
            }
            fprintf(fasm,"\t%s\t%s,%s %s,%s %s\n","invoke","crt_scanf","OFFSET","_fmt_in_i","OFFSET",p->dest);
        }
        break;
    case op_set_label:
        fprintf(fasm,"%s:\n",dest);
        break;
    case op_load_ret:
        fprintf(fasm,fmt2,"mov",dest,"eax");
        break;
    case op_arr_get:
        apply_reg(tmp_reg[1],"",dest,src2);
        fprintf(fasm,"\t%s\t%s,[%s]\n","lea",tmp_reg[1],src1);
        if(!is_reg(src2)){
            apply_reg(tmp_reg[2],src2,dest,"");
            fprintf(fasm,fmt2,"mov",tmp_reg[2],src2);
        }else{
            strcpy(tmp_reg[2],src2);
        }
        fprintf(fasm,"\t%s\t%s,[%s+4*%s]\n","mov",dest,tmp_reg[1],tmp_reg[2]);
//        fprintf(fasm,fmt2,"imul","eax","4");
//        fprintf(fasm,fmt2,"add","eax","ebx");
//        fprintf(fasm,"\t%s\t%s,dword ptr [%s]","mov",dest,"eax");
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
    fprintf(fasm,"%s",".code\n");

    int i,j;
    struct opt_quat_struct* p;
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
                    sendback_reg("eax");
                    fprintf(fasm,"\t%s\t%s\n","push","ecx");
                    fprintf(fasm,"\t%s\t%s\n","push","edx");//调用者保护现场
                    para_count=0;
                    while(p->op==op_para){//参数压栈
                        reconvert_name(ce_name,p->dest);
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
                    fprintf(fasm,"\t%s\t%s\n","pop","edx");
                    fprintf(fasm,"\t%s\t%s\n","pop","ecx");
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
