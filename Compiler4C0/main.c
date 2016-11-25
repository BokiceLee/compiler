#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<ctype.h>
#include "Global.h"
#include "Error.h"
#include "GenAsm.h"
#include "LexicalAnalysis.h"
#include "Optimization.h"
#include "SemanticAnalysis.h"
#include "SymbolManagement.h"
#include "GenQuaternary.h"
////global////
int output2where_console_0_file_1=0;
int optimization=0;
int return_temp=0;
int end_flag=0;
////词法分析////
int row_in_source_file=1;
int column_in_source_file=0;
////输入输出////
FILE *fsource;//源代码文件指针
FILE *ftarget;//目标代码输出文件指针
FILE *fasm;
char fsourcename[MAX_LEN_OF_FILE];//源代码文件名称
char ftargetname[MAX_LEN_OF_FILE];//目标代码文件名称
char fasm_name[]="C:\\Users\\24745\\Desktop\\res.asm";
////词法分析////
char ch;//读到的字符
enum symbol sym;
char token[LEN_OF_NAME];
int num_read=0;
int read_ch_len=0;
////lobal////
void setInputOutput(){
    set_in:
    printf("please input the absolute path of your source file(space is not available):\n");
    //scanf("%s",fsourcename);
    fsource=fopen("E:\\StudyInSchool\\compiler\\Homework\\judgehw02\\easy_test.txt","r");
    if(NULL==fsource){
        printf("no such file\n");
        goto set_in;
    }
    set_out:
    printf("input 0/1,0 for output result to console,1 for output result to file and you will have to input a file path\n");
    //scanf("%d",&output2where_console_0_file_1);
    if(0){//output2where_console_0_file_1){
        printf("please input the absolute path of target file you expect:\n");
        scanf("%s",ftargetname);
        ftarget=fopen(ftargetname,"w");
        if(NULL==ftarget){
            printf("please input the correct absolute path of target file\n");
            goto set_out;
        }
    }else{
        ftarget=stdout;
    }
}
void finish_compile(){
    fclose(fsource);
    if(output2where_console_0_file_1){
        fclose(ftarget);
    }
    //fclose(fasm);
}
////符号管理////
int i_temp;
enum types typ_var_funct;
char ident_name_var_funct[LEN_OF_NAME];

int ident_index=0;

char string_tab[LEN_OF_STRING_TAB][LEN_OF_STRING];
int string_table_index=0;
int string_index=0;

struct global_ident_tab_item global_ident_tab[IDENT_TAB_LEN];
struct local_ident_tab_item local_ident_tab[IDENT_TAB_LEN];
int global_ident_index=0;
int local_ident_index=0;
int last_local_ident_index=-1;
struct funct_tab_item funct_tab[LEN_OF_FUNC_TAB];
int funct_tab_index=0;
int funct_index=-1;//记录当前函数在函数表中的索引
int array_tab[LEN_OF_ARRAY_TAB];
int array_tab_index=0;
int position_res_global_flag;


////生成四元式////
char label_table[LABEL_TABLE_LEN][LABEL_LEN];
int label_index=-1;
struct quat_struct quat_table[QUAT_TABLE_LEN];
int quat_index=-1;
int name_index=-1;
int gen_lab(){
    sprintf(label_table[label_index],"label%03d",++label_index);
    return label_index;
}
void gen_name(char name[]){
    sprintf(name,"&%d",++name_index);
}
void convert_name(char dest[],char src[],int is_global){
    if(is_global){
        sprintf(dest,"%s",src);
    }else{
        sprintf(dest,"@%s",src);
    }
}
void para_name(char dest[],int n){
    sprintf(dest,"^%d",n);
}
void insert2name(char name[],int c,int typ){
    if(typ==chars){
        sprintf(name,"#%c",c);
    }else{
        sprintf(name,"$%d",c);
    }
}
void gen_string_name(char name[]){
    sprintf(name,"*%d",string_table_index);
}
int gen_quaternary(int op,char dest[],char src1[],char src2[]){
    quat_table[++quat_index].op=op;
    strcpy(quat_table[quat_index].dest,dest);
    strcpy(quat_table[quat_index].src1,src1);
    strcpy(quat_table[quat_index].src2,src2);
    return quat_index;
}
int get_para_index(int pos){
    int i=1;
    while(local_ident_tab[pos].link!=-1){
        i++;
        pos=local_ident_tab[pos].link;
    }
    return i;
}
void convert_output(char name[]){
    switch(name[0]){
    case '@':
        printf("%s  ",name);
        break;
    case '&':
        printf("%s  ",name);
        break;
    case '*':
        printf("%s  ",name);
        break;
    case '$':
        printf("%s  ",name);
        break;
    case '#':
        printf("%s ",name);
        break;
    case '^':
        printf("%s ",name);
        break;
    case '\0':
        printf("__  ");
        break;
    default:
        printf("%s   ",name);
    }
}

