#include "GenQuaternary.h"
#ifndef GENASM_H_INCLUDED
#define GENASM_H_INCLUDED
#define INSTRU_LEN 40
#define TMP_VAR_NUM 1000

int qx=0;
int tmp_var_flag[TMP_VAR_NUM];
void gen_asm();
void gen_asm_head();
void gen_asm_data(int is_glocal);
void gen_asm_local_data();
void gen_instruction(struct quat_struct quat);
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
        fprintf(fasm,"\t_STR_%03d\tDB\t\"%s\",0\n",i,string_tab[i]);
    }
    fprintf(fasm,"\t_fmt_out_s\tDB\t'%%s',0\n");
    fprintf(fasm,"\t_fmt_out_c\tDB\t'%%c',0\n");
    fprintf(fasm,"\t_fmt_out_i\tDB\t'%%d',0\n");
    fprintf(fasm,"\t_fmt_in_c\tDB\t'%%c',0\n");
    fprintf(fasm,"\t_fmt_in_i\tDB\t'%%d ',0\n");//读数字后面有空格隔开
}
void gen_asm_data(int is_global){
    char* s[]={"\tlocal\t%s\n","\t_%s\tDD\t0\n"};
    char* dup_s[]={"\tlocal\t%s[%d]\n","\t_%s\tDD\t%d\tDUP(0)\n"};
    while(quat_table[qx].op==op_var_dcl||quat_table[qx].op==op_array_dcl){
        if(quat_table[qx].op==op_var_dcl){
            fprintf(fasm,s[is_global],quat_table[qx].dest);
        }else{
            fprintf(fasm,dup_s[is_global],quat_table[qx].dest,atoi(quat_table[qx].src1+1));
        }
        qx++;
    }
}
void gen_asm_local_data(){
    char* s="\tlocal\t@_tmp_var_%03d\t\n";
    int i;
    gen_asm_data(0);
    for(i=qx;quat_table[i].op!=op_efunc && quat_table[i].op!=op_emain;i++){
        if(quat_table[i].dest[0]=='&'){
            if(tmp_var_flag[atoi(quat_table[i].dest+1)]==0){
                fprintf(fasm,s,atoi(1+quat_table[i].dest));
                tmp_var_flag[atoi(quat_table[i].dest+1)]=1;
            }
        }
    }
}
void gen_instruction(struct quat_struct quat){
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
        fprintf(fasm,fmt0,"ret");
        break;
    case op_ret_value:
        fprintf(fasm,"\t%s\t%s\n","pop","esi");
        fprintf(fasm,"\t%s\t%s\n","pop","edi");
        fprintf(fasm,"\t%s\t%s\n","pop","ebx");//被调用者恢复现场
        fprintf(fasm,fmt2,"mov","eax",dest);
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
            fprintf(fasm,fmt1,"mov","eax",dest);
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
    fprintf(fasm,"%s",".code\n");
    while(quat_table[qx].op==op_func||quat_table[qx].op==op_main){
        if(quat_table[qx].op==op_main){
            fprintf(fasm,"main PROC\n");
            qx++;
        }else{
            reconvert_name(ce_name,quat_table[qx].dest);
            fprintf(fasm,"%s PROC\n",ce_name);
            qx++;
        }
        gen_asm_local_data();
        fprintf(fasm,"\t%s\t%s\n","push","ebx");
        fprintf(fasm,"\t%s\t%s\n","push","edi");
        fprintf(fasm,"\t%s\t%s\n","push","esi");//被调用者保护现场
        while(quat_table[qx].op!=op_efunc&&quat_table[qx].op!=op_emain){
            if(quat_table[qx].op!=op_para && quat_table[qx].op!=op_call){
                gen_instruction(quat_table[qx]);
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
            fprintf(fasm,"\tinvoke ExitProcess,0\n");
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
//void gen_instruction(struct quat_struct quat);
#endif // GENASM_H_INCLUDED
