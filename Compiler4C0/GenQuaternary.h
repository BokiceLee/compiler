#ifndef GENQUATERNARY_H_INCLUDED
#define GENQUATERNARY_H_INCLUDED
#define LABEL_LEN 20
#define LABEL_TABLE_LEN 1000
#define VAR_LEN 20
#define QUAT_TABLE_LEN 1000
#define NAME_LEN 20
#define SW_BR_NUM 20
#define MAX_PARA 20
#define OP_LEN 31
enum op_code{
    op_add=0,op_sub,op_mul,op_idiv,//3
    op_mov,//4
    op_para,//5
    op_call,op_arr_assign,op_ret_void,op_ret_value,//9
    op_beq,op_bne,op_ble,op_bls,op_bgt,op_bge,//15
    op_jump,//16
    op_prints,op_printi,op_printc,op_scanfc,op_scanfi,//21
    op_set_label,//22
    op_var_dcl,op_array_dcl,//24
    op_load_ret,//25
    op_func,op_efunc,op_main,op_emain,//29
    op_arr_get,//30
    op_leaf//31 Ò¶½Úµã
};
char* op_name[OP_LEN]={
    "add",
    "sub",
    "mul",
    "idiv",
    "mov",
    "para",
    "call",
    "arr_assign",
    "ret_void",
    "ret_value",
    "beq",
    "bne",
    "ble",
    "bls",
    "bgt",
    "bge",
    "jump",
    "prints",
    "printi",
    "printc",
    "scanfc",
    "scanfi",
    "set_label",
    "var_dcl",
    "array_dcl",
    "load_ret",
    "func",
    "efunc",
    "main",
    "emain",
    "arr_get"
};
struct quat_struct{
    enum op_code op;
    char dest[VAR_LEN];
    char src1[VAR_LEN];
    char src2[VAR_LEN];
};
//enum compare_type{
//    cmp_eql,cmp_neq,cmp_leq,cmp_lss,cmp_gtr,cmp_geq,cmp_bgz,cmp_blez
//};
char label_table[LABEL_TABLE_LEN][LABEL_LEN];
int label_index=-1;
struct quat_struct quat_table[QUAT_TABLE_LEN];
int quat_index=-1;
int name_index=-1;
int gen_quaternary(int op,char dest[],char src1[],char src2[]);
void gen_name(char name[]);
int gen_lab();
void insert2name(char name[],int c,int typ);
void convert_name(char dest[],char src[],int is_global);
void para_name(char dest[],int n);
void gen_string_name(char name[]);
int gen_quaternary(int op,char dest[],char src1[],char src2[]);
int get_para_index(int pos);
void convert_output(char name[]);
void outputquat();
int gen_lab(){
    sprintf(label_table[label_index],"label%03d",++label_index);
    return label_index;
}
void gen_name(char name[]){
    sprintf(name,"&%03d",++name_index);
}
void convert_name(char dest[],char src[],int is_global){
    if(is_global){
        sprintf(dest,"%s",src);
    }else{
        sprintf(dest,"@_%s",src);
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
        fprintf(fquat,"%s  ",name);
        break;
    case '&':
        fprintf(fquat,"%s  ",name);
        break;
    case '*':
        fprintf(fquat,"%s  ",name);
        break;
    case '$':
        fprintf(fquat,"%s  ",name);
        break;
    case '#':
        fprintf(fquat,"%s ",name);
        break;
    case '^':
        fprintf(fquat,"%s ",name);
        break;
    case '\0':
        fprintf(fquat,"__  ");
        break;
    default:
        fprintf(fquat,"%s   ",name);
    }
}
void outputquat(){
    int i=0;
    int op_n;
    //return;
    fquat=fopen(fquat_name,"w");
    for(i=0;i<=quat_index;i++){
        fprintf(fquat,"%d  ",i);
        op_n=quat_table[i].op;
        fprintf(fquat,"%s  ",op_name[op_n]);
        convert_output(quat_table[i].dest);
        convert_output(quat_table[i].src1);
        convert_output(quat_table[i].src2);
        fprintf(fquat,"\n");
    }
}

#endif // GENQUATERNARY_H_INCLUDED