void outputquat(){
    int i=0;
    int op_n;
    //return;
    for(i=0;i<=quat_index;i++){
        printf("%d  ",i);
        op_n=quat_table[i].op;
        printf("%s  ",op_name[op_n]);
        convert_output(quat_table[i].dest);
        convert_output(quat_table[i].src1);
        convert_output(quat_table[i].src2);
        printf("\n");
    }
}
////生成汇编////
int qx=0;
void gen_asm(){
    fasm=fopen(fasm_name,"w");
    gen_asm_head();
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
    for(i=0;i<string_table_index;i++){
        fprintf(fasm,"\t__STR_%03d\tDB\t'%s',0\n",i,string_tab[i]);
    }
    fprintf(fasm,"\t_fmt_out_s\tDB\t'%%s',0\n");
    fprintf(fasm,"\t_fmt_out_c\tDB\t'%%c',0\n");
    fprintf(fasm,"\t_fmt_out_i\tDB\t'%%d',0\n");
    fprintf(fasm,"\t_fmt_in_c\tDB\t'%%c',0\n");
    fprintf(fasm,"\t_fmt_in_i\tDB\t'%%d',0\n");
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
    char* s="\tlocal\t@_tmp_var_%d\t\n";
    int i;
    int cur_num=-1;
    gen_asm_data(0);
    for(i=qx;quat_table[i].op!=op_efunc && quat_table[i].op!=op_emain;i++){
        if(quat_table[i].dest[0]=='&'){
            if(atoi(quat_table[i].dest+1)>cur_num){
                fprintf(fasm,s,atoi(1+quat_table[i].dest));
                cur_num=atoi(1+quat_table[i].dest);
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
        fprintf(fasm,fmt1,"idiv",src2);
        fprintf(fasm,fmt2,"mov",dest,"eax");
        break;
    case op_mov:
        fprintf(fasm,fmt2,"mov","eax",src1);
        fprintf(fasm,fmt2,"mov",dest,"eax");
        break;
    case op_push:
        fprintf(fasm,fmt1,"push",dest);
        break;
    case op_pop:
        fprintf(fasm,fmt1,"pop","eax");
        break;
    case op_call:
        fprintf(fasm,fmt1,"call",dest);
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
        fprintf(fasm,fmt0,"leave");
        fprintf(fasm,fmt0,"ret");
        break;
    case op_ret_value:
        fprintf(fasm,fmt2,"mov","eax",dest);
        fprintf(fasm,fmt0,"leave");
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
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_printf","ADDR","_fmt_out_s",dest);
        }else{
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_printf","OFFSET","_fmt_out_s",dest);
        }
        break;
    case op_printi:
        if(dest[0]=='@'){
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_printf","ADDR","_fmt_out_i",dest);
        }else{
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_printf","OFFSET","_fmt_out_i",dest);
        }
        break;
    case op_printc:
        if(dest[0]=='@'){
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_printf","ADDR","_fmt_out_c",dest);
        }else{
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_printf","OFFSET","_fmt_out_c",dest);
        }
        break;
    case op_scanfc:
        if(dest[0]=='@'){
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_scanf","ADDR","_fmt_in_c",dest);
        }else{
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_scanf","OFFSET","_fmt_in_c",dest);
        }
        break;
    case op_scanfi:
        if(dest[0]=='@'){
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_scanf","ADDR","_fmt_in_i",dest);
        }else{
            fprintf(fasm,"\t%s\t%s,%s %s,%s\n","invoke","crt_scanf","OFFSET","_fmt_in_i",dest);
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
    }
}
void gen_asm_code(){
    fprintf(fasm,"%s",".code\n");
    while(quat_table[qx].op==op_func||quat_table[qx].op==op_main){
        if(quat_table[qx].op==op_main){
            fprintf(fasm,"main PROC\n");
            qx++;
        }else{
            fprintf(fasm,"_%s PROC\n",quat_table[qx].dest);
            qx++;
        }
        gen_asm_local_data();
        while(quat_table[qx].op!=op_efunc&&quat_table[qx].op!=op_emain){
            gen_instruction(quat_table[qx]);
            qx++;
        }
        if(quat_table[qx].op==op_emain){
            fprintf(fasm,"\tinvoke ExitProcess,0\n");
            fprintf(fasm,"main ENDP\n");
            fprintf(fasm,"END main\n");
            qx++;
        }else{
            fprintf(fasm,"_%s ENDP\n",quat_table[qx].dest);
            qx++;
        }
        if(qx>=quat_index){
            break;
        }
    }
}

////符号表管理////
int position(char tmp_token[],int *is_global){
    int i;
    for(i=local_ident_index-1;i>=0;i--){
        if(strcmp(tmp_token,local_ident_tab[i].name)==0){
            position_res_global_flag=0;
            *is_global=0;
            return i;
        }
    }
    for(i=global_ident_index-1;i>=0;i--){
        if(strcmp(tmp_token,global_ident_tab[i].name)==0){
            position_res_global_flag=1;
            *is_global=1;
            return i;
        }
    }
    return i;
}
void enter_ident(int is_global,char name[],int obj,int typ,int refer,int adr){
    if(ident_index>=IDENT_TAB_LEN){
        fatal();
    }else{
        if(is_global){
            strcpy(global_ident_tab[global_ident_index].name,name);
            global_ident_tab[global_ident_index].obj=obj;
            global_ident_tab[global_ident_index].typ=typ;
            global_ident_tab[global_ident_index].refer=refer;
            global_ident_tab[global_ident_index].adr=adr;
            global_ident_index++;
        }else{
            strcpy(local_ident_tab[local_ident_index].name,name);
            local_ident_tab[local_ident_index].obj=obj;
            local_ident_tab[local_ident_index].typ=typ;
            local_ident_tab[local_ident_index].refer=refer;
            local_ident_tab[local_ident_index].adr=adr;
            local_ident_tab[local_ident_index].link=last_local_ident_index;
            last_local_ident_index=local_ident_index;
            local_ident_index++;
        }
    }
}
void enter_array(int high){
    array_tab[array_tab_index++]=high;
}
int enter_funct(int last,int lastpar,int psize,int vsize){
    funct_tab[funct_tab_index].last=last;
    funct_tab[funct_tab_index].lastpar=lastpar;
    funct_tab[funct_tab_index].psize=psize;
    funct_tab[funct_tab_index].vsize=vsize;
    return funct_tab_index++;
}
////出错处理////
int errors=0;
int end_right=0;
////出错处理////
void error(int error_code){
    switch(error_code){
    case -1:
        return;
    case 0:
        token[--read_ch_len]='\0';
        break;
    case 1:
        break;
    case 2:
        num_read=0;
        break;
    case 3:
        break;
    case 4:
        break;
    case 5:
        break;
    case 6:
        break;
    case 7:
        break;
    case 8:
        break;
    }
    printf("error at row %d,column %d:\n",row_in_source_file,column_in_source_file);
    printf("error:%d ~~~~~error message:%s\n",++errors,errormsg[error_code]);
}
int search_sym_in_set(int sym_set[],int set_len){
    int i;
    int flag=0;
    for(i=0;i<set_len;i++){
        if(sym_set[i]==sym){
            flag=1;
            break;
        }
    }
    return flag;
}
void skip(int sym_set[],int set_len,int err_code){
    error(err_code);
    while(!search_sym_in_set(sym_set,set_len)){
        getNextSym();
    }
}
int merge_sym_set(int s1[],int s1_len,int s2[],int s2_len){
    int i;
    int j;
    int res=s1_len;
    if(s2_len==0){
        return s1_len;
    }
    for(j=0;j<s2_len;j++){
        for(i=0;i<s1_len;i++){
            if(s1[i]==s2[j]){
                break;
            }
        }
        if(i>=s1_len){
            s1[res++]=s2[j];
        }
    }
    return res;
}
void test(int legal_set[],int legal_set_len,int stop_set[],int stop_set_len,int err_code){
    int new_set[SET_LEN];
    int new_set_len;
    if(!search_sym_in_set(legal_set,legal_set_len)){
        new_set_len=merge_sym_set(new_set,0,legal_set,legal_set_len);
        new_set_len=merge_sym_set(new_set,new_set_len,stop_set,stop_set_len);
        skip(new_set,new_set_len,err_code);
    }
}
void needsym(int sym_need){
    if(sym!=sym_need){
        switch(sym_need){
            case semicolon:
                error(15);
                break;
            case rparent:
                error(17);
                break;
            case rbrack:
                error(18);
                break;
            case rquote:
                error(19);
                break;
            default:
                ;
        }
    }else{
        getNextSym();
    }
}
void fatal(){
    ;
}
////词法分析////
void clearTmpToken(char tmp_Token[]){
    int i;
    for(i=0;i<LEN_OF_NAME;i++){
        tmp_Token[i]='\0';
    }
}
void clearToken(){
    while(read_ch_len>=0){
        token[read_ch_len--]='\0';
    }
    read_ch_len=0;
}
int is_reversed(char s[]){
    int left=0;
    int right=REVERSED_NUM-1;
    int mid;
    int compare_res;
    strlwr(s);
    do{
        mid=(left+right)/2;
        compare_res=strcmp(s,reversed_table[mid]);
        switch(compare_res){
        case -1:
            right=mid-1;
            break;
        case 0:
            return mid;
        case 1:
            left=mid+1;
            break;
        }
    }while(left<=right);
    return 0;
}
int is_empty_char(char c){
    if(c==' '||c=='\n'||c=='\t'){
        return 1;
    }
    return 0;
}
int is_add_op_char(char c){
    if('+'==c||'-'==c){
        return 1;
    }
    return 0;
}
int is_mul_op_char(char c){
    if('/'==c||'*'==c){
        return 1;
    }
    return 0;
}
int is_letter(char c){
    if(c=='_'){
        return 1;
    }
    if('a'<=c&&c<='z'){
        return 1;
    }
    if('A'<=c&&c<='Z'){
        return 1;
    }
    return 0;
}
int is_not_zero_digital(char c){
    if('1'<=c && c<='9'){
        return 1;
    }
    return 0;
}
int is_right_char_in_string(char c){
    if(35<=c && c<=126){
        return 1;
    }
    if(c==32 || c==33){
        return 1;
    }
    return 0;
}
int is_digital(char c){
    if(is_not_zero_digital(c)){
        return 1;
    }
    if(c=='0'){
        return 1;
    }
    return 0;
}
void getNextCh(){
    if(end_flag){
        return;
    }
    if(return_temp==1){
        return_temp=0;
        row_in_source_file++;
        column_in_source_file=0;
    }
    if((ch=fgetc(fsource))==EOF){
        end_flag=1;
        column_in_source_file++;
    }else if(ch=='\n'){
        return_temp=1;
        column_in_source_file++;
    }else if(ch=='\t'){
        column_in_source_file+=4;
    }else{
        column_in_source_file++;
    }
    //system("pause");
}
void getNextSym(){
    int reversed_id;
    while(is_empty_char(ch)){
        getNextCh();
    }
    clearToken();
    if(is_letter(ch)){
        //开始读token
        do{
            token[read_ch_len++]=ch;
            getNextCh();
        }while(is_letter(ch)||is_digital(ch));
        token[read_ch_len]='\0';
        reversed_id=is_reversed(token);
        if(reversed_id==0){
            sym=ident;
        }else{
            sym=reversed_id;
        }
    }else if(is_digital(ch)){
        sym=intcon;
        num_read=0;
        //开始读数字
        do{
            if(read_ch_len==1&&token[0]=='0'){
                error(0);
            }
            token[read_ch_len++]=ch;
            getNextCh();
        }while(is_digital(ch));
        if(is_letter(ch)){
            error(1);
        }
        token[read_ch_len]='\0';
        num_read=atoi(token);
        if(read_ch_len>MAX_LEN_OF_NUM || num_read>MAX_NUM){
            error(2);
        }
    }else{
        switch(ch){
        case '+':
            sym=pluss;
            getNextCh();
            break;
        case '-':
            sym=minuss;
            getNextCh();
            break;
        case '*':
            sym=times;
            getNextCh();
            break;
        case '/':
            sym=idiv;
            getNextCh();
            break;
        case ',':
            sym=comma;
            getNextCh();
            break;
        case ':':
            sym=colon;
            getNextCh();
            break;
        case ';':
            sym=semicolon;
            getNextCh();
            break;
        case '(':
            sym=lparent;
            getNextCh();
            break;
        case ')':
            sym=rparent;
            getNextCh();
            break;
        case '[':
            sym=lbrack;
            getNextCh();
            break;
        case ']':
            sym=rbrack;
            getNextCh();
            break;
        case '{':
            sym=lquote;
            getNextCh();
            break;
        case '}':
            sym=rquote;
            getNextCh();
            break;
        case '\''://??????????
            sym=charcon;
            getNextCh();
            if(!(is_add_op_char(ch)||is_mul_op_char(ch)||is_digital(ch)||is_letter(ch))){
                if(ch=='\''){
                    error(9);
                    token[0]='\0';
                    read_ch_len++;
                    getNextCh();
                    break;
                }else{
                    error(3);
                }
            }
            token[0]=ch;
            do{
                getNextCh();
                read_ch_len++;
            }while(ch!='\'');
            if(read_ch_len>1){
                error(4);
            }
            getNextCh();
            break;
        case '"':
            string_index=0;
            do{
                getNextCh();
                if(ch!='"'){
                    if(is_right_char_in_string(ch)){
                        string_tab[string_table_index][string_index++]=ch;
                    }else{
                        error(5);
                    }
                }else{
                    string_tab[string_table_index][string_index]='\0';
                    string_table_index++;
                }
            }while(ch!='"');
            getNextCh();
            sym=stringcon;
            break;
        case '!':
            getNextCh();
            if(ch=='='){
                sym=neq;
                getNextCh();
            }else{
                error(6);
            }
            break;
        case '=':
            getNextCh();
            if(ch=='='){
                sym=eql;
                getNextCh();
            }else{
                sym=becomes;
            }
            break;
        case '<':
            getNextCh();
            if(ch=='='){
                sym=leq;
                getNextCh();
            }else{
                sym=lss;
            }
            break;
        case '>':
            if(ch=='='){
                sym=geq;
                getNextCh();
            }else{
                sym=gtr;
            }
            break;
        case -1:
            sym=-1;
            break;
        default:
            error(7);
            sym=illegalcon;
            token[read_ch_len++]=ch;
            getNextCh();
        }
    }
}
////语法分析////
void program(){
    int stop_set[SET_LEN]={intsy,charsy,voidsy};
    int stop_set_len=3;
    if(sym==constsy){
        constdeclaraction(stop_set,stop_set_len,1);
    }
    if(sym==intsy||sym==charsy){
        vardeclaraction(stop_set,stop_set_len,1);
    }
    if(sym==lparent){
        funct_ret_declaraction(stop_set,stop_set_len,1);
    }
    while(sym==intsy||sym==charsy||sym==voidsy){
        if(sym==voidsy){
            getNextSym();
            funct_void_declaraction(stop_set,stop_set_len,1);
        }else{
            funct_ret_declaraction(stop_set,stop_set_len,1);
        }
        if(sym!=mainsy){
            test(stop_set,stop_set_len,NULL,0,31);
        }
    }
    funct_main_declaraction(1,NULL,0);
    printf("程序\n");
    if(sym==-1){
        printf("程序正确结束\n");
    }else{
        printf("程序未正确结束");
    }
}
void constdeclaraction(int fsys[],int fsys_len,int is_global){
    int stop_set[SET_LEN]={semicolon};
    int stop_set_len=1;
    int stop_set2[SET_LEN]={semicolon,constsy};
    int stop_set2_len=2;
    do{
        if(sym==constsy){//可以不要
            getNextSym();
        }
        if(sym==intsy||sym==charsy){
            stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
            constdefinition(stop_set,stop_set_len,is_global);
            needsym(semicolon);
            stop_set2_len=merge_sym_set(stop_set2,stop_set2_len,fsys,fsys_len);
            test(stop_set2,stop_set2_len,NULL,0,31);
        }else{
            error(14);
            stop_set2_len=merge_sym_set(stop_set2,stop_set2_len,fsys,fsys_len);
            test(stop_set,stop_set_len,fsys,fsys_len,-1);
            needsym(semicolon);
        }
        printf("常量说明\n");
    }while(sym==constsy);
}
void constdefinition(int fsys[],int fsys_len,int is_global){
    enum types ident_typ;
    int redcl_flag=0;
    char tmp_token[LEN_OF_NAME];
    int value=0;
    int positive;
    int stop_set[SET_LEN]={comma,semicolon};
    int stop_set_len=1;
    if(sym==intsy){//进来肯定是常量声明，sym肯定是int或char
        ident_typ=ints;
    }else{
        ident_typ=chars;
    }
    do{
        getNextSym();
        if(sym==ident){
            redcl_flag=check_redeclaraction(is_global,token);
            if(redcl_flag){
                error(36);//跳到下一个comma
                test(stop_set,stop_set_len,fsys,fsys_len,-1);
            }else{
                strcpy(tmp_token,token);
                getNextSym();
                if(sym==becomes){
                    positive=1;
                    getNextSym();
                    if(sym==pluss||sym==minuss){
                        if(sym==minuss){
                            positive=-1;
                        }
                        getNextSym();
                        if(sym!=intcon){
                            error(39);//跳到comma
                            test(stop_set,stop_set_len,fsys,fsys_len,-1);
                        }else if(num_read==0){
                            error(16);//不管
                        }
                    }
                    if(sym!=intcon&&sym!=charcon){
                        error(23);//跳到comma
                        test(stop_set,stop_set_len,fsys,fsys_len,-1);
                    }
                    if(sym==intcon && ident_typ==ints){
                        enter_ident(is_global,tmp_token,con,ints,0,num_read);
                        getNextSym();
                    }else if(sym==charcon && ident_typ==chars){
                        value=token[0];
                        enter_ident(is_global,tmp_token,con,chars,0,value);
                        getNextSym();
                    }else if(sym==chars && ident_typ==ints){
                        error(26);//不管
                        value=token[0];
                        enter_ident(is_global,tmp_token,con,ints,0,value);
                        getNextSym();
                    }else {
                        error(11);//comma
                        test(stop_set,stop_set_len,fsys,fsys_len,-1);
                    }
                    test(stop_set,stop_set_len,fsys,fsys_len,-1);
                }else{
                    error(12);//comma
                    test(stop_set,stop_set_len,fsys,fsys_len,-1);
                }
            }
        }else{
            error(13);//comma
            test(stop_set,stop_set_len,fsys,fsys_len,-1);
        }
        printf("常量定义\n");
    }while(sym==comma);
}
void vardeclaraction(int fsys[],int fsys_len,int is_global){
    enum types ident_typs;
    int stop_set[SET_LEN]={semicolon,lparent};
    int stop_set_len=2;
    if(is_global){
        stop_set[stop_set_len++]=intsy;
        stop_set[stop_set_len++]=charsy;
        stop_set[stop_set_len++]=voidsy;
    }
    do{
        if(sym==intsy){
            ident_typs=ints;
        }else{
            ident_typs=chars;
        }
        stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
        vardefinition(stop_set,stop_set_len,is_global);
        if(sym==lparent){
            break;
        }else{
            needsym(semicolon);
        }
        if(is_global){
            test(stop_set,stop_set_len,fsys,fsys_len,31);
        }
    }while(sym==intsy||sym==charsy);
}
void vardefinition(int fsys[],int fsys_len,int is_global){
    enum types ident_typs;
    int redcl_flag=0;
    char tmp_token[LEN_OF_NAME];
    int stop_set[SET_LEN]={comma,semicolon};
    int stop_set_len=2;
    int stop_set2[SET_LEN]={intcon};
    int stop_set2_len=1;
    char iname[VAR_LEN];
    char array_len[VAR_LEN];
    if(sym==intsy){
        ident_typs=ints;
    }else{
        ident_typs=chars;
    }
    //读到标识符时循环
    do{
        getNextSym();
        if(sym==ident){
            redcl_flag=check_redeclaraction(is_global,token);
            if(redcl_flag){
                error(36);
                test(stop_set,stop_set_len,fsys,fsys_len,-1);
            }else{
                strcpy(tmp_token,token);
                getNextSym();
                if(sym==lparent){
                    typ_var_funct=ident_typs;
                    strcpy(ident_name_var_funct,tmp_token);
                    break;
                }
                if(sym!=lbrack){
                    enter_ident(is_global,tmp_token,var,ident_typs,0,0);
                    convert_name(iname,tmp_token,is_global);
                    gen_quaternary(op_var_dcl,iname,"","");
                    test(stop_set,stop_set_len,fsys,fsys_len,45);
                }else{
                    getNextSym();
                    if(sym!=intcon){
                        error(20);
                        test(stop_set2,stop_set2_len,fsys,fsys_len,-1);
                        if(sym==intcon){
                            goto int_label;
                        }
                    }else{
                    int_label:
                        if(num_read==0){
                            error(20);//不管
                        }
                        enter_ident(is_global,tmp_token,arrays,ident_typs,array_tab_index,0);
                        enter_array(num_read);//？
                        convert_name(iname,tmp_token,is_global);
                        insert2name(array_len,num_read,ints);
                        gen_quaternary(op_array_dcl,iname,array_len,"");
                        getNextSym();
                        needsym(rbrack);
                        test(stop_set,stop_set_len,fsys,fsys_len,45);
                    }
                }
            }
        }
        printf("变量定义\n");
    }while(sym==comma);
}
void funct_void_declaraction(int fsys[],int fsys_len){
    int refer=0;
    int adr=0;
    int redcl_flag=0;
    int lastpar;
    char tmp_token[LEN_OF_NAME];
    int stop_set2[SET_LEN]={rquote};
    int stop_set2_len=1;
    int stop_set3[SET_LEN]={rparent};
    int stop_set3_len=1;
    if(sym==mainsy){
        return;
    }
    if(sym==ident){
        redcl_flag=check_redeclaraction(1,token);
        if(redcl_flag){
            error(36);//跳到下一个函数声明
            getNextSym();
            test(fsys,fsys_len,NULL,0,-1);
            return;
        }else{
            strcpy(tmp_token,token);
            getNextSym();
            if(sym!=lparent){
                error(22);//跳到下一个函数
                test(fsys,fsys_len,NULL,0,-1);
                return;
            }else{
                gen_quaternary(op_func,tmp_token,"","");
                last_local_ident_index=-1;
                stop_set3_len=merge_sym_set(stop_set3,stop_set3_len,fsys,fsys_len);
                getNextSym();
                parameterlist(stop_set3,stop_set3_len);
                needsym(rparent);
                lastpar=last_local_ident_index;
                refer=enter_funct(0,lastpar,0,0);
                enter_ident(1,tmp_token,func,notyp,refer,adr);
                if(sym!=lquote){
                    error(24);//不管
                }else{
                    getNextSym();
                }
                stop_set2_len=merge_sym_set(stop_set2,stop_set2_len,fsys,fsys_len);
                compound_statement(stop_set2,stop_set2_len);
                needsym(rquote);//不管,应该不会出现
                funct_tab[refer].last=last_local_ident_index;
                gen_quaternary(op_ret_void,"","","");
                gen_quaternary(op_efunc,tmp_token,"","");
            }
        }
        printf("无返回值函数定义\n");
    }else{//非标识符
        error(13);//跳到下一个函数
        test(fsys,fsys_len,NULL,0,-1);
    }
}
void funct_ret_declaraction(int fsys[],int fsys_len){
    enum types typ;
    int refer;
    int adr=0;
    int redcl_flag=0;
    int lastpar;
    char tmp_token[LEN_OF_NAME];
    int stop_set2[SET_LEN]={rquote};
    int stop_set2_len=1;
    int stop_set3[SET_LEN]={lquote};
    int stop_set3_len=1;
    if(sym==intsy){
        typ=ints;
        getNextSym();
    }else if(sym==chars){
        typ=chars;
        getNextSym();
    }
    //如果是main，报错
    if(sym==ident){
        redcl_flag=check_redeclaraction(1,token);
        if(redcl_flag){
            error(36);
            getNextSym();
            test(fsys,fsys_len,NULL,0,-1);
            return;
        }else{
            strcpy(tmp_token,token);
        }
        getNextSym();
    }else if(sym==lparent){
        if(ident_name_var_funct[0]=='\0'){
            error(28);
            test(fsys,fsys_len,NULL,0,-1);
        }else{
            strcpy(tmp_token,ident_name_var_funct);
            clearTmpToken(ident_name_var_funct);
        }
    }else{
        if(sym==mainsy){
            error(44);
        }else{
            error(13);
        }
        test(fsys,fsys_len,NULL,0,-1);
        return;
    }
    if(sym!=lparent){
        error(22);
        test(fsys,fsys_len,NULL,0,-1);
    }else{
        gen_quaternary(op_func,tmp_token,"","");
        last_local_ident_index=-1;
        stop_set3_len=merge_sym_set(stop_set3,stop_set3_len,fsys,fsys_len);
        getNextSym();
        parameterlist(stop_set3,stop_set3_len);
        lastpar=last_local_ident_index;
        needsym(rparent);
        refer=enter_funct(0,lastpar,0,0);
        enter_ident(1,tmp_token,func,typ,refer,adr);
        if(sym!=lquote){
            error(24);//不管
        }else{
            getNextSym();
        }
        stop_set2_len=merge_sym_set(stop_set2,stop_set2_len,fsys,fsys_len);
        compound_statement(stop_set2,stop_set2_len);
        needsym(rquote);
        funct_tab[refer].last=last_local_ident_index;
        gen_quaternary(op_ret_value,"$0","","");
        gen_quaternary(op_efunc,tmp_token,"","");
        printf("有返回值函数定义\n");
    }
}
void funct_main_declaraction(int fsys[],int fsys_len){
    int refer;
    int adr=0;
    int stop_set[SET_LEN]={rquote};
    int stop_set_len=1;
    int stop_set3[SET_LEN]={lquote};
    int stop_set3_len=1;
    if(sym==mainsy){
        getNextSym();
        if(sym!=lparent){
            error(22);//不管
        }else{
            getNextSym();
        }
        gen_quaternary(op_main,"","","");
        last_local_ident_index=-1;
        stop_set3_len=merge_sym_set(stop_set3,stop_set3_len,fsys,fsys_len);
        parameterlist(stop_set3,stop_set3_len);
        if(last_local_ident_index!=-1){
            error(22);
        }
        needsym(rparent);
        if(sym!=lquote){
            error(24);
        }else{
            getNextSym();
        }
        refer=enter_funct(0,-1,0,0);
        merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
        compound_statement(stop_set,stop_set_len);
        enter_ident(1,"main",func,notyp,refer,adr);
        funct_tab[refer].last=last_local_ident_index-1;
        //enter_ident(token,is_global,func,notyp,refer,adr);
        needsym(rquote);
        gen_quaternary(op_emain,"","","");
    }else{
        error(43);//不管
    }
    printf("主函数定义\n");
}
void parameterlist(int fsys[],int fsys_len){
    enum types ident_typ;
    int redcl_flag=0;
    char tmp_token[LEN_OF_NAME];
    int stop_set[SET_LEN]={rparent,comma};
    int stop_set_len=2;
    int i=0;
    do{
        if(sym==comma){
            getNextSym();
        }
        if(sym!=intsy&&sym!=charsy&&sym!=rparent){
            error(22);
            getNextSym();
            test(stop_set,stop_set_len,fsys,fsys_len,-1);
        }else if(sym==intsy||sym==charsy){
            if(sym==intsy){
                ident_typ=ints;
            }else{
                ident_typ=chars;
            }
            getNextSym();
            if(sym==ident){
                redcl_flag=check_redeclaraction(0,token);
                if(redcl_flag){
                    error(36);
                    getNextSym();
                    test(stop_set,stop_set_len,fsys,fsys_len,-1);
                }else{
                    strcpy(tmp_token,token);
                    enter_ident(0,tmp_token,paras,ident_typ,0,++i);
                    getNextSym();
                }
            }else{
                error(13);
                test(stop_set,stop_set_len,fsys,fsys_len,-1);
            }
        }
    }while(sym==comma);
    printf("参数表\n");
}
void compound_statement(int fsys[],int fsys_len){
    if(sym==constsy){
        constdeclaraction(fsys,fsys_len,0);
    }
    if(sym==intsy||sym==charsy){
        vardeclaraction(fsys,fsys_len,0);
    }
    statements(fsys,fsys_len);
   printf("复合语句\n");
}
void statement(int fsys[],int fsys_len){
    char tmp_token[LEN_OF_NAME];
    int stop_set[SET_LEN]={rquote};
    int stop_set_len=1;
    int fun_type;
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    //test(fsys,fsys_len,NULL,0,-1);
    switch(sym){
        case ifsy:
            getNextSym();
            if_statement(fsys,fsys_len);
            break;
        case whilesy:
            getNextSym();
            while_statement(fsys,fsys_len);
            break;
        case lquote:
            getNextSym();
            statements(stop_set,stop_set_len);
            needsym(rquote);
            break;
        case ident:
            strcpy(tmp_token,token);
            getNextSym();
            if(sym==lbrack||sym==becomes){
                assignment(fsys,fsys_len,tmp_token);
            }else if(sym==lparent){
                getNextSym();
                funct_call(fsys,fsys_len,tmp_token,&fun_type);
            }else{
                error(31);
                break;
            }
            needsym(semicolon);
            break;
        case scanfsy:
            getNextSym();
            scanf_statement(fsys,fsys_len);
            needsym(semicolon);
            break;
        case printfsy:
            getNextSym();
            printf_statement(fsys,fsys_len);
            needsym(semicolon);
            break;
        case switchsy:
            getNextSym();
            switch_statement(fsys,fsys_len);
            break;
        case returnsy:
            getNextSym();
            return_statement(fsys,fsys_len);
            needsym(semicolon);
            break;
        case semicolon:
            getNextSym();
            break;
        default:
            break;
    }
    test(fsys,fsys_len,NULL,0,-1);
printf("语句\n");
}
void statements(int fsys[],int fsys_len){
    char tmp_token[LEN_OF_NAME];
    int stop_set[SET_LEN]={ident,ifsy,whilesy,lquote,scanfsy,printfsy,switchsy,returnsy,semicolon,rquote};
    int stop_set_len=10;
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    while(sym!=rquote){
        if(sym==semicolon){
            getNextSym();
        }
        statement(stop_set,stop_set_len);
    }
printf("语句列\n");
}
void if_statement(int fsys[],int fsys_len){
    int stop_set1[SET_LEN]={rparent};
    int stop_set1_len=1;
    int stop_set2[SET_LEN]={elsesy};
    int stop_set2_len=1;
    int labx1,labx2;
    int code_x1,code_x2;
    if(sym!=lparent){
        error(25);
    }else{
        getNextSym();
    }
    stop_set1_len=merge_sym_set(stop_set1,stop_set1_len,fsys,fsys_len);
    code_x1=condition(stop_set1,stop_set1_len);
    needsym(rparent);
    stop_set2_len=merge_sym_set(stop_set2,stop_set2_len,fsys,fsys_len);
    statement(stop_set2,stop_set2_len);
    if(sym==elsesy){//对吗？
        code_x2=gen_quaternary(op_jump,"","","");
        labx2=gen_lab();
        gen_quaternary(op_set_label,label_table[labx2],"","");
        strcpy(quat_table[code_x1].dest,label_table[labx2]);
        getNextSym();
        statement(fsys,fsys_len);
        labx1=gen_lab();
        gen_quaternary(op_set_label,label_table[labx1],"","");
        strcpy(quat_table[code_x2].dest,label_table[labx1]);
        printf("if 中 else\n");
    }else{
        labx1=gen_lab();
        gen_quaternary(op_set_label,label_table[labx1],"","");
        strcpy(quat_table[code_x1].dest,label_table[labx1]);
    }
printf("if 语句\n");
}
void while_statement(int fsys[],int fsys_len){
    int stop_set[SET_LEN]={rparent};
    int stop_set_len=1;
    int lab_x1,lab_x2;
    int codex;
    if(sym!=lparent){
        error(25);
    }else{
        getNextSym();
    }
    lab_x1=gen_lab();
    gen_quaternary(op_set_label,label_table[lab_x1],"","");
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    codex=condition(stop_set,stop_set_len);
    needsym(rparent);
    statement(fsys,fsys_len);
    gen_quaternary(op_jump,label_table[lab_x1],"","");
    lab_x2=gen_lab();
    gen_quaternary(op_set_label,label_table[lab_x2],"","");
    strcpy(quat_table[codex].dest,label_table[lab_x2]);
    printf("while 语句\n");
}
int condition(int fsys[],int fsys_len){
    enum symbol tmp_sym;
    int stop_set[SET_LEN]={eql,neq,gtr,geq,lss,leq};
    int stop_set_len=6;
    char sname1[VAR_LEN],sname2[VAR_LEN];
    int stype1,stype2;
    int code_x;
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    simpleexpression(stop_set,stop_set_len,&stype1,sname1);
    if(sym==eql||sym==neq||sym==gtr||sym==geq||sym==lss||sym==leq){
        tmp_sym=sym;
        getNextSym();
        simpleexpression(fsys,fsys_len,&stype2,sname2);
        switch(tmp_sym){
        case eql://根据不同情况生成指令
            code_x=gen_quaternary(op_bne,"",sname1,sname2);
            break;
        case neq:
            code_x=gen_quaternary(op_beq,"",sname1,sname2);
            break;
        case gtr:
            code_x=gen_quaternary(op_ble,"",sname1,sname2);
            break;
        case geq:
            code_x=gen_quaternary(op_bls,"",sname1,sname2);
            break;
        case lss:
            code_x=gen_quaternary(op_bge,"",sname1,sname2);
            break;
        case leq:
            code_x=gen_quaternary(op_bgt,"",sname1,sname2);
            break;
        }
    }else{
        code_x=gen_quaternary(op_ble,"",sname1,"$0");
    }
    //打标签
    printf("if或while判断条件 \n");
    return code_x;
}
void assignment(int fsys[],int fsys_len,char tmp_token[]){
    int stop_set[SET_LEN]={becomes};
    int stop_set_len=1;
    int is_global;
    int res_position=position(tmp_token,&is_global);
    int stype;
    char sname[VAR_LEN];
    int seltype;
    char selname[VAR_LEN];
    char target_name[VAR_LEN];
    char basename[VAR_LEN];
    int is_arr=0;
    //tmp_token可能是数组
    if(sym==lbrack){
        is_arr=1;
        getNextSym();
        stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
        selector(stop_set,stop_set_len,&seltype,selname);
        convert_name(basename,tmp_token,is_global);
    }
    if(sym!=becomes){
        error(12);
        test(fsys,fsys_len,NULL,0,-1);
    }else{
        if(res_position==-1){
            error(32);
            test(fsys,fsys_len,NULL,0,-1);
        }else{
            getNextSym();
            simpleexpression(fsys,fsys_len,&stype,sname);
            if(is_arr==0){
                convert_name(target_name,tmp_token,is_global);
                gen_quaternary(op_mov,target_name,sname,"");
            }else{
                gen_quaternary(op_arr_assign,basename,selname,sname);
            }
            //插入运算指令
    //        if(ret_typ==ints && ident_tab[res_position].typ==ints){
    //        }else if(ret_typ==chars && ident_tab[res_position].typ==chars){
    //        }else if(ret_typ==ints && ident_tab[res_position].typ==ints){
    //        }else{
    //            error(11);//报错之后继续插入
    //        }
        }
    }
    printf("赋值语句\n");
}
void switch_statement(int fsys[],int fsys_len){
    int stop_set[SET_LEN]={rparent};
    int stop_set_len=1;
    int stop_set2[SET_LEN]={rquote};
    int stop_set2_len=1;
    char sname[VAR_LEN];
    int stype;
    if(sym!=lparent){
        error(25);
    }else{
        getNextSym();
    }
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    simpleexpression(stop_set,stop_set_len,&stype,sname);
    needsym(rparent);
    if(sym!=lquote){
        error(24);
    }else{
        getNextSym();
    }
    stop_set2_len=merge_sym_set(stop_set2,stop_set2_len,fsys,fsys_len);
    caselabel(stop_set2,stop_set2_len,&stype,sname);//onst?
    needsym(rquote);
    printf("情况语句\n");
}
void caselabel(int fsys[],int fsys_len,int *stype,char sname[]){
    int stop_set[SET_LEN]={casesy,defaultsy};
    int stop_set_len=2;
    int con_name[VAR_LEN];
    int lab_finish;
    int labx[SW];
    int i=0;
    int case_begin[SW],case_end[SW];
    test(stop_set,stop_set_len,fsys,fsys_len,27);
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    if(sym==casesy){
        do{
            getNextSym();
            if(sym!=intcon&&sym!=charcon){
                error(40);//重标签同处理
                test(stop_set,2,fsys,fsys_len,-1);
            }else{
                //分号
                if(sym==intcon){
                    insert2name(con_name,num_read,ints);
                }else{
                    insert2name(con_name,token[0],chars);
                }
                labx[i]=gen_lab();
                gen_quaternary(op_set_label,label_table[labx[i]],"","");
                case_begin[i]=gen_quaternary(op_bne,"",sname,con_name);
                getNextSym();
                onecase(stop_set,stop_set_len);
                case_end[i]=gen_quaternary(op_jump,"","","");
                i++;
                //打标签
            }
        }while(sym==casesy);
    }else if(sym==defaultsy){
        error(27);
    }
    if(sym==defaultsy){
        labx[i]=gen_lab();
        gen_quaternary(op_set_label,label_table[labx[i]],"","");
        getNextSym();
        defaultcase(fsys,fsys_len);
        lab_finish=gen_lab();
        gen_quaternary(op_set_label,label_table[lab_finish],"","");
        while(i>0){
            strcpy(quat_table[case_begin[i-1]].dest,label_table[labx[i]]);
            strcpy(quat_table[case_end[i-1]].dest,label_table[lab_finish]);
            i--;
        }
    }else{
        lab_finish=gen_lab();
        labx[i]=lab_finish;
        gen_quaternary(op_set_label,label_table[lab_finish],"","");
        while(i>0){
            strcpy(quat_table[case_begin[i-1]].dest,label_table[labx[i]]);
            strcpy(quat_table[case_end[i-1]].dest,label_table[lab_finish]);
            i--;
        }
    }
    printf("情况表\n");
}
void onecase(int fsys,int fsys_len){
    if(sym!=colon){
        error(41);
    }else{
        getNextSym();
    }
    statement(fsys,fsys_len);
    printf("情况子语句\n");
}
void defaultcase(int fsys[],int fsys_len){
    if(sym!=colon){
        error(42);
    }else{
        getNextSym();
    }
    statement(fsys,fsys_len);
    printf("缺省\n");
}
void scanf_statement(int fsys[],int fsys_len){
    int stop_set[SET_LEN]={comma,rparent};
    int stop_set_len=2;
    int res_position;
    int is_global;
    int ce_name[VAR_LEN];
    if(sym!=lparent){
        error(29);
    }else{
        getNextSym();
    }
    if(sym!=ident){
        error(30);
        test(stop_set,stop_set_len,fsys,fsys_len,-1);
    }
    do{
        if(sym==comma){
            getNextSym();
        }
        if(sym!=ident){
            error(30);
            test(stop_set,stop_set_len,fsys,fsys_len,-1);
        }else{
            res_position=position(token,&is_global);
            if(res_position==-1){
                error(32);
                test(stop_set,stop_set_len,fsys,fsys_len,-1);
            }else{
                convert_name(ce_name,token,is_global);
                if(is_global){
                    if(global_ident_tab[res_position].typ==ints){
                        gen_quaternary(op_scanfi,ce_name,"","");
                    }else{
                        gen_quaternary(op_scanfc,ce_name,"","");
                    }
                }else{
                    if(local_ident_tab[res_position].typ==ints){
                        gen_quaternary(op_scanfi,ce_name,"","");
                    }else{
                        gen_quaternary(op_scanfc,ce_name,"","");
                    }
                }
                getNextSym();
            }
        }
    }while(sym==comma);
    needsym(rparent);
printf("读语句\n");
}
void printf_statement(int fsys[],int fsys_len){
    int res_position;
    int stop_set[SET_LEN]={rparent};
    int stop_set_len=1;
    char string_name[VAR_LEN];
    char sname[VAR_LEN];
    int stype;
    if(sym!=lparent){
        error(29);
    }else{
        getNextSym();
    }
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    if(sym==stringcon){
        gen_string_name(string_name);
        gen_quaternary(op_prints,string_name,"","");
        getNextSym();
        if(sym==comma){
            getNextSym();
            simpleexpression(stop_set,stop_set_len,&stype,sname);
            if(stype==ints){
                gen_quaternary(op_printi,sname,"","");
            }else{
                gen_quaternary(op_printc,sname,"","");
            }
        }
        needsym(rparent);
    }else {
        simpleexpression(stop_set,stop_set_len,&stype,sname);
        needsym(rparent);
        if(stype==ints){
            gen_quaternary(op_printi,sname,"","");
        }else{
            gen_quaternary(op_printc,sname,"","");
        }
    }
    printf("写语句\n");
}
int factor(int fsys[],int fsys_len,int *ftype,char fname[]){
    int res_position;
    char tmp_token[LEN_OF_NAME];
    int legal_set[SET_LEN]={lparent,ident,intcon,charcon};
    int legal_set_len=4;
    int stop_set[SET_LEN]={rparent};
    int stop_set_len=1;
    int is_global;
    int seltype;
    int selname[VAR_LEN];
    char basename[VAR_LEN];
    int para_n=0;
    //test
    test(legal_set,legal_set_len,fsys,fsys_len,31);
    if(sym==ident||sym==intcon||sym==charcon){
        if(sym==ident){
            strcpy(tmp_token,token);
            res_position=position(tmp_token,&is_global);
            if(res_position==-1){
                error(32);
                getNextSym();
                test(fsys,fsys_len,NULL,0,-1);
            }else{
                getNextSym();
                if(sym==lbrack){//数组
                    getNextSym();
                    selector(fsys,fsys_len,&seltype,selname);
                    gen_name(fname);
                    convert_name(basename,tmp_token,is_global);
                    gen_quaternary(op_arr_get,fname,basename,selname);
                }else if(sym==lparent){//函数调用
                    getNextSym();
                    funct_call(fsys,fsys_len,tmp_token,ftype);
                    gen_name(fname);
                    gen_quaternary(op_load_ret,fname,"","");
                }else{//
                    if(is_global){
                        *ftype=global_ident_tab[res_position].typ;
                        if(global_ident_tab[res_position].obj==con){
                            insert2name(fname,global_ident_tab[res_position].adr,*ftype);
                        }else{
                            convert_name(fname,tmp_token,1);
                        }
                    }else{
                        if(local_ident_tab[res_position].obj==paras){
                            para_n=local_ident_tab[res_position].adr;
                            para_name(fname,para_n);
                        }else if(local_ident_tab[res_position].obj==con){
                            insert2name(fname,local_ident_tab[res_position].adr,*ftype);
                        }else{
                            convert_name(fname,tmp_token,0);
                        }
                        *ftype=local_ident_tab[res_position].typ;
                    }
                }
            }
        }else{
            if(sym==charcon){
                insert2name(fname,token[0],chars);
                *ftype=chars;
            }else{
                insert2name(fname,num_read,ints);
                *ftype=ints;
            }
            getNextSym();
        }
    }else if(sym==lparent){
        getNextSym();
        stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
        simpleexpression(stop_set,stop_set_len,ftype,fname);
        needsym(rparent);
    }
    printf("因子\n");
    return 0;
}
int term(int fsys[],int fsys_len,int *ttype,char tname[]){
    enum symbol fac_op;
    int stop_set[SET_LEN]={times,idiv};
    int stop_set_len=2;
    char fname1[VAR_LEN],fname2[VAR_LEN];
    int ftype1,ftype2;
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    factor(stop_set,stop_set_len,&ftype1,fname1);
    while(times==sym||idiv==sym){
        fac_op=sym;
        getNextSym();
        factor(stop_set,stop_set_len,&ftype2,fname2);
        gen_name(tname);
        if(fac_op==times){
            gen_quaternary(op_mul,tname,fname1,fname2);
        }else{
            gen_quaternary(op_idiv,tname,fname1,fname2);
        }
        strcpy(fname1,tname);
    }
    strcpy(tname,fname1);
    printf("项\n");
    return 0;
}
int simpleexpression(int fsys[],int fsys_len,int *sype,char sname[]){
    enum symbol positive;
    int stop_set[SET_LEN]={pluss,minuss};
    int stop_set_len=2;
    char tname1[VAR_LEN],tname2[VAR_LEN];
    int ttype1,ttype2;
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    if(sym==pluss||sym==minuss){
        positive=sym;
        getNextSym();
        term(stop_set,stop_set_len,&ttype1,tname1);
        if(positive==minuss){
            //插入取反操作
            gen_name(tname2);
            gen_quaternary(op_sub,tname2,"$0",tname1);
        }else{
            strcpy(tname2,tname1);
        }
    }else{
        term(stop_set,stop_set_len,&ttype2,tname2);
    }
    while(sym==pluss||sym==minuss){
        gen_name(sname);
        positive=sym;
        getNextSym();
        term(stop_set,stop_set_len,&ttype1,tname1);
        if(positive==pluss){
            //插入加指令
            gen_quaternary(op_add,sname,tname2,tname1);
        }else{
            //插入减指令
            gen_quaternary(op_sub,sname,tname2,tname1);
        }
        strcpy(tname2,sname);
    }
    strcpy(sname,tname2);
    printf("表达式\n");
    return 0;
}
void return_statement(int fsys[],int fsys_len){
    int stop_set[SET_LEN]={rparent};
    int stop_set_len=1;
    int stype;
    char sname[VAR_LEN];
    if(sym==semicolon){
        gen_quaternary(op_ret_void,"","","");
        printf("返回语句\n");
        return;
    }
    if(sym!=lparent){
        error(29);
    }else{
        getNextSym();
    }
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    simpleexpression(stop_set,stop_set_len,&stype,sname);
    needsym(rparent);
    gen_quaternary(op_ret_value,sname,"","");
    printf("返回语句\n");
}
void funct_call(int fsys[],int fsys_len,char funct_name[],int *funct_type){
    int stop_set[SET_LEN]={comma,rparent};
    int stop_set_len=2;
    int stype;
    char sname[MAX_PARA][VAR_LEN];
    int i=global_ident_index-1;
    int ref;
    int lastpar=-1;
    int par_x[MAX_PARA];
    int par_num=0;
    while(i>=0){
        if(strcmp(global_ident_tab[i].name,funct_name)==0 && global_ident_tab[i].obj==func){
            ref=global_ident_tab[i].refer;
            lastpar=funct_tab[ref].lastpar;
            *funct_type=global_ident_tab[i].typ;
            break;
        }
        i--;
    }
    if(i<0){
        printf("%s 不是函数\n",funct_name);
    }
    i=0;
    while(lastpar!=-1){
        par_x[i++]=lastpar;
        lastpar=local_ident_tab[lastpar].link;
    }
    par_num=i;
    if(par_num<=0){
        ;
    }//参数个数不匹配
    if(sym!=rparent && par_num>0){
        stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
        simpleexpression(stop_set,stop_set_len,&stype,sname[par_num-i]);
        if(local_ident_tab[par_x[i-1]].typ!=stype){
            printf("类型不匹配");
        }
        i--;
        if(sym==comma){
            do{
                if(i<0){
                    printf("函数参数个数不等");
                    break;
                }
                getNextSym();
                simpleexpression(stop_set,stop_set_len,&stype,sname[par_num-i]);
                i--;
            }while(sym==comma);
        }
        printf("值参数表\n");
    }else{
        //没有参数
        printf("空值参数表\n");
    }
    needsym(rparent);
    i=par_num;
    while(i>0){
        gen_quaternary(op_push,sname[i-1],"","");
        i--;
    }
    gen_quaternary(op_call,funct_name,"","");
    i=par_num;
    while(i>0){
        gen_quaternary(op_pop,"","","");
        i--;
    }
    printf("函数调用语句\n");
}
void selector(int fsys[],int fsys_len,int *seltype,char selname[]){
    int stop_set[SET_LEN]={rbrack};
    int stop_set_len=1;
    stop_set_len=merge_sym_set(stop_set,stop_set_len,fsys,fsys_len);
    simpleexpression(stop_set,stop_set_len,seltype,selname);
    needsym(rbrack);
    printf("数组选择器\n");
}

////语义分析////
int check_redeclaraction(int is_global,char ident_name[]){
    int check_index;
    if(is_global){
        check_index=global_ident_index-1;
        while(check_index>=0){
            if(strcmp(global_ident_tab[check_index].name,ident_name)==0){
                return 1;
            }
            check_index--;
        }
        return 0;
    }else{
        check_index=last_local_ident_index;
        while(check_index!=-1){
            if(strcmp(local_ident_tab[check_index].name,ident_name)==0){
                return 1;
            }else{
                check_index=local_ident_tab[check_index].link;
            }
        }
        return 0;
    }
    return 0;
}
//int resulttype(int &r_type,int &type1,int &type2){
//    if(*type1==*type2){
//        *r_type=type1;
//        if(*r_type!=notyp){
//            return 0;
//        }else{
//            return -1;
//        }
//    }
//    if(*type1==ints && *type2==chars){
//        *r_type=ints;
//        return 1;
//    }
//    if(*type1==chars && *type2==ints){
//        *r_type=;
//    }
//}

////代码优化////



int main()
{
    setInputOutput();
    ch=' ';
    fprintf(ftarget,"grammar analysis result:");
    getNextSym();
    program();
    while(1){
        break;
//        fprintf(ftarget,"\n");
//        getNextSym();
//        if(0<=sym && sym<=15){
//            fprintf(ftarget,reversed_table[sym]);
//            fprintf(ftarget," ");
//        }else if(16<=sym&&sym<=35){
//            fprintf(ftarget,special_symbol[sym-16]);
//            fprintf(ftarget," ");
//        }else{
//            switch(sym){
//            case ident:
//                fprintf(ftarget,"ident:");
//                fprintf(ftarget,"%s ",token);
//                break;
//            case intcon:
//                fprintf(ftarget,"numcon:");
//                fprintf(ftarget,"%d ",num_read);
//                break;
//            case charcon:
//                fprintf(ftarget,"charcon:");
//                fprintf(ftarget,"%c",token[0]);
//                break;
//            case stringcon:
//                fprintf(ftarget,"stringcon:");
//                fprintf(ftarget,"%s",string_tab[string_table_index-1]);
//                break;
//            default:
//                fprintf(ftarget,"illegal:");
//                fprintf(ftarget,"%c",token[0]);
//            }
//        }
    }
    outputquat();
    gen_asm();
    finish_compile();
    printf("12");
    return 0;
}
